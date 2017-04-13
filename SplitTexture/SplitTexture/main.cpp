#include <Windows.h>
#include <d3dx9.h>

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE|D3DFVF_TEX1)

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

HRESULT InitVertex()
{
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX
		, D3DPOOL_DEFAULT, &g_pVB, NULL)))
		return E_FAIL;

	CUSTOMVERTEX* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(CUSTOMVERTEX) * 4, (void**)&pVertices, 0)))
		return E_FAIL;
}

HRESULT InitD3D(HWND hWnd)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
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

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL,
		NULL,NULL,NULL, "D3D Tutorial",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(hWnd)))
	{

		if (SUCCEEDED(InitVertex()))
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
				//else
					//Render();
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}