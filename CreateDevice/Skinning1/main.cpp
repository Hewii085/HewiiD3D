#include <d3d9.h>
#include<d3dx9.h>

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9 g_pIB = NULL;
LPDIRECT3DTEXTURE9 g_pTexture = NULL;

D3DXMATRIXA16 g_mat0;
D3DXMATRIXA16 g_mat1;

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	FLOAT b[3];
	DWORD index;
	DWORD color;
	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct  MYINDEX
{
	WORD _0, _1, _2;
};


HRESULT InitD3D(HWND hWnd)
{

	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS);

	return S_OK;
}

HRESULT InitVB()
{
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	CUSTOMVERTEX * pVertices;

	if (FAILED(g_pVB->Lock(0, 0, (void**)&pVertices, 0)))
		return E_FAIL;

	int idx = 0;
	for (DWORD i = 0; i < 50; i++)
	{
		FLOAT theta = (2 * D3DX_PI*i) / (50 - 1);
		idx = 2 * i;
		pVertices[idx].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
		pVertices[idx].b[0] = 1.0f;
		pVertices[idx].b[1] = 0.0f;
		pVertices[idx].b[2] = 0.0f;
		pVertices[idx].index = 0x0000;
		pVertices[idx].color = 0xffffffff;
		pVertices[idx].tu = ((FLOAT)i) / (50 - 1);
		pVertices[idx].tv = 1.0f;
		
		idx += 1;
		pVertices[idx].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
		pVertices[idx].b[0] = 0.5f;
		pVertices[idx].b[1] = 0.5f;
		pVertices[idx].b[2] = 0.0f;
		pVertices[idx].index = 0x0001;
		pVertices[idx].color = 0xff808080;
		pVertices[idx].tu = ((FLOAT)i) / (50 - 1);
		pVertices[idx].tv = 0.0f;

	}

	g_pVB->Unlock();
	//vertex 배치
	return S_OK;
}

HRESULT InitIB()
{
	return S_OK;
}

HRESULT InitGeometry()
{
	if (FAILED(InitVB())) return E_FAIL;
	if (FAILED(InitIB())) return E_FAIL;

	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "lake.bmp", &g_pTexture)))
		return E_FAIL;

	return S_OK;
}

void SetupCamera()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEyePt(0.0f, 2.0f, -3.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0F, 1.0F, 100.0F);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

}

VOID Animate()
{
	D3DXMatrixIdentity(&g_mat0);
	DWORD d = GetTickCount() % ((int)((D3DX_PI * 2) * 1000));
	D3DXMatrixRotationY(&g_mat1, d / 1000.0f);
}

VOID Cleanup()
{
	if (g_pTexture != NULL)
		g_pTexture->Release();

	if (g_pIB != NULL)
		g_pIB->Release();
	
	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

void DrawMesh(void)
{
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);
}

VOID Render()
{
	D3DXMATRIXA16 matWorld;

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0F, 0);
	Animate();

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), &g_mat0);
		g_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(1), &g_mat1);
		g_pd3dDevice->SetTexture(0, g_pTexture);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		DrawMesh();

		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	/// 윈도우 클래스 등록
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"BasicFrame", NULL };
	RegisterClassEx(&wc);

	/// 윈도우 생성
	HWND hWnd = CreateWindow("BasicFrame", "Skinning",
		WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	/// Direct3D 초기화
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		if (SUCCEEDED(InitGeometry()))
		{
			/// 카메라 행렬설정
			SetupCamera();

			/// 윈도우 출력
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			/// 메시지 루프
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				/// 메시지큐에 메시지가 있으면 메시지 처리
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
					Render();
			}
		}
	}

	/// 등록된 클래스 소거
	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}




