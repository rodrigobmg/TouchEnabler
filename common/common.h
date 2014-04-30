#ifndef _COMMON_H
#define _COMMON_H

#include <string>
#include <list>
#include <vector>
using namespace std;

#include "InteractionContext.h"

#include "../hook_d3d9/hook.h"

//// Angelscript headers
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

//// Vjoy headers
//#include <public.h>
//#include <vjoyinterface.h>

//// Logging system
#include "log.h"


class CommonAPI
{
public:
	static CommonAPI* getInstance()
	{
		if(instance == 0)
			instance = new CommonAPI();

		return instance;
	}
	void OnRender();
	void OnShutdown();
	void MouseMove(int x, int y);
	void MouseClick();
	//void PressKey(int vkey);
	//void ReleaseKey(int vkey);

	int AddSprite(const std::string& fileName, int width, int height);
	void DrawSprite(unsigned int id, float posX, float posY, float r, float g, float b, float a);

	void SetInfoText(const std::string& text);

	void Init(HookWrapper* h);
	bool InitAS();
	bool InitVjoy();
	bool InitInput();
	bool InitTouch();
	void RegisterAPI();
	void CallScriptFunction(const char* func);
	void ProcessPointerFrames(WPARAM wParam, POINTER_INFO pointerInfo, int index);
	void OnInteractionOutputCallback(const INTERACTION_CONTEXT_OUTPUT* output);
	void OnManipulation(const MANIPULATION_TRANSFORM* delta);
	int IsPointerInteraction(UINT32 pointerId);

	asIScriptEngine* engine;
	asIScriptContext* ctx;
	asIScriptModule* module;

	INPUT inputMouse;
	
	string exePath;
	vector<HINTERACTIONCONTEXT> interactionContexts;
	vector<list<UINT32>> pointers;
	//UINT32 frameId;

	LONG_PTR oldProc; // Store original hwnd proc
	HookWrapper* hook;



private:
	CommonAPI() {};
	CommonAPI(CommonAPI const&);
	void operator=(CommonAPI const&);

	static CommonAPI* instance;
};


#endif