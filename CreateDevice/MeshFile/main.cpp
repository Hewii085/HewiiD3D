#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

struct CUSTOMVERTEX
{
	/*FLOAT x, y, z, rhw;*/
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL)

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPD3DXMESH g_pMesh = NULL;
D3DMATERIAL9* g_MeshMaterials = NULL;
LPDIRECT3DTEXTURE9* g_MeshTextures = NULL;
DWORD g_dwNumMaterials = 0L;

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

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
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

	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0Xffffffff);
	return S_OK;
}

VOID Render()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0F, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		SetupMatrices();

		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			g_pd3dDevice->SetMaterial(&g_MeshMaterials[i]);
			g_pd3dDevice->SetTexture(0, g_MeshTextures[i]);

			g_pMesh->DrawSubset(i);
		}

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
	LPD3DXBUFFER pD3DXMtrlBuffer;

	if (FAILED(D3DXLoadMeshFromX("Tiger.x", D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
	{
		if (FAILED(D3DXLoadMeshFromX("Tiger.x", D3DXMESH_SYSTEMMEM,
			g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
			{


				return E_FAIL;
			}
	}

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_MeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_MeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	if (g_MeshMaterials == NULL)
		return E_OUTOFMEMORY;

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		g_MeshMaterials[i] = d3dxMaterials[i].MatD3D;
		g_MeshMaterials[i].Ambient = g_MeshMaterials[i].Diffuse;

		g_MeshTextures[i] = NULL;

		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, d3dxMaterials[i].pTextureFilename
				, &g_MeshTextures[i])))
			{
				const CHAR* strPrefix = "..\\";
				const int lenPrefix = lstrlen(strPrefix);
				CHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix);

				if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, strTexture, &g_MeshTextures[i])))
				{
					;
				}

			}
		}
	}

	pD3DXMtrlBuffer->Release();
	return S_OK;
}


VOID SetupMatrices()
{

	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);

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