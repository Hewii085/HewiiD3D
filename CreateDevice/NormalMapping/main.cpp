#include<d3d9.h>
#include<d3dx9.h>

#define WINDOW_W 500
#define WINDOW_H 500

HWND g_hWnd;
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DTEXTURE9	g_pTexDiffuse = NULL;
LPDIRECT3DTEXTURE9	g_pTexNormal = NULL;

D3DXMATRIXA16 g_matAni;
D3DXVECTOR3 g_vLight;

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT u, v;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_TEX1 |D3DFVF_TEXCOORDSIZE2(0))

HRESULT InitD3D(HWND hWnd)
{
	//d3d initialize 
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

	return S_OK;
}

HRESULT InitVB()
{

	//버텍스 초기화
	CUSTOMVERTEX vertices[]=
	{
		{ -1, 1, 0, 0xffffffff, 0 , 0},
		{  1, 1, 0, 0xffffffff, 1 , 0},
		{ -1,-1, 0, 0xffffffff, 0 , 1},
		{  1,-1, 0, 0xffffffff, 1 , 1}
	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;

	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();

	return S_OK;
}

HRESULT InitTexture()
{
	//텍스쳐를 파일로부터 로드
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "env2.bmp", &g_pTexDiffuse)))
		return E_FAIL;

	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "normal.bmp", &g_pTexNormal)))
		return E_FAIL;

	return S_OK;
}

HRESULT InitGeometry()
{
	if (FAILED(InitVB()))
		return E_FAIL;

	if (FAILED(InitTexture()))
		return E_FAIL;

	g_vLight = D3DXVECTOR3(0.0F, 0.0F, 1.0F);
	//광선 벡터 설정.
	return S_OK;
}

void SetupCamera()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -4.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

VOID Animate()
{
	//Animate 하면서 마우스 위치를 읽어서 광선의 위치를 바꿔준다
	D3DXMatrixIdentity(&g_matAni);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	g_vLight.x = -(((2.0f * pt.x) / WINDOW_W) - 1);
	g_vLight.y = -(((2.0f * pt.y) / WINDOW_H) - 1);
	g_vLight.z = 0.0f;

	if (D3DXVec3Length(&g_vLight) > 1.0f)
		D3DXVec3Normalize(&g_vLight, &g_vLight);
	else
		g_vLight.z = sqrtf(1.0f - g_vLight.x * g_vLight.x - g_vLight.y * g_vLight.y);

}

VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

void DrawMesh(D3DXMATRIXA16* pMat)
{
	g_pd3dDevice->SetTransform(D3DTS_WORLD, pMat);
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

DWORD VectortoRGBA(D3DXVECTOR3 * v, FLOAT fHeight)
{
	DWORD r = (DWORD)(127.0f * v->x + 128.0f);
	DWORD g = (DWORD)(127.0f * v->y + 128.0f);
	DWORD b = (DWORD)(127.0f * v->z + 128.0f);
	DWORD a = (DWORD)(255.0f * fHeight);

	return ((a << 24L) + (r << 16L) + (g << 8L) + (b << 0L));
}

VOID Render()
{
	D3DXMATRIXA16 matWorld;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0F, 0);

	Animate();

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetTexture(0, g_pTexNormal);
		g_pd3dDevice->SetTexture(1, g_pTexDiffuse);

		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

		DWORD dwFactor = VectortoRGBA(&g_vLight, 0.0f);
		g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwFactor);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		DrawMesh(&g_matAni);

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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC , MsgProc, 0L, 0L,
					 GetModuleHandle(NULL),NULL,NULL,NULL,NULL,
					 "BasicFrame",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("BasicFrame", "Dot3 Normal Mapping",
		WS_EX_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	g_hWnd = hWnd;
	
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		if (SUCCEEDED(InitGeometry()))
		{
			SetupCamera();

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
}
