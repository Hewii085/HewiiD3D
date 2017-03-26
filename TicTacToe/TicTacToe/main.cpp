#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"QuadTree"

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void InitRender(HWND hwnd)
{

}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"BasicFrame", NULL };
	
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("BasicFrame", WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

//	g_hwnd = hWnd;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		return 0;
	case WM_KEYDOWN:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
