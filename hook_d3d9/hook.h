#ifndef _HOOK_H
#define _HOOK_H

#include <Windows.h>
#include <string>

using namespace std;

class HookWrapper
{
public:
	virtual int AddTexture(const std::string& fileName, int width, int height) = 0;
	virtual void DrawSprite(unsigned int texId, long left, long top, long right, long bottom, float posX,
		float posY, int r = 255, int g = 255, int b = 255, int a = 255) = 0;

	HWND windowHandle;
	string infoText;
};


#endif