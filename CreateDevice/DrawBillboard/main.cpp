#define MAINBODY

#include <d3d9.h>
#include <d3dx9.h>
#include "ZCamera.h"
#include "ZFLog.h"
#include "ZWater.h"

#define WINDOW_TITLE "Billboard"
#define WINDOW_W 500
#define WINDOW_H 500

HWND g_hWnd = NULL;
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DTEXTURE9 g_pTexBillboard[4] = { NULL, NULL, NULL, NULL };

D3DXMATRIXA16 g_matAni;
D3DXMATRIXA16 g_matWorld;
D3DXMATRIXA16 g_matView;
D3DXMATRIXA16 g_matProj;

DWORD g_dwMouseX = 0;
DWORD g_dwMouseY = 0;
BOOL g_bBillboard = TRUE;
BOOL g_bWireframe = FALSE;

/*
사전적 의미의 와이어프레임은 컴퓨터 그래픽에서 3차원 물체의 형상을 나타내기 위해
물체의 형상을 수많은 선의 모임으로 표시하여 입체감을 나타내는것
*/

ZCamera* g_pCamera = NULL;
ZWater* g_pWater = NULL;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT InitD3D(HWND hWnd);
HRESULT InitObjects();
HRESULT InitGeometry();
void DeleteObjects();
void DrawBillboard();
VOID Render();
VOID SetupLights();
VOID Cleanup();
VOID Animate();
void InitMatrix();
void LogFPS(void);
void LogStatus(void);
void ProcessMouse(void);
void ProcessKey(void);
void ProcessInputs(void);


LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0L);
			break;
		case '1':
			g_bWireframe = !g_bWireframe;
			break;
		case '2':
			g_bBillboard = !g_bBillboard;
			break;
		}
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc , 0L, 0L,
					 GetModuleHandle(NULL), NULL,NULL,NULL,NULL,
					 "BasicFrame",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("BasicFrame", WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	g_hWnd = hWnd;
	srand(GetTickCount());
	
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		if (SUCCEEDED(InitObjects()))
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
	}

	DeleteObjects();
	UnregisterClassA("BasicFrame", wc.hInstance);

	return 0;

}

HRESULT InitGeometry()
{
	InitMatrix();

	D3DXCreateTextureFromFile(g_pd3dDevice, "tree01s.dds", &g_pTexBillboard[0]);
	D3DXCreateTextureFromFile(g_pd3dDevice, "tree02S.dds", &g_pTexBillboard[1]);
	D3DXCreateTextureFromFile(g_pd3dDevice, "tree35S.dds", &g_pTexBillboard[2]);
	D3DXCreateTextureFromFile(g_pd3dDevice, "tex.jpg", &g_pTexBillboard[3]);

	POINT pt;
	GetCursorPos(&pt);
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;

	return S_OK;
}

VOID Cleanup()
{
	for (int i = 0; i < 4; i++)
	{
		S_REL(g_pTexBillboard[i]);
	}

	S_REL(g_pd3dDevice);
	S_REL(g_pD3D);
}

VOID SetupLights()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial(&mtrl);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
}

VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200, 200, 200)
		, 1.0f, 0);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	Animate();

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pWater->Draw();
		DrawBillboard();
		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
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

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

void ProcessMouse(void)
{
	POINT pt;
	float fDelta = 0.001f;

	GetCursorPos(&pt);
	int dx = pt.x - g_dwMouseX;
	int dy = pt.y - g_dwMouseY;

	g_pCamera->RotateLocalX(dy*fDelta);
	g_pCamera->RotateLocalY(dx*fDelta);
	D3DXMATRIXA16* pmatView = g_pCamera->GetViewMatrix();
	g_pd3dDevice->SetTransform(D3DTS_VIEW, pmatView);

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;

}

void ProcessKey(void)
{
	if (GetAsyncKeyState('A'))
		g_pCamera->MoveLocalZ(0.5f);

	if (GetAsyncKeyState('Z'))
		g_pCamera->MoveLocalZ(-0.5f);
}

void ProcessInputs(void)
{
	ProcessMouse();
	ProcessKey();
}

void LogStatus(void)
{
	g_pLog->Log("Wireframe : %d", g_bWireframe);
	g_pLog->Log("BillBoard : %d", g_bBillboard);
}

void LogFPS(void)
{
	static DWORD nTick = 0;
	static DWORD nFPS = 0;

	if (GetTickCount() - nTick > 1000)
	{
		nTick = GetTickCount();
		g_pLog->Log("FPS:%d", nFPS);

		nFPS = 0;
		LogStatus();
		return;
	}

	nFPS++;
}

void DrawBillboard()
{
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	struct MYVERTEX
	{
		enum {FVF = D3DFVF_XYZ | D3DFVF_TEX1};
		float px, py, pz;
		float tu, tv;
	};

	MYVERTEX vtx[4]=
	{
		{-1, 0, 0, 0, 1},
		{-1, 4, 0, 0, 0},
		{ 1, 0, 0, 1, 1},
		{ 1, 4, 0, 1, 0}
	};

	D3DXMATRIXA16 matBillboard;
	D3DXMatrixIdentity(&matBillboard);

	g_pd3dDevice->SetTexture(1, NULL);
	g_pd3dDevice->SetFVF(MYVERTEX::FVF);

	if (g_bBillboard)
	{
		matBillboard._11 = g_pCamera->GetViewMatrix()->_11;
		matBillboard._13 = g_pCamera->GetViewMatrix()->_13;
		matBillboard._31 = g_pCamera->GetViewMatrix()->_31;
		matBillboard._33 = g_pCamera->GetViewMatrix()->_33;
		D3DXMatrixInverse(&matBillboard, NULL, &matBillboard);
	}

	for (int z = 0; z <= 40; z += 5)
	{
		for (int x = 0; x <= 40; x += 5)
		{
			matBillboard._41 = x - 20;
			matBillboard._42 = 0;
			matBillboard._43 = z - 20;
			g_pd3dDevice->SetTexture(0, g_pTexBillboard[(x + z) % 4]);
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matBillboard);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(MYVERTEX));
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);
}

HRESULT InitObjects()
{
	g_pLog = new ZFLog(ZF_LOG_TARGET_WINDOW);
	g_pCamera = new ZCamera();
	g_pWater = new ZWater();
	g_pWater->Create(g_pd3dDevice, 64, 64, 100);

	return S_OK;
}

void InitMatrix()
{
	D3DXMatrixIdentity(&g_matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld);

	D3DXVECTOR3 vEyePt(0.0f, 5.0f, (float)-3.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 5.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&g_matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_matView);

	D3DXMatrixPerspectiveFovLH(&g_matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_matProj);

	D3DXMatrixPerspectiveFovLH(&g_matProj, D3DX_PI / 4, 1.0f, 1.0f, 200.0f);

	g_pCamera->SetView(&vEyePt, &vLookatPt, &vUpVec);
}

VOID Animate()
{
	D3DXMatrixIdentity(&g_matAni);

	SetupLights();
	ProcessInputs();

	LogFPS();
}

void DeleteObjects()
{
	S_DEL(g_pWater);
	S_DEL(g_pLog);
	S_DEL(g_pCamera);
}