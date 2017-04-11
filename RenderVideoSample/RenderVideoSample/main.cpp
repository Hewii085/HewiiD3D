extern "C"
{
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib , "avformat.lib")
#pragma comment( lib, "avutil.lib" )
#pragma comment( lib, "avcodec.lib" )
#pragma comment( lib, "swscale.lib" )

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;

	FLOAT tu, tv;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE|D3DFVF_TEX1)

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DTEXTURE9 g_pTexture = NULL;

bool LoadTextureFromBMP(BYTE* buffer, int width, int height);
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT InitD3D(HWND hwnd);
VOID Render();
VOID Cleanup();
VOID PlayVideo();

HRESULT InitGeometry();

struct MYBITMAPINFO
{
	BITMAPFILEHEADER bFile;
	BITMAPINFOHEADER bInfo;
	LPBYTE data;
};
bool initImage = false;
MYBITMAPINFO* imges[2];


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL,
		NULL,NULL,NULL, "D3D Tutorial",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Tutorial 01: CreateDevice", WS_OVERLAPPEDWINDOW, 100, 100, 800, 800,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	bool isPlay = false;
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		av_register_all();
		avformat_network_init();

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

				if (!isPlay)
				{
					isPlay = true;
					PlayVideo();
				}
			
			}
		}
	}

	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}
struct RGBFRAME
{
	int width;
	int height;
	int stride;
	BYTE* data[];
};
VOID PlayVideo()
{
	const char *szFilePath = "C:\\Users\\Bin\\Documents\\Visual Studio 2015\\Projects\\ffmpegSample\\x64\\Debug\\somthing.mp4";
	int ret = 0;

	AVFormatContext *pFmtCtx = NULL;
	ret = avformat_open_input(&pFmtCtx, szFilePath, NULL, NULL);

	ret = avformat_find_stream_info(pFmtCtx, NULL);

	int nVSI = -1;
	int nASI = -1;
	int i;

	for (i = 0; i < pFmtCtx->nb_streams; i++) {
		if (nVSI < 0 && pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			nVSI = i;
		}
		else if (nASI < 0 && pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			nASI = i;
		}
	}

	if (nVSI < 0 && nASI < 0) {
		av_log(NULL, AV_LOG_ERROR, "No Video & Audio Streams were Found\n");
		exit(-1);
	}

	AVCodec *pVideoCodec = avcodec_find_decoder(pFmtCtx->streams[nVSI]->codec->codec_id);

	avcodec_open2(pFmtCtx->streams[nVSI]->codec, pVideoCodec, NULL);
	AVCodecContext *pVCtx = pFmtCtx->streams[nVSI]->codec;
	AVCodecContext *pACtx = pFmtCtx->streams[nASI]->codec;
	SwsContext* ctx = NULL;
	AVPacket pkt;
	AVFrame* pVFrame, *pAFrame, *rgbFrame;

	int bGotPicture = 0;
	int bGotSound = 0;
	pVFrame = av_frame_alloc();
	pAFrame = av_frame_alloc();
	int idx = 0;
	while (av_read_frame(pFmtCtx, &pkt) >= 0) {
		idx++;
		if (pkt.stream_index == nVSI) {
			if (avcodec_decode_video2(pVCtx, pVFrame, &bGotPicture, &pkt) >= 0) {
				if(g_pTexture != nullptr)
					g_pTexture->Release();

				if (ctx == NULL)
				{
					ctx = sws_getContext(pVFrame->width, pVFrame->height, AV_PIX_FMT_YUV420P, pVFrame->width, pVFrame->height, AV_PIX_FMT_RGB24, 0, 0, 0, 0);
				}
				AVPicture dstPic;
				int rgbSize = avpicture_get_size(AV_PIX_FMT_RGB24, pVFrame->width, pVFrame->height);

				uint8_t *buffer = (uint8_t *)malloc(rgbSize*sizeof(uint8_t));
				avpicture_fill(&dstPic, buffer, AV_PIX_FMT_BGR24, pVFrame->width, pVFrame->height);
				const int inLineSize[1] = { pVFrame->width };
				int ret = sws_scale(ctx, pVFrame->data, pVFrame->linesize, 0, pVFrame->height, dstPic.data, dstPic.linesize);
				LoadTextureFromBMP((BYTE*)buffer, pVFrame->width, pVFrame->height);
				Render();
				free(buffer);
			}
		}
		else if (pkt.stream_index == nASI) {
			if (avcodec_decode_audio4(pACtx, pAFrame, &bGotSound, &pkt) >= 0) {
		
			}
		}

		::Sleep(10);
		av_free_packet(&pkt);
	}

	av_free(pVFrame);
	av_free(pAFrame);

	avformat_close_input(&pFmtCtx);

	avformat_network_deinit();
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

VOID Render()
{

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0F, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//SetupMatrices();
		g_pd3dDevice->SetTexture(0, g_pTexture);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

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

bool LoadTextureFromBMP(BYTE* buffer, int width, int height)
{
	LPDIRECT3DSURFACE9	surface;

	D3DLOCKED_RECT	rect;
	DWORD	w, h;
	INT		x, y;
	LPDWORD	data;

	//텍스쳐 사이즈를 2의 승수에 맞춘다.
	w = width;
	h = height;

	//텍스쳐를 만든다.
	if (FAILED(g_pd3dDevice->CreateTexture(w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &g_pTexture, NULL)))
		goto error;

	//Surface를 얻어온다.
	if (FAILED(g_pTexture->GetSurfaceLevel(0, &surface)))
		goto error;

	surface->LockRect(&rect, NULL, 0); //D3DLOCK_DISCARD);

	long int dataIdx = 0;

	BYTE *p = buffer;
	for (y = h-1 ; y >= 0; y--)
	{
		data = (LPDWORD)((LPBYTE)rect.pBits + rect.Pitch * y);
		
		for (x = 0; x<w; x++, data++)
		{
 			BYTE b,g,r;
			b = buffer[dataIdx]; dataIdx++;
			g = buffer[dataIdx]; dataIdx++;
			r = buffer[dataIdx]; dataIdx++;
			*data = 0xff000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | ((DWORD)b);
		}
	}

	surface->UnlockRect();

	surface->Release();

	return true;
error:
	if (g_pTexture)
		g_pTexture->Release();

	return false;
}

HRESULT InitGeometry()
{
	//bool isSuccess = LoadTextureFromBMP(0);

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
