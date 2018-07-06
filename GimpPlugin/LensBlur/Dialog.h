#pragma once
#include <Windows.h>

typedef enum {
	UtilFichier,
	UtilNoyau,
} QuelMethode;

typedef struct _PLUGIN_PARAMS {
	char noyauNonFichier[256];
	int rayon;
	double facteur;
	QuelMethode qm;
} PLUGIN_PARAMS;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams);
LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

extern const PLUGIN_PARAMS	g_defaultParams;
