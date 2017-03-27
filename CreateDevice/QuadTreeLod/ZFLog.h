#pragma once
#include <Windows.h>
#include <stdio.h>
#include "define.h"

#define ZF_LOG_TARGET_CONSOLE 0x00000001
#define ZF_LOG_TARGET_FILE    0x00000002
#define ZF_LOG_TARGET_WINDOW  0x00000004
#define ZF_LOG_TARGET_ALL (ZF_LOG_TARGET_CONSOLE|ZF_LOG_TARGET_FILE|ZF_LOG_TARGET_WINDOW)
#define ZF_LOG_WINDOW_TITLE "ZEngine F_Killer"
#define ZF_LOG_WINDOW_CX 300
#define ZF_LOG_WINDOW_CY 600

class ZFLog
{
public:
	ZFLog(UINT32 nTarget, LPSTR szFilename = NULL);
	~ZFLog();
	void CreateLogWindow();
	int Log(LPSTR fmt, ...);

private:
	static LRESULT CALLBACK
		ZFLog::WndProcZFLog(HWND hwnd, UINT uMsg, WPARAM wPAram, LPARAM lParam);

};

DECLARE  ZFLog* g_pLog;
