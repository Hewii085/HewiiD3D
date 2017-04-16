#include "RenderCell.h"

RenderCell::RenderCell()
{

}

RenderCell::~RenderCell()
{

}
HRESULT RenderCell::SetPosition(float x, float y, float z)
{
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX
		, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		return E_FAIL;

	CUSTOMVERTEX vetx;
	vetx.pos.x = x;
	vetx.pos.y = y;
	vetx.pos.z = z;
	vetx.color = 0xffffffff;
	VOID* pVertices;

	if (FAILED(m_pVB->Lock(0, sizeof(CUSTOMVERTEX), (void**)&pVertices, 0)))
		return E_FAIL;
	
	m_pVB->Unlock();

	return S_OK;
}

void RenderCell::Render()
{

}