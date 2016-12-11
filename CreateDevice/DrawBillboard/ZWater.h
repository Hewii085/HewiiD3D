#pragma once

#include "define.h"

class ZWater
{
	LPDIRECT3DDEVICE9 m_pDev;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	LPDIRECT3DTEXTURE9 m_pTexWater[2];
	int m_cx;
	int m_cz;
	int m_nWaveTick;
	int m_nPrevTick;
	float m_fWave;
	float m_fDir;

	struct WATERVERTEX
	{
		enum {FVF = D3DFVF_XYZ| D3DFVF_DIFFUSE | D3DFVF_TEX1};
		float px, py, pz;
		DWORD d;
		float tx, ty;
	};

protected:
	int _LoadTextures();
	int _CreateVIB();
	int _WaveTexture();

public:
	ZWater();
	~ZWater();

	int Create(LPDIRECT3DDEVICE9 pDev, int cx, int cz, int nSpeed);
	int Draw();

};