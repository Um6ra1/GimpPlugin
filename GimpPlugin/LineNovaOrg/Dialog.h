#pragma once
#include <Windows.h>

typedef struct _PLUGIN_PARAMS {
	int	lineNum;
	int	degCornerAngX100;	// Sharpness
	int	offset;	// Offset radius
	int	variation;	// Rondomness
	int	bFill;
	int	bLeaveSelection;
} PLUGIN_PARAMS;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams);
LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

extern const PLUGIN_PARAMS	g_defaultParams;
