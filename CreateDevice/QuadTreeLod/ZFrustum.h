#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#define PLANE_EPSILON 5.0f

class ZFrustum
{
public:
	ZFrustum();
	~ZFrustum();

	BOOL Make(D3DXMATRIXA16*  pmatViewProj);
	BOOL IsIN(D3DXVECTOR3* pv);
	BOOL IsInSphere(D3DXVECTOR3* pv, float radius);
	BOOL Draw(LPDIRECT3DDEVICE9 pDev);
	D3DXVECTOR3* GetPos() { return &m_vPos; }

private:
	D3DXVECTOR3 m_vtx[8];
	D3DXVECTOR3 m_vPos;
	D3DXPLANE m_plane[6];
};

void ZMakePlane(D3DXPLANE* pPlane, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2);