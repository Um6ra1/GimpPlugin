#pragma once
#include <Windows.h>

typedef struct _PLUGIN_PARAMS {
	char kernelFilename[256];
	int radius;
} PLUGIN_PARAMS;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams);
LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

extern const PLUGIN_PARAMS	g_defaultParams;
