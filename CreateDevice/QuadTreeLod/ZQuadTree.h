#pragma once

#include "define.h"
#include "ZFrustum.h"
#include "ZFLog.h"

class ZQuadTree
{
	enum CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	enum QuadLocation { FRUSTUM_OUT = 0, FRUSTUM_PARTIALLY_IN =1, FRUSTUM_COMPLETELY_IN =2, FRUSTUME_UNKNOWN = -1	};

public:
	ZQuadTree(int x, int cy);
	ZQuadTree(ZQuadTree* pParent);
	~ZQuadTree();

	BOOL Build(TERRAINVERTEX* pHEightMap);
	int GenerateIndex(LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio);

private:

	ZQuadTree* _AddChild(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);
	BOOL _SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);
	BOOL _SubDivide();
	int _GenTriIndex(int nTris, LPVOID pIndex, TERRAINVERTEX* pHEightMap, ZFrustum* pFrustum, float fLODRatio);
	void _Destroy();

	float _GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2) { return D3DXVec3Length(&(*pv2 - *pv1)); }

	int _GETLODLEVEL(TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		float d = _GetDistance((D3DXVECTOR3*)(pHeightMap + m_nCenter), pCamera);
		return max((int)(d*fLODRatio), 1);
	}

	BOOL _IsVisible(TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		return (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= _GETLODLEVEL(pHeightMap, pCamera, fLODRatio));
	}

	int _IsInFrustum(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);
	void _FrustumCull(TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum);


	ZQuadTree* m_pChild[4];
	int m_nCenter;
	int m_nCorner[4];
	BOOL m_bCulled;
	float m_fRadius;
};

