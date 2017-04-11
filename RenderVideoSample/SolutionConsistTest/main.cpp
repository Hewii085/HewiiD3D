#include <libavformat/avformat.h>
#pragma comment( lib, "avformat.lib" )
#pragma comment( lib, "avutil.lib" )

#include <Windows.h>

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	av_log(NULL, AV_LOG_INFO, "Hello FFmpeg\n");
	av_register_all();
	avformat_network_init();
	int ret;

	AVFormatContext* pFmtCtx = NULL;
	ret = avformat_open_input(&pFmtCtx, "somthing.mp4", NULL, NULL);

	if (ret != 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Error");
		exit(-1);
	}

	ret = avformat_find_stream_info(pFmtCtx, NULL);

	if (ret != 0)
	{

	}

	return 0;
}