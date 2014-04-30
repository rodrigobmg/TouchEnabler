#ifndef _HOOKD3D9_H
#define _HOOKD3D9_H

#include "hook.h"
#include <time.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <vector>

class HookD3D9 : public HookWrapper
{
public:
	HookD3D9(LPDIRECT3DDEVICE9 dev);
	virtual int AddTexture(const std::string& fileName, int width, int height);
	virtual void DrawSprite(unsigned int texId, long left, long top, long right, long bottom, float posX, 
		float posY, int r = 255, int g = 255, int b = 255, int a = 255);

	void Render();

private:
	std::vector<LPD3DXSPRITE> sprites;
	std::vector<LPDIRECT3DTEXTURE9> textures;
	LPDIRECT3DDEVICE9 device;
	ID3DXFont* font;
};


#endif