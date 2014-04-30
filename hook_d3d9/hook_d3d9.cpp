#include "hook_d3d9.h"
#include "../common/common.h"

HookD3D9::HookD3D9(LPDIRECT3DDEVICE9 dev) : device(dev), font(0)
{
	D3DDEVICE_CREATION_PARAMETERS params;
	device->GetCreationParameters(&params);

	windowHandle = params.hFocusWindow;
	//infoText = "D3D9 Hooked";

	D3DXCreateFont(device, 22, 0, FW_REGULAR, 0, 0, 1, 0, 0, 0 | FF_DONTCARE, TEXT("Arial"), &font);
}


int HookD3D9::AddTexture(const std::string& fileName, int width, int height)
{
	D3DXIMAGE_INFO d3dxImageInfo;
	LPDIRECT3DTEXTURE9 texture = 0;
	LPD3DXSPRITE sprite = 0;

	if(D3DXCreateTextureFromFileEx(device, 
		fileName.c_str(), 
		width, 
		height, 
		0,
		D3DPOOL_DEFAULT,
		D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),
		&d3dxImageInfo,
		NULL,
		&texture) != D3D_OK)
	{
		FILE_LOG(logERROR) << "Error loading texture " << fileName;
	}

	textures.push_back(texture);

	D3DXCreateSprite(device, &sprite);
	sprites.push_back(sprite);

	return sprites.size() - 1;
}

void HookD3D9::DrawSprite(unsigned int texId, long left, long top, long right, long bottom, float posX,
						  float posY, int r, int g, int b, int a)
{
	RECT destRect = {left, top, right, bottom};
	LPD3DXSPRITE sprite = sprites[texId];
	LPDIRECT3DTEXTURE9 texture = textures[texId];
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 pos(posX, posY, 0.0f);

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

    sprite->Draw(texture, NULL, NULL, &pos, D3DCOLOR_RGBA(r, g, b, a));

	sprite->End();
}

void HookD3D9::Render()
{
	RECT srcRect = {0, 0, 300, 300};
	font->DrawTextA(NULL, infoText.c_str(), (int)strlen(infoText.c_str()), &srcRect, 0, D3DCOLOR_ARGB(134, 144, 255, 0));
}

HINSTANCE ghInstance = NULL;
char dlldir[320];
typedef IDirect3D9* (__stdcall *DIRECT3DCREATE9)(unsigned int);
typedef long (__stdcall *PRESENT9)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, void*);
typedef long (__stdcall *DIP9)(IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

PRESENT9 g_D3D9_Present = 0;
DIP9 g_D3D9_DIP = 0;
BYTE g_codeFragment_p9[5] = {0, 0, 0, 0, 0};
BYTE g_jmp_p9[5] = {0, 0, 0, 0, 0};
BYTE g_jmp_DIP[5] = {0, 0, 0, 0, 0};
BYTE g_codeFragment_DIP[5] = {0, 0, 0, 0, 0};
DWORD dip9 = 0;
DWORD g_savedProtection_DIP = 0;
DWORD g_savedProtection_p9 = 0;
DWORD present9 = 0;

HookD3D9* hook = 0;

void GetDevice9Methods()
{
	HWND hWnd = CreateWindowExA(0, "STATIC", "dummy", 0, 0, 0, 0, 0, 0, 0, 0, 0);
	HMODULE hD3D9 = LoadLibrary("d3d9");
	DIRECT3DCREATE9 Direct3DCreate9 = (DIRECT3DCREATE9)GetProcAddress(hD3D9, "Direct3DCreate9");
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = 1;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	IDirect3DDevice9* d3dDevice = 0;
    d3d->CreateDevice(0, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice);
	DWORD* vtablePtr = (DWORD*)(*((DWORD*)d3dDevice));
	present9 = vtablePtr[17] - (DWORD)hD3D9;
	d3dDevice->Release();
	d3d->Release(); 
	FreeLibrary(hD3D9);
	CloseHandle(hWnd);
}

long __stdcall HookedPresent9(IDirect3DDevice9* pDevice, const RECT* src, const RECT* dest, HWND hWnd, void* unused)
{
	BYTE* codeDest = (BYTE*)g_D3D9_Present;
	codeDest[0] = g_codeFragment_p9[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(g_codeFragment_p9 + 1));

	if(!hook)
	{
		hook = new HookD3D9(pDevice);
		CommonAPI::getInstance()->Init(hook);
	}
	else
	{
		pDevice->BeginScene();
		hook->Render();
		CommonAPI::getInstance()->OnRender();
		pDevice->EndScene();
	}


	DWORD res = g_D3D9_Present(pDevice, src, dest, hWnd, unused);
	codeDest[0] = g_jmp_p9[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(g_jmp_p9 + 1));
	return res;
}

long __stdcall HookedDIP9(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE type, INT baseVertexIndex,UINT minIndex,UINT numVertices, UINT startIndex, UINT primCount)
{
	BYTE* codeDest = (BYTE*)g_D3D9_DIP;
	codeDest[0] = g_codeFragment_DIP[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(g_codeFragment_DIP + 1));	
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	DWORD res = g_D3D9_DIP(pDevice, type, baseVertexIndex, minIndex, numVertices, startIndex, primCount);
	codeDest[0] = g_jmp_DIP[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(g_jmp_DIP + 1));
	return res;
}

void HookDevice9Methods()
{
	HMODULE hD3D9 = GetModuleHandle("d3d9.dll");
	g_D3D9_Present = (PRESENT9)((DWORD)hD3D9 + present9);
	g_jmp_p9[0] = 0xE9;
	DWORD addr = (DWORD)HookedPresent9 - (DWORD)g_D3D9_Present - 5;
	memcpy(g_jmp_p9 + 1, &addr, sizeof(DWORD));
	memcpy(g_codeFragment_p9, g_D3D9_Present, 5);
	VirtualProtect(g_D3D9_Present, 8, PAGE_EXECUTE_READWRITE, &g_savedProtection_p9); 
	memcpy(g_D3D9_Present, g_jmp_p9, 5);
}

DWORD __stdcall TF(void* lpParam)
{
	GetDevice9Methods();
	HookDevice9Methods();
	return 0;
}


BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	if( fdwReason == DLL_PROCESS_ATTACH )
	{
		ghInstance = hinstDLL;
		CreateThread(0, 0, &TF, 0, 0, 0);
	}

	return TRUE;
}