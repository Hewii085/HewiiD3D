#pragma once

#pragma once

#include <d3dx9.h>
#include <d3d9.h>
#include <stdio.h>

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
	FLOAT       tu, tv;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


class CTexture
{
public:

	CTexture();
	~CTexture();

	int InitVB(LPDIRECT3DDEVICE9    g_pd3dDevice);
	void Render();
	void SetPosition(D3DXVECTOR2 Pos);
	void SetTexture(int Number);



private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DTEXTURE9      m_pTexture[4];

	char str[128];

	CUSTOMVERTEX m_Vertices[4];
};
