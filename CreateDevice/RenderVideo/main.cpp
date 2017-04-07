#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

#define SHOW_HOW_TO_USE_TCI

struct CUSTOMVERTEX
{
	/*FLOAT x, y, z, rhw;*/
	D3DXVECTOR3 position;
	D3DCOLOR color;

	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE|D3DFVF_TEX1)


LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DTEXTURE9 g_pTexture = NULL;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT InitD3D(HWND hwnd);
HRESULT InitVB();
VOID Render();
VOID Cleanup();
VOID SetupMatrices();
HRESULT InitGeometry();


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL,
		NULL,NULL,NULL, "D3D Tutorial",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(hWnd)))
	{

		if (SUCCEEDED(InitGeometry()))
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));

			while (msg.message != WM_QUIT)
			{
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

	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}


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

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}
int a = 0;
VOID Render()
{
	HMODULE
	/*g_pTexture->Release();
	if (a % 120 == 0)
	{
		D3DXCreateTexture(g_pd3dDevice, 500, 500, 0, D3DUSAGE_RENDERTARGET, D3DFORMAT::D3DFMT_R8G8B8, D3DPOOL::D3DPOOL_DEFAULT, &g_pTexture);
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "lake.bmp", &g_pTexture)))
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "..\\lake.bmp", &g_pTexture)))
				return;
		}
	}
	else if (a % 250 == 0)
	{
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "1325070408_1_(7).jpg", &g_pTexture)))
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "..\\1325070408_1_(7).jpg", &g_pTexture)))
				return;
		}
	}
	else if (a % 450 == 0)
	{
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "banana.bmp", &g_pTexture)))
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "..\\banana.bmp", &g_pTexture)))
				return;
		}
	}
	a++;*/

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0F, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//SetupMatrices();
		g_pd3dDevice->SetTexture(0, g_pTexture);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
		//case WM_PAINT:
		//	Render();
		//	ValidateRect(hWnd, NULL);
		//	return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}

HRESULT InitGeometry()
{
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "banana.bmp", &g_pTexture)))
	{
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "..\\banana.bmp", &g_pTexture)))
			return E_FAIL;
	}

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX
		, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	CUSTOMVERTEX* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(CUSTOMVERTEX) * 4, (void**)&pVertices, 0)))
		return E_FAIL;

	pVertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	pVertices[0].color = 0xffffffff;
	pVertices[0].tu = 1.0f;
	pVertices[0].tv = 1.0f;

	pVertices[1].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	pVertices[1].color = 0xffffffff;
	pVertices[1].tu = 0.0f;
	pVertices[1].tv = 1.0f;

	pVertices[2].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	pVertices[2].color = 0xffffffff;
	pVertices[2].tu = 1.0f;
	pVertices[2].tv = 0.0f;

	pVertices[3].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	pVertices[3].color = 0xffffffff;
	pVertices[3].tu = 0.0f;
	pVertices[3].tv = 0.0f;

	g_pVB->Unlock();

	return S_OK;
}

VOID SetupMatrices()
{

	D3DXMATRIXA16 matWorld;

	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime *(2.0f*D3DX_PI) / 1000.0f;

	D3DXMatrixRotationY(&matWorld, fAngle);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}