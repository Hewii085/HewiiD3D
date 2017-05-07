#include "Dib.h"
#include <stdio.h>
#include <math.h>
#include<Windows.h>

#define DEFAULT_XPIXELSPERMETER 3000
#define DEFAULT_YPIXELSPERMETER 3000

LPBYTE DibLoadHandle(LPSTR lpFileName)
{
	FILE *fp;
	BITMAPFILEHEADER bmfh;
	HGLOBAL hDib;
	LPBYTE lpDib;
	int nSize;

	fopen_s(&fp, lpFileName, "rb");
	if (!fp) return NULL;

	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	nSize = bmfh.bfSize - sizeof(BITMAPFILEHEADER);
	hDib = (HGLOBAL)GlobalAlloc(GMEM_FIXED, nSize);
	lpDib = (LPBYTE)GlobalLock(hDib);

	if (!lpDib)
	{
		fclose(fp);
		return NULL;
	}

	fread(lpDib, nSize, 1, fp);
	fclose(fp);

	return lpDib;
}

bool DibSave(LPBYTE lpDib, LPSTR lpFileName)
{
	FILE *fp;
	BITMAPFILEHEADER bmfh;

	if (!lpDib || !lpFileName)
		return false;

	fopen_s(&fp, lpFileName, "wb");

	if (!fp)
		return false;

	bmfh.bfType = 0x4d42;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + DIB_SIZE(lpDib);
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + DIB_PALSIZE(lpDib);
	fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(lpDib, DIB_SIZE(lpDib), 1, fp);
	fclose(fp);

	return true;

}

LPBYTE DibCreateEmpty(int nBitsPerPixel, int nWidth, int nHeight)
{
	LPBITMAPINFOHEADER lpBMIH;
	LPBYTE lpDib;
	int nPal = nBitsPerPixel == 24 ? 0 : 1 << nBitsPerPixel;
	int nPalSize = sizeof(RGBQUAD) * nPal;
	int nLineSize = ALIGN_4B(nWidth * nBitsPerPixel / 8);
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + nPalSize + nLineSize * nHeight;

	lpDib = new BYTE[dwSize];
	memset(lpDib, 0xff, dwSize);
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biWidth = nWidth;
	lpBMIH->biHeight = nHeight;
	lpBMIH->biPlanes = 1;
	lpBMIH->biBitCount = nBitsPerPixel;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biSizeImage = 0;
	lpBMIH->biXPelsPerMeter = DEFAULT_XPIXELSPERMETER;
	lpBMIH->biYPelsPerMeter = DEFAULT_YPIXELSPERMETER;
	lpBMIH->biClrUsed = nPal;
	lpBMIH->biClrImportant = nPal;

	RGBQUAD rgbQuad;
	int i;

	for (i = 0; i < nPal;i++)
	{
		rgbQuad.rgbBlue = i;
		rgbQuad.rgbGreen = i;
		rgbQuad.rgbRed = i;
		rgbQuad.rgbReserved = 0;
		DIB_BMI(lpDib)->bmiColors[i] = rgbQuad;
	}

	return lpDib;

}

LPBYTE DibCreateEmptyHandle(int nBitsPerPixel, int nWidth, int nHeight)
{
	LPBITMAPINFOHEADER lpBMIH;
	HGLOBAL hDib;
	LPBYTE lpDib;
	int nPal = nBitsPerPixel == 24 ? 0 : 1 << nBitsPerPixel;
	int nPalSize = sizeof(RGBQUAD) * nPal;
	int nLineSIze = ALIGN_4B(nWidth * nBitsPerPixel / 8);
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + nPalSize + nLineSIze * nHeight;

	hDib = (HGLOBAL)GlobalAlloc(GMEM_FIXED ,dwSize);
	lpDib = (LPBYTE)GlobalLock(hDib);
	memset(lpDib, 0xff, dwSize);
	lpBMIH = (LPBITMAPINFOHEADER)lpDib;
	lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
	lpBMIH->biWidth = nWidth;
	lpBMIH->biHeight = nHeight;
	lpBMIH->biPlanes = 1;
	lpBMIH->biBitCount = nBitsPerPixel;
	lpBMIH->biCompression = BI_RGB;
	lpBMIH->biSizeImage = 0;
	lpBMIH->biXPelsPerMeter = DEFAULT_XPIXELSPERMETER;
	lpBMIH->biYPelsPerMeter = DEFAULT_YPIXELSPERMETER;
	lpBMIH->biClrUsed = nPal;
	lpBMIH->biClrImportant = nPal;

	RGBQUAD rgbQuad;
	int i;
	for (i = 0; i < nPal; i++)
	{
		rgbQuad.rgbBlue = i;
		rgbQuad.rgbGreen = i;
		rgbQuad.rgbRed = i;
		rgbQuad.rgbReserved = 0;
		DIB_BMI(lpDib)->bmiColors[i] = rgbQuad;
	}

	return lpDib;
}

void DibDeleteHandle(LPBYTE lpSrc)
{
	if (lpSrc)
	{
		GlobalUnlock(lpSrc);
		GlobalFree(lpSrc);
	}
}

LPBYTE DibDuplicate(LPBYTE lpSrc)
{
	if (!lpSrc)
		return NULL;

	HGLOBAL hDib;
	LPBYTE lpDib;
	DWORD dwSize = DIB_SIZE(lpSrc);

	lpDib = new BYTE[dwSize];

	if (lpDib)
		memcpy(lpDib, lpSrc, dwSize);

	return lpDib;
}

LPBYTE DibDuplicateHandle(LPBYTE lpSrc)
{
	if (!lpSrc)
		return NULL;

	HGLOBAL hDib;
	LPBYTE lpDib;
	DWORD dwSize = DIB_SIZE(lpSrc);

	hDib = (HGLOBAL)GlobalAlloc(GMEM_FIXED, dwSize);
	lpDib = (LPBYTE)GlobalLock(hDib);

	if (lpDib)
		memcpy(lpDib, lpSrc, dwSize);

	return lpDib;
}

bool DibCopyRect(LPBYTE lpDstDib, int dstx, int dsty, LPBYTE lpSrcDib, LPRECT prSrc1)
{
	if (!lpDstDib || !lpSrcDib || DIB_BPP(lpDstDib) != DIB_BPP(lpSrcDib))
		return false;

	RECT	rDst, rDst1, rDst2, rSrc2, rSrc;

	SetRect(&rSrc2, 0, 0, DIB_CX(lpSrcDib), DIB_CY(lpSrcDib));
	IntersectRect(&rSrc, prSrc1, &rSrc2);
	SetRect(&rDst1, 0, 0, rSrc.right - rSrc.left, rSrc.bottom - rSrc.top);
	OffsetRect(&rDst1, dstx, dsty);
	SetRect(&rDst2, 0, 0, DIB_CX(lpDstDib), DIB_CY(lpDstDib));
	IntersectRect(&rDst, &rDst1, &rDst2);

	LPBYTE	lpSrc, lpDst;
	int		y, nWidth = (rDst.right - rDst.left) * DIB_BPP(lpSrcDib) / 8;

	for (y = rDst.top; y < rDst.bottom; y++)
	{
		lpSrc = DIB_DATAXY_INV(lpSrcDib, rSrc.left, rSrc.top + y - rDst.top);
		lpDst = DIB_DATAXY_INV(lpDstDib, rDst.left, y);
		memcpy(lpDst, lpSrc, nWidth);
	}

	return true;
}


bool DibCopyRectROP(LPBYTE lpDstDib, int dstx, int dsty, LPBYTE lpSrcDib, LPRECT prSrc1, DWORD dwROP)
{
	if (!lpDstDib || !lpSrcDib || DIB_BPP(lpDstDib) != DIB_BPP(lpSrcDib))
		return false;

	RECT rDst, rDst1, rDst2, rSrc2, rSrc;

	SetRect(&rSrc2, 0, 0, DIB_CX(lpSrcDib), DIB_CY(lpSrcDib));
	IntersectRect(&rSrc, prSrc1, &rSrc2);
	SetRect(&rDst1, 0, 0, rSrc.right - rSrc.left, rSrc.bottom - rSrc.top);
	OffsetRect(&rDst1, dstx, dsty);
	SetRect(&rDst2, 0, 0, DIB_CX(lpDstDib), DIB_CY(lpDstDib));
	IntersectRect(&rDst, &rDst1, &rDst2);

	LPBYTE lpSrc, lpDst;
	int y, i, nWidth = (rDst.right - rDst.left)*DIB_BPP(lpSrcDib) / 8;

	for (y = rDst.top; y < rDst.bottom; y++)
	{
		lpSrc = DIB_DATAXY_INV(lpSrcDib, rSrc.left, rSrc.top + y - rDst.top);
		lpDst = DIB_DATAXY_INV(lpDstDib, rDst.left, y);

		switch (dwROP)
		{
		case SRCAND:
			for (i = 0; i < nWidth; i++)
			{
				*lpDst = *lpDst & *lpSrc;
				lpDst++;
				lpSrc++;
			}
			break;
		case SRCPAINT:
			for (i = 0; i < nWidth; i++)
			{
				*lpDst = *lpDst | *lpSrc;
				lpDst++;
				lpSrc++;
			}
			break;
		case SRCCOPY:
			memcpy(lpDst, lpSrc, nWidth);
			break;
		default:
			break;
		}
	}

	return true;
}


