#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include<stdlib.h>
#include<stdio.h>

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

bool LoadTextureFromBMP(LPCSTR str);
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
	//g_pTexture->Release();
	if (a % 120 == 0)
	{
		LoadTextureFromBMP("lake.bmp");
	}
	else if (a % 450 == 0)
	{
		LoadTextureFromBMP("banana.bmp");
	}
	
	a++;

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

bool LoadTextureFromBMP(LPCSTR str)
{
	LPDIRECT3DSURFACE9	surface;
	BITMAPFILEHEADER	bFile;
	BITMAPINFOHEADER	bInfo;
	D3DLOCKED_RECT	rect;
	DWORD	w, h;
	INT		x, y;
	LPDWORD	data;
	FILE	*fp = NULL;

	fp = fopen(str, "rb");

	//BMP 파일 헤더와 헤더 인포를 읽는다.
	fread(&bFile, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bInfo, sizeof(BITMAPINFOHEADER), 1, fp);

	//24bit 픽셀만 취급한다.
	if (bInfo.biBitCount != 24)
		goto error;

	//텍스쳐 사이즈를 2의 승수에 맞춘다.
	w = bInfo.biWidth;
	h = bInfo.biHeight;

	//텍스쳐를 만든다.
	if (FAILED(g_pd3dDevice->CreateTexture(w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &g_pTexture, NULL)))
		goto error;

	//Surface를 얻어온다.
	if (FAILED(g_pTexture->GetSurfaceLevel(0, &surface)))
		goto error;

	surface->LockRect(&rect, NULL, 0); //D3DLOCK_DISCARD);

									   //BMP 데이타의 시작위치로 이동한다.
	fseek(fp, bFile.bfOffBits, SEEK_SET);

	//BMP 데이타를 카피한다(비트맵은 위아래 반대로 저장되기 때문에 아래에서부터 저장한다)
	for (y = (int)bInfo.biHeight - 1; y >= 0; y--)
	{
		//Y값의 시작 위치로 이동한다.
		data = (LPDWORD)((LPBYTE)rect.pBits + rect.Pitch * y);

		for (x = 0; x<bInfo.biWidth; x++, data++)
		{
			BYTE	r, g, b;
			fread(&b, sizeof(BYTE), 1, fp);
			fread(&g, sizeof(BYTE), 1, fp);
			fread(&r, sizeof(BYTE), 1, fp);

			//불투명 칼라값을 surface에 넣는다.
			*data = 0xff000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | ((DWORD)b);
		}
	}

	//surface의 락을 푼다.
	surface->UnlockRect();

	//surface 메모리를 해제한다.
	//_RELEASE_<LPDIRECT3DSURFACE9>(surface);
	surface->Release();

	//열린 파일 핸들을 닫는다.
	fclose(fp);

	return true;

error:
	if (g_pTexture)
		g_pTexture->Release();

	if (fp)
		fclose(fp);
	return false;
}

HRESULT InitGeometry()
{
	bool isSuccess = LoadTextureFromBMP("banana.bmp");

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

//
//FILE* f = fopen("banana.bmp", "r");
//
//if (f == NULL)
//{
//	int a = 0;
//	a++;
//}
////fseek(f, 0, SEEK_END);
//int len = ftell(f);
////fseek(f, SEEK_SET, SEEK_SET);
//LPVOID data = malloc(len);
//fread(data, sizeof(BITMAPINFOHEADER), sizeof(BITMAPINFOHEADER), f);
////memcpy(&bmp, f, sizeof(BITMAPFILEHEADER));
//if (FAILED(D3DXCreateTextureFromFileInMemory(g_pd3dDevice, data, (UINT)len, &g_pTexture)))
//return;
