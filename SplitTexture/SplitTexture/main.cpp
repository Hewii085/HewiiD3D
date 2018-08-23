

#include <Windows.h>
#include <d3dx9.h>

typedef struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	FLOAT tu, tv;
}CUSTOMVERTEX, *LPCUSTOMVERTEX;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_TEX1)

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DTEXTURE9 g_tex[2] = { 0, };


HRESULT InitTexture()
{
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "C:\\Users\\Bin\\Desktop\\abab.png", &g_tex[0])))
	{
		if(FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "..\banana.bmp", &g_tex[0])))
			return E_FAIL;
	}
	
	return S_OK;
}


HRESULT InitVertex()
{
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(9 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX
		, D3DPOOL_DEFAULT, &g_pVB, NULL)))
		return E_FAIL;

	CUSTOMVERTEX vertexArry[]=
	{
		{ ::D3DXVECTOR3( 0.0f, 0.0f, 0.0f), 1.0f, 1.0f },
		{ ::D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f },
		{ ::D3DXVECTOR3( 0.0f, 1.0f, 0.0f), 1.0f, 0.0f },
		{ ::D3DXVECTOR3(-1.0f, 1.0f, 0.0f), 0.0f, 0.0f },

		{ ::D3DXVECTOR3( 1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
		{ ::D3DXVECTOR3( 0.0f, 0.0f, 0.0f), 0.0f, 1.0f },
		{ ::D3DXVECTOR3( 1.0f, 1.0f, 0.0f), 1.0f, 0.0f },
		{ ::D3DXVECTOR3( 0.0f, 1.0f, 0.0f), 0.0f, 0.0f }

		/*{ ::D3DXVECTOR3(  0.0f, 0.0f, 0.0f),0xffffffff, 0.0f, 1.0f },
		{ ::D3DXVECTOR3(  0.0f, 1.0f, 0.0f),0xffffffff, 1.0f, 0.0f },
		{ ::D3DXVECTOR3( -1.0f, 1.0f, 0.0f),0xffffffff, 0.0f, 0.0f }*/
		//{ ::D3DXVECTOR3( -1.0f, 0.0f, 0.0f),0xffffffff, 1.0f, 1.0f}
	};

	VOID* pVertices;

	if (FAILED(g_pVB->Lock(0, sizeof(CUSTOMVERTEX) * 8, (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vertexArry, sizeof(vertexArry));
	g_pVB->Unlock();

	return S_OK;
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
	//d3dpp.EnableAutoDepthStencil = TRUE;
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0F, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetTexture(0, g_tex[0]);
		/*g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
*/
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);

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

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(hWnd)))
	{

		if (SUCCEEDED(InitTexture()))
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
					else
						Render();
				}
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}


//#include <Windows.h>
//#include <d3dx9.h>
//#include "RenderCell.h"
//
//#define RENDERLISTSIZE 4
//#define VERTEXCOUNT 9
//
//D3DXVECTOR3 vertecArry[VERTEXCOUNT]
//= {
//	::D3DXVECTOR3(-1.0f,  1.0f, 0.0f),
//	::D3DXVECTOR3( 0.0f,  1.0f, 0.0f),
//	::D3DXVECTOR3( 1.0f,  1.0f, 0.0f),
//	::D3DXVECTOR3(-1.0f,  0.0f, 0.0f),
//	::D3DXVECTOR3( 0.0f,  0.0f, 0.0f),
//	::D3DXVECTOR3( 1.0f,  0.0f, 0.0f),
//	::D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
//	::D3DXVECTOR3( 0.0f, -1.0f, 0.0f),
//	::D3DXVECTOR3( 1.0f, -1.0f, 0.0f),
//  };
//
//RenderCell renderArry[4];
//
//VOID Render()
//{
//	for (int i = 0; i < RENDERLISTSIZE; i++)
//	{
//		renderArry[i].Render();
//	}
//}
//
//
//LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		return 0;
//	default:
//		break;
//	}
//
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//
//}
//
//INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
//{
//	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL,
//		NULL,NULL,NULL, "D3D Tutorial",NULL };
//
//	RegisterClassEx(&wc);
//
//	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
//		GetDesktopWindow(), NULL, wc.hInstance, NULL);
//
//
//	ShowWindow(hWnd, SW_SHOWDEFAULT);
//	UpdateWindow(hWnd);
//
//	MSG msg;
//	ZeroMemory(&msg, sizeof(msg));
//
//	while (msg.message != WM_QUIT)
//	{
//		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	UnregisterClass("D3D Tutorial", wc.hInstance);
//	return 0;
//}
