#pragma once
#include "define.h"
#include "ZQuadTree.h"
#define MAX_TERRAIN_TEX 4

class ZTerrain
{
public:
	ZTerrain();
	~ZTerrain();

	HRESULT Create(LPDIRECT3DDEVICE9 pDev, D3DXVECTOR3* pvfScale, float fLODRation, LPSTR lpBMPFilename, LPSTR lpTexFilename[MAX_TERRAIN_TEX]);

	TERRAINVERTEX* GetVertex(int x, int z) { return (m_pvHeightMap + x + z * m_cxDIB); }
	inline float GetHeight(int x, int z) { return (GetVertex(x, z))->p.y; }
	inline int   GetCXDIB() { return m_cxDIB; }
	inline int   GetCZDIB() { return m_czDIB; }
	inline float GetCXTerrain() { return m_cxDIB * m_vfScale.x; }
	inline float GetCYTerrain() { return m_cyDIB * m_vfScale.y; }
	inline float GetCZTerrain() { return m_czDIB * m_vfScale.z; }

	HRESULT Draw(ZFrustum* pFrustum);

private:
	int m_cxDIB;
	int m_czDIB;
	int m_cyDIB;
	D3DXVECTOR3 m_vfScale;
	TERRAINVERTEX* m_pvHeightMap;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DTEXTURE9 m_pTex[MAX_TERRAIN_TEX];
	LPDIRECT3DVERTEXBUFFER9 m_VB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	int m_nTriangles;
	float m_fLODRatio;

	HRESULT _Destroy();
	HRESULT _LoadTextures(LPSTR lpTexFilename[MAX_TERRAIN_TEX]);
	HRESULT _BuildHEightMap(LPSTR lpFilename);
	HRESULT _BuildQuadTree();
	HRESULT _CreateVIB();
	HRESULT _Render();

};
