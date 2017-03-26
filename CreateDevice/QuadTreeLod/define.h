#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#ifndef MAINBODY
#define DECLARE extern
#else
#define DECALARE
#endif

#ifndef DEL
	#define DEL(p) {if(p){delete (p); (p) = NULL;}}
#endif

#ifndef DELS
#define DELS(p) {if(p) {delete[] (p); (p) = NULL;}}
#endif

#ifndef REL
#define REL(p) {if(p) { (p)->Release(); (p) = NULL;}}
#endif

struct TERRAINVERTEX
{
	enum _FVF {FVF=(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) };
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR2 t;
};