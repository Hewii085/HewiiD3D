#include "Texture.h"


CTexture::CTexture()
{}


CTexture::~CTexture()
{
	if (m_pVB != NULL)
		m_pVB->Release();

	for (int i = 0; i<4; ++i)
	{
		if (m_pTexture[i] != NULL)
			m_pTexture[i]->Release();
	}
}


int CTexture::InitVB(LPDIRECT3DDEVICE9  g_pd3dDevice)
{

	m_pd3dDevice = g_pd3dDevice;

	for (int i = 0; i<4; ++i)
	{
		sprintf_s(str, "C:\\Users\\Bin\\Desktop\\abab.png", i);
		D3DXCreateTextureFromFile(m_pd3dDevice, str, &m_pTexture[i]);
	}

	m_Vertices[0].x = 0.0f;
	m_Vertices[0].y = 300.0f;
	m_Vertices[0].tu = 0.0f;
	m_Vertices[0].tv = 1.0f;


	m_Vertices[1].x = 0.0f;
	m_Vertices[1].y = 0.0f;
	m_Vertices[1].tu = 0.0f;
	m_Vertices[1].tv = 0.0f;


	m_Vertices[2].x = 400.0f;
	m_Vertices[2].y = 300.0f;
	m_Vertices[2].tu = 1.0f;
	m_Vertices[2].tv = 1.0f;


	m_Vertices[3].x = 400.0f;
	m_Vertices[3].y = 0.0f;
	m_Vertices[3].tu = 1.0f;
	m_Vertices[3].tv = 0.0f;

	for (int i = 0; i<4; ++i)
	{
		m_Vertices[i].z = 0.5f;
		m_Vertices[i].rhw = 0.5f;
		m_Vertices[i].color = 0xffffffff;
	}


	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}


	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(m_Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, m_Vertices, sizeof(m_Vertices));
	m_pVB->Unlock();

	return S_OK;
}
void CTexture::Render()
{
	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
void CTexture::SetPosition(D3DXVECTOR2 Pos)
{
	m_Vertices[0].x = Pos.x - 200.0f;
	m_Vertices[0].y = Pos.y + 150.0f;
	m_Vertices[0].tu = 0.0f;
	m_Vertices[0].tv = 1.0f;


	m_Vertices[1].x = Pos.x - 200.0f;
	m_Vertices[1].y = Pos.y - 150.0f;
	m_Vertices[1].tu = 0.0f;
	m_Vertices[1].tv = 0.0f;


	m_Vertices[2].x = Pos.x + 200.0f;
	m_Vertices[2].y = Pos.y + 150.0f;
	m_Vertices[2].tu = 1.0f;
	m_Vertices[2].tv = 1.0f;


	m_Vertices[3].x = Pos.x + 200.0f;
	m_Vertices[3].y = Pos.y - 150.0f;
	m_Vertices[3].tu = 1.0f;
	m_Vertices[3].tv = 0.0f;

	for (int i = 0; i<4; ++i)
	{
		m_Vertices[i].z = 0.5f;
		m_Vertices[i].rhw = 0.5f;
		m_Vertices[i].color = 0xffffffff;
	}

	VOID* pVertices;
	m_pVB->Lock(0, sizeof(m_Vertices), (void**)&pVertices, 0);
	memcpy(pVertices, m_Vertices, sizeof(m_Vertices));
	m_pVB->Unlock();

}

void CTexture::SetTexture(int Number)
{
	m_pd3dDevice->SetTexture(0, m_pTexture[Number]);
}
