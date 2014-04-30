#include "common.h"
#include <windows.h>
#include <commctrl.h>

CommonAPI* CommonAPI::instance = 0;

//Register callback function to interpret Interaction Context output for each DrawingObject
void CALLBACK InteractionOutputCallback(void* clientData, const INTERACTION_CONTEXT_OUTPUT* output)
{
    CommonAPI::getInstance()->OnInteractionOutputCallback(output);
}

LRESULT CALLBACK WindowProcSubclass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CommonAPI* api = CommonAPI::getInstance();
	POINTER_INFO pointerInfo = {};
	HRESULT hr;
    switch (message)
    {
	case WM_POINTERDOWN:
        // Get frame id from current message
        if (GetPointerInfo(GET_POINTERID_WPARAM(wParam), &pointerInfo))
        {
			asIScriptFunction* sf = api->module->GetFunctionByDecl("int OnPointerDown(int x, int y)");
			if(sf == 0)
				FILE_LOG(logERROR) << "The script must have the function 'OnPointerDown'.";

			asIScriptContext* ctx = api->engine->CreateContext();
			ctx->Prepare(sf);
			ctx->SetArgDWord(0, pointerInfo.ptPixelLocation.x);
			ctx->SetArgDWord(1, pointerInfo.ptPixelLocation.y);
			ctx->Execute();
			int meow = ctx->GetReturnQWord();

			if(meow >= 0 && sf != 0)
			{
				// Background was hit
				hr = AddPointerInteractionContext(api->interactionContexts[meow], GET_POINTERID_WPARAM(wParam));
				if (SUCCEEDED(hr))
				{
					api->pointers[meow].push_back(GET_POINTERID_WPARAM(wParam));
					api->ProcessPointerFrames(wParam, pointerInfo, meow);
				}
			}
        }
        break;

    case WM_POINTERUPDATE:
        // Get frame id from current message
        if (GetPointerInfo(GET_POINTERID_WPARAM(wParam), &pointerInfo))
        {
            // Background
            if (api->IsPointerInteraction(GET_POINTERID_WPARAM(wParam)))
			{
                api->ProcessPointerFrames(wParam, pointerInfo, 0);
			}
        }
        break;

    case WM_POINTERUP:
        // Get frame id from current message
        if (GetPointerInfo(GET_POINTERID_WPARAM(wParam), &pointerInfo))
        {
            // Background
			int t = api->IsPointerInteraction(GET_POINTERID_WPARAM(wParam));
            if (t >= 0)
            {
                api->ProcessPointerFrames(wParam, pointerInfo, t);
                hr = RemovePointerInteractionContext(api->interactionContexts[t],
                                                     GET_POINTERID_WPARAM(wParam));

				asIScriptFunction* sf = api->module->GetFunctionByDecl("void OnPointerUp(int)");
				if(sf == 0)
					FILE_LOG(logERROR) << "The script must have the function 'OnPointerUp'.";

				asIScriptContext* ctx = api->engine->CreateContext();
				ctx->Prepare(sf);
				ctx->SetArgDWord(0, t);
				ctx->Execute();

                if (SUCCEEDED(hr))
                {
					if (!api->pointers[t].empty())
						api->pointers[t].remove(GET_POINTERID_WPARAM(wParam));
                }
            }
        }

        break;
    default:
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}



void MessageCallback(const asSMessageInfo* msg, void* param)
{
	FILE_LOG(logINFO) << "ASmsg: " << msg->section << ", row: " << msg->row << ". " << msg->message;
}

void MessageLog(const std::string& msg)
{
	FILE_LOG(logINFO) << msg;
}

std::string GetExecutablePath() 
{
	std::vector<char> executablePath(MAX_PATH);

	// Try to get the executable path with a buffer of MAX_PATH characters.
	DWORD result = GetModuleFileNameA(0, &executablePath[0], static_cast<DWORD>(executablePath.size()));

	// As long the function returns the buffer size, it is indicating that the buffer
	// was too small. Keep enlarging the buffer by a factor of 2 until it fits.
	while(result == executablePath.size())
	{
		executablePath.resize(executablePath.size() * 2);
		result = GetModuleFileNameA(0, &executablePath[0], static_cast<DWORD>(executablePath.size()));
	}

	// If the function returned 0, something went wrong
	if(result == 0) 
	{
		throw std::runtime_error("GetModuleFileName() failed");
	}

	// We've got the path, construct a standard string from it
	return std::string(executablePath.begin(), executablePath.begin() + result);
}

void ShowMessage(string& msg)
{
	MessageBox(0, msg.c_str(), "Message", 0);
}

bool CommonAPI::InitVjoy()
{
	/*if (!vJoyEnabled())
		FILE_LOG(logERROR) << "vJoy driver not enabled: Failed Getting vJoy attributes.";
	else
		FILE_LOG(logINFO) << "Vendor: " << TEXT(GetvJoyManufacturerString()) << ", Product: " << TEXT(GetvJoyProductString()) << ", S/N: " << TEXT(GetvJoySerialNumberString());

	VjdStat status = GetVJDStatus(1);
	switch (status)
	{
	case VJD_STAT_OWN:
		FILE_LOG(logINFO) << "vJoy Device 1 is already owned by this feeder";
		break;
	case VJD_STAT_FREE:
		FILE_LOG(logINFO) << "vJoy Device 1 is free";
		break;
	case VJD_STAT_BUSY:
		FILE_LOG(logERROR) << "vJoy Device 1 is already owned by another feeder";
		break;
	case VJD_STAT_MISS:
		FILE_LOG(logERROR) << "vJoy Device 1 is not installed or disabled";
		break;
	default:
		FILE_LOG(logERROR) << "vJoy Device 1 general error";
		break;
	};

	
	if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!AcquireVJD(1))))
		FILE_LOG(logERROR) << "Failed to acquire vJoy device number 1";
	else
		FILE_LOG(logINFO) << "Acquired: vJoy device number 1";*/

	return true;
}

bool CommonAPI::InitInput()
{
	inputMouse.type = INPUT_MOUSE;
    inputMouse.mi.dx = (0 * (0xFFFF / GetSystemMetrics(SM_CXSCREEN)));
    inputMouse.mi.dy = (0 * (0xFFFF / GetSystemMetrics(SM_CYSCREEN)));
    inputMouse.mi.mouseData = 0;
    inputMouse.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
    inputMouse.mi.time = 0;
    inputMouse.mi.dwExtraInfo = 0;

	//inputKB.type = INPUT_KEYBOARD;
	//inputKB.ki.dwFlags = KEYEVENTF_SCANCODE; // hardware scan code for key
	//inputKB.ki.time = 0;
	//inputKB.ki.dwExtraInfo = 0;

	return true;
}

void PressKey(int vkey)
{
	FILE_LOG(logINFO) << "Key press: " << vkey;
	INPUT inputKB;
	inputKB.type = INPUT_KEYBOARD;
	inputKB.ki.dwFlags = KEYEVENTF_SCANCODE; // hardware scan code for key
	inputKB.ki.time = 0;
	inputKB.ki.dwExtraInfo = 0;
	inputKB.ki.wScan = vkey;
	inputKB.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &inputKB, sizeof(INPUT));
}

void ReleaseKey(int vkey)
{
	FILE_LOG(logINFO) << "Key release: " << vkey;
	INPUT inputKB;
	inputKB.type = INPUT_KEYBOARD;
	inputKB.ki.dwFlags = KEYEVENTF_SCANCODE; // hardware scan code for key
	inputKB.ki.time = 0;
	inputKB.ki.dwExtraInfo = 0;
	inputKB.ki.wScan = vkey;
	inputKB.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	SendInput(1, &inputKB, sizeof(INPUT));
}

void CommonAPI::MouseMove(int x, int y)
{
    inputMouse.mi.dx = (x * (0xFFFF / GetSystemMetrics(SM_CXSCREEN)));
    inputMouse.mi.dy = (y * (0xFFFF / GetSystemMetrics(SM_CYSCREEN)));
    inputMouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

    SendInput(1, &inputMouse, sizeof(INPUT));
}


void CommonAPI::MouseClick()
{
    inputMouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
    SendInput(1, &inputMouse, sizeof(INPUT));

    Sleep(10);

    inputMouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
    SendInput(1, &inputMouse, sizeof(INPUT));
}

UINT32 dpiScaleX = 0;
UINT32 dpiScaleY = 0;

bool isInRectangle(float centerX, float centerY, float radius, float x, float y)
{
        return x >= centerX - radius && x <= centerX + radius && 
            y >= centerY - radius && y <= centerY + radius;
}    

//test if coordinate (x, y) is within a radius from coordinate (center_x, center_y)
bool isPointInCircle(float centerX, float centerY, float radius, float x, float y)
{
    if(isInRectangle(centerX, centerY, radius, x, y))
    {
        float dx = centerX - x;
        float dy = centerY - y;
        dx *= dx;
        dy *= dy;
        float distanceSquared = dx + dy;
        float radiusSquared = radius * radius;
        return distanceSquared <= radiusSquared;
    }
    return false;
}

void CommonAPI::SetInfoText(const std::string& text)
{
	hook->infoText = text;
}

void CommonAPI::RegisterAPI()
{
	engine->RegisterGlobalFunction("void SetInfoText(const string &in)", asMETHOD(CommonAPI, SetInfoText), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void MouseMove(int x, int y)", asMETHOD(CommonAPI, MouseMove), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void MouseClick()", asMETHOD(CommonAPI, MouseClick), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void AddTouchHandler()", asMETHOD(CommonAPI, InitTouch), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void PressKey(int)", asFUNCTION(PressKey), asCALL_CDECL);
	engine->RegisterGlobalFunction("void ReleaseKey(int)", asFUNCTION(ReleaseKey), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool PointInCircle(float,float,float,float,float)", asFUNCTION(isPointInCircle), asCALL_CDECL);
	engine->RegisterGlobalFunction("int AddSprite(const string &in, int, int)", asMETHOD(CommonAPI, AddSprite), asCALL_THISCALL_ASGLOBAL, this);
	engine->RegisterGlobalFunction("void DrawSprite(uint, float, float, float r, float g, float b, float a)",
		asMETHOD(CommonAPI, DrawSprite), asCALL_THISCALL_ASGLOBAL, this);
	//engine->RegisterGlobalFunction("bool SetAxis(int64 x, uint y, uint z)", asFUNCTION(SetAxis), asCALL_CDECL);
	engine->RegisterGlobalFunction("void ShowMessage(const string &in)", asFUNCTION(ShowMessage), asCALL_CDECL);
	engine->RegisterGlobalFunction("void echo(const string &in)", asFUNCTION(MessageLog), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool EnableMouseInPointer(bool)", asFUNCTION(EnableMouseInPointer), asCALL_CDECL);
	engine->RegisterGlobalFunction("void exit(int)", asFUNCTION(exit), asCALL_CDECL);

	HDC hdc = GetDC(NULL);
	if (hdc)
	{
		dpiScaleX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpiScaleY = GetDeviceCaps(hdc, LOGPIXELSY);
	}
	else
	{
		// ERROR
	}
	ReleaseDC(NULL, hdc);

	engine->RegisterGlobalProperty("const uint dpiScaleX", &dpiScaleX);
	engine->RegisterGlobalProperty("const uint dpiScaleY", &dpiScaleY);
}

int CommonAPI::AddSprite(const std::string& fileName, int width, int height)
{
	//InitTouch();
	return hook->AddTexture(fileName, width, height);
}

void CommonAPI::DrawSprite(unsigned int id, float posX, float posY, float r, float g, float b, float a)
{
	hook->DrawSprite(id, 0, 0, 0, 0, posX, posY, r, g, b, a);
}

bool CommonAPI::InitAS()
{
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	RegisterStdString(engine);

	RegisterAPI();

	CScriptBuilder builder;

	if(builder.StartNewModule(engine, "MyModule") < 0)
		FILE_LOG(logERROR) << "Unrecoverable error while starting a new module.";

	string scriptPath = exePath.substr(0, exePath.find_last_of(".")).append(".tes");

	if(builder.AddSectionFromFile(scriptPath.c_str()) < 0)
		FILE_LOG(logERROR) << "Unable to load script." << scriptPath;
	else
		FILE_LOG(logINFO) << "Script " << scriptPath << " loaded successfully.";

	if(builder.BuildModule() < 0)
		FILE_LOG(logERROR) << "Please correct the errors in the script and try again.";

	
	module = engine->GetModule("MyModule");

	return true;
}

int CommonAPI::IsPointerInteraction(UINT32 pointerId)
{
	for(int i = 0; i < pointers.size(); ++i)
	{
		for(std::list<UINT32>::iterator it = pointers[i].begin(); it != pointers[i].end(); it++)
		{
			if(*it == pointerId)
				return i;
		}
	}
    return -1;
}

void CommonAPI::ProcessPointerFrames(WPARAM wParam, POINTER_INFO pointerInfo, int index)
{
    if (interactionContexts[index]) // New frame
    {
        // Find out pointer count and frame history length
        UINT uEntriesCount = 0;
        UINT uPointerCount = 0;

        if (GetPointerFrameInfoHistory(GET_POINTERID_WPARAM(wParam), &uEntriesCount, &uPointerCount, NULL))
        {
            // Allocate space for pointer frame history
            POINTER_INFO *pPointerInfo = NULL;
            try
            {
                pPointerInfo = new POINTER_INFO [uEntriesCount * uPointerCount];
            }
            catch (...)
            {
                pPointerInfo = NULL;
            }

            if (pPointerInfo != NULL)
            {
                // Retrieve pointer frame history
                if (GetPointerFrameInfoHistory(GET_POINTERID_WPARAM(wParam), &uEntriesCount, &uPointerCount, pPointerInfo))
                {
                    // Process frame history
                    ProcessPointerFramesInteractionContext(interactionContexts[index], uEntriesCount, uPointerCount, pPointerInfo);
                }
                delete [] pPointerInfo;
                pPointerInfo = NULL;
            }
        }
    }
}



bool CommonAPI::InitTouch()
{
	// Initialize Interaction Context
	HINTERACTIONCONTEXT hc;
	HRESULT hr = CreateInteractionContext(&hc);
	if (SUCCEEDED(hr))
	{
		hr = SetPropertyInteractionContext(hc, INTERACTION_CONTEXT_PROPERTY_FILTER_POINTERS, TRUE);
	}


	if (SUCCEEDED(hr))
    {
        INTERACTION_CONTEXT_CONFIGURATION cfg[] = 
        {
            {INTERACTION_ID_MANIPULATION,
                INTERACTION_CONFIGURATION_FLAG_MANIPULATION |
                INTERACTION_CONFIGURATION_FLAG_MANIPULATION_TRANSLATION_X |
                INTERACTION_CONFIGURATION_FLAG_MANIPULATION_TRANSLATION_Y |
                INTERACTION_CONFIGURATION_FLAG_MANIPULATION_SCALING |
                INTERACTION_CONFIGURATION_FLAG_MANIPULATION_ROTATION},
            {INTERACTION_ID_TAP, INTERACTION_CONFIGURATION_FLAG_TAP},
            {INTERACTION_ID_SECONDARY_TAP, INTERACTION_CONFIGURATION_FLAG_SECONDARY_TAP},
            {INTERACTION_ID_HOLD, INTERACTION_CONFIGURATION_FLAG_HOLD}
        };
        hr = SetInteractionConfigurationInteractionContext(hc, sizeof(cfg)/sizeof(cfg[0]), cfg);
    }

	if (SUCCEEDED(hr))
    {
		hr = RegisterOutputCallbackInteractionContext(hc,  &InteractionOutputCallback, hc);
	}
	
	/*UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;*/
	list<UINT32> pointerIds;
	this->pointers.push_back(pointerIds);
	this->interactionContexts.push_back(hc);

	return true;
}


// Interaction Context Gesture Recognizer
void CommonAPI::OnInteractionOutputCallback(const INTERACTION_CONTEXT_OUTPUT* output)
{
    switch(output->interactionId)
    {
    case INTERACTION_ID_MANIPULATION:
        OnManipulation(&output->arguments.manipulation.cumulative);
        break;

    case INTERACTION_ID_TAP:
        if (output->arguments.tap.count == 1)
        {
			asIScriptFunction* sf = module->GetFunctionByDecl("void OnTap()");
			if(sf == 0)
			{
				FILE_LOG(logERROR) << "The script must have the function 'OnTap'.";
				return;
			}

			asIScriptContext* ctx = engine->CreateContext();
			ctx->Prepare(sf);
			ctx->Execute();
        }
        break;

    case INTERACTION_ID_SECONDARY_TAP:
		{
			asIScriptFunction* sf = module->GetFunctionByDecl("void OnSecondaryTap()");
			if(sf == 0)
			{
				FILE_LOG(logERROR) << "The script must have the function 'OnSecondaryTap'.";
				return;
			}

			asIScriptContext* ctx = engine->CreateContext();
			ctx->Prepare(sf);
			ctx->Execute();
		}
        break;

    default:
        break;
    }
}

#define PI 3.14159265

void CommonAPI::OnManipulation(const MANIPULATION_TRANSFORM* delta)
{
	asIScriptFunction* sf = module->GetFunctionByDecl("void OnManipulation(float, float, float, float)");
	if(sf == 0)
	{
		FILE_LOG(logERROR) << "The script must have the function 'OnManipulation'.";
		return;
	}

	asIScriptContext* ctx = engine->CreateContext();
	ctx->Prepare(sf);
	ctx->SetArgFloat(0, delta->translationX);
	ctx->SetArgFloat(1, delta->translationY);
	ctx->SetArgFloat(2, 180 * (delta->rotation)/PI);
	ctx->SetArgFloat(3, delta->scale);
	//ctx->SetArgFloat(0, delta->expansion);
	ctx->Execute();
    //D2D1_POINT_2F position = Point2F(m_CenterPt.x, m_CenterPt.y);
    //Matrix3x2F mxDelta;
    //float angle = 180 * (delta->rotation)/PI;  // convert radian to degrees

    ////Transform translation to the rotation and scale of background
    //float backScale = g_background.GetBackgroundScale();
    //float backRotation = g_background.GetBackgroundRotation();
    //D2D1_POINT_2F translate = {UnscaleX(delta->translationX), UnscaleY(delta->translationY)};
    //translate =  (Matrix3x2F::Rotation(backRotation)*
    //              Matrix3x2F::Scale(backScale, backScale, Point2F()))
    //              .TransformPoint(translate);
    //
    ////Translate the center point of object
    //m_CenterPt = Matrix3x2F::Translation(translate.x, translate.y).TransformPoint(m_CenterPt);

    //mxDelta = Matrix3x2F::Translation(translate.x, translate.y) *
    //          Matrix3x2F::Scale(delta->scale, delta->scale, position) *
    //          Matrix3x2F::Rotation(angle, m_CenterPt);
    //
    //m_mxManipulation = m_mxManipulation*mxDelta;
}


void CommonAPI::Init(HookWrapper* h)
{
	hook = h;
	exePath = GetExecutablePath();

	// Init logging
	FILE* logFile = fopen("touchenabler.log", "w");
	Output2FILE::Stream() = logFile;

	// Init AS
	InitAS();

	// Init Vjoy
	//InitVjoy();

	// Init Input
	InitInput();

	//InitTouch();
	
	if (!SetWindowSubclass(hook->windowHandle,  &WindowProcSubclass, 0, 0))
    {
		FILE_LOG(logERROR) << "Unable to subclass HWND.";
    }


	CallScriptFunction("void OnLoad()");

}

void CommonAPI::OnRender()
{
	CallScriptFunction("void OnRender()");
}

void CommonAPI::OnShutdown()
{
	CallScriptFunction("void OnShutdown()");
}

void CommonAPI::CallScriptFunction(const char* func)
{
	asIScriptFunction* sf = module->GetFunctionByDecl(func);
	if(sf == 0)
	{
		FILE_LOG(logERROR) << "The script must have the function '" << func << "'.";
		return;
	}

	ctx = engine->CreateContext();
	ctx->Prepare(sf);
	ctx->Execute();
}

