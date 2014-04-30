// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9tex.h>
#include <stdio.h>

// define the screen resolution
#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d; // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev; // the pointer to the device class
D3DPRESENT_PARAMETERS d3dpp; // create a struct to hold various device information
LARGE_INTEGER     TimerFreq;
LARGE_INTEGER     TimerStart;
LARGE_INTEGER     TimerEnd;
int NumFrames;
LPD3DXFONT pFont = NULL;            // pointer to the ID3DXFont interface
char fpsStr[128] = "";        // character variable to hold fps
RECT fpsRect = {900, 0, 1024, 100};      
bool deviceLost = false;

// function prototypes
void initD3D(HWND hWnd); // sets up and initializes Direct3D
void render_frame(void); // renders a single frame
void cleanD3D(void); // closes Direct3D and releases memory

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground = (HBRUSH)COLOR_WINDOW;    // not needed any more
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          "Our Direct3D Program",
                          WS_EX_TOPMOST | WS_POPUP,    // fullscreen values
                          0, 0,    // the starting x and y positions should be 0
                          SCREEN_WIDTH, SCREEN_HEIGHT,    // set the window to 640 x 480
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);
	QueryPerformanceFrequency(&TimerFreq);
	QueryPerformanceCounter(&TimerStart);


    // set up and initialize Direct3D
    initD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while(TRUE)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;

        render_frame();
    }

    // clean up DirectX and COM
    cleanD3D();

    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
		//case 0x202:
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION); // create the Direct3D interface

 

    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = FALSE;    // program fullscreen, not windowed
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = SCREEN_WIDTH;    // set the width of the buffer
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;    // set the height of the buffer


    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);
	 D3DXCreateFont(d3ddev,    // the D3D Device
                   25,    // font height
                   0,    // default font width
                   FW_NORMAL,    // font weight
                   1,    // not using MipLevels
                   false,    // italic font
                   DEFAULT_CHARSET,    // default character set
                   OUT_DEFAULT_PRECIS,    // default OutputPrecision,
                   DEFAULT_QUALITY,    // default Quality
                   DEFAULT_PITCH | FF_DONTCARE,    // default pitch and family
                   "Arial",    // use Facename Arial
                   &pFont);    // the font object
}


// this is the function used to render a single frame
void render_frame(void)
{
	HRESULT hr;
	if(deviceLost == true)
	{
		Sleep(100);
		
		if(FAILED(hr = d3ddev->TestCooperativeLevel()))
		{
			if(hr ==  D3DERR_DEVICELOST)
				return;

			if(hr ==  D3DERR_DEVICENOTRESET)
			{
				hr = d3ddev->Reset(&d3dpp);

				if(FAILED(hr))
					return;
			}

			return;
		}

		deviceLost = false;
	}


    // clear the window to a deep blue
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

    d3ddev->BeginScene();    // begins the 3D scene
	if(NumFrames % 100 == 0)
	{
		float fps;
		QueryPerformanceCounter((LARGE_INTEGER*)&TimerEnd);
		fps = NumFrames * TimerFreq.QuadPart / (float)(TimerEnd.QuadPart - TimerStart.QuadPart);
		sprintf_s(fpsStr, "%.2f fps", fps);
	}

	// draws the frame rate every frame
	pFont->DrawTextA(NULL, fpsStr, (int)strlen(fpsStr), &fpsRect, 0, D3DCOLOR_ARGB(255, 0, 255, 0));

    // do 3D rendering on the back buffer here

    d3ddev->EndScene();    // ends the 3D scene
	NumFrames++;

    hr = d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
	if(hr ==  D3DERR_DEVICELOST)
		deviceLost = true;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    d3ddev->Release(); // close and release the 3D device
    d3d->Release(); // close and release Direct3D
}