#pragma once
#include <Windows.h>

#define ALIGN_2B( a )				((a)%2 ? (a) + 1 : (a))
#define ALIGN_4B( a )				((a)%4 ? (a) + 4 - ((a)%4) : (a))

#define DIB_HEADER( a )				((LPBITMAPINFOHEADER)(a))
#define DIB_BMI( a )				((LPBITMAPINFO)(a))
#define DIB_CX( a )					(DIB_HEADER(a)->biWidth)
#define DIB_CY( a )					(DIB_HEADER(a)->biHeight)
#define DIB_BPP( a )				(DIB_HEADER(a)->biBitCount)
#define DIB_PALSIZE( a )			(DIB_BPP(a) == 24 ? 0 : sizeof(RGBQUAD) * (1 << DIB_BPP(a)))
#define DIB_LINESIZE ( a )			(ALIGN_4B(DIB_CX(a) * DIB_BPP(a) / 8))
#define DIB_DATA( a )				((LPBYTE)(a) + sizeof(BITMAPINFOHEADER) + DIB_PALSIZE(a))
#define DIB_DATASIZE( a )			(DIB_LINESIZE(a) * DIB_CY(a))
#define DIB_HEADERSIZE ( a )		(sizeof(BITMAPINFOHEADER))
#define DIB_SIZE ( a )				(DIB_HEADERSIZE(a) + DIB_PALSIZE(a) + DIB_DATASIZE(a))