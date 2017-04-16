#pragma once

#include <d3dx9.h>
#include <d3d9.h>
#include <stdio.h>

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DCOLOR color;
	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE|D3DFVF_TEX1)

class RenderCell
{
public:
	RenderCell();
	~RenderCell();

	HRESULT SetPosition(float x, float y, float z);
	void Render();
	inline void SetD3dDevice(LPDIRECT3DDEVICE9 d3dDev) { m_pd3dDevice = d3dDev; }
private:
	CUSTOMVERTEX m_vertexLst[4];
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;//정점 4개 필요함.
};