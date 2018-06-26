
#define _CRT_SECURE_NO_WARNINGS 1
#include "Dialog.h"
#include "Resource.h"
#include <commctrl.h>
#include <stdio.h>

#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)

typedef struct { int min, max; } Range;

const PLUGIN_PARAMS	g_defaultParams = { 1, 1, 1, 1 };
PLUGIN_PARAMS *g_pParams;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams) {
	g_pParams = pParams;
	return IDOK == DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

extern void	Msg(PCSTR psFmt, ...);
LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND hEditDx1, hEditDy1, hEditDx2, hEditDy2;
	static HWND hSpinDx1, hSpinDy1, hSpinDx2, hSpinDy2;
	char val[512];

	switch(msg) {
		case WM_COMMAND: {
			if (lp == (LPARAM)hEditDx1 && HIWORD(wp) == EN_CHANGE) {
				int pos = SendMessage(hSpinDx1, UDM_GETPOS, 0, 0);
				sprintf(val, "%d", pos);
				//SetWindowTextA(hEditDx1, val);
					//Msg("%d", _itoa(SendMessage(hSpinDx1, UDM_GETPOS, 0, 0), val, 10));
					//InvalidateRect();
				return true;
				
			}
			switch(LOWORD(wp)) {
				case IDOK:
					::GetWindowText(hEditDx1, val, sizeof(val));
					g_pParams->dx1 = ::atoi(val);
					::GetWindowText(hEditDx2, val, sizeof(val));
					g_pParams->dx2 = ::atoi(val);
					::GetWindowText(hEditDy1, val, sizeof(val));
					g_pParams->dy1 = ::atoi(val);
					::GetWindowText(hEditDy2, val, sizeof(val));
					g_pParams->dy2 = ::atoi(val);
					EndDialog(hWnd, IDOK);	break;
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);	break;
				//case IDRESET:
				//	*g_pParams = g_defaultParams;	break;
				default:	return false;
			}
			//if (HIWORD(wp) == EN_CHANGE) {
			//	if (lp == (LPARAM)hEditDx1) {
			return false;
		}
		
		case WM_INITDIALOG: {
			hEditDx1 = GetDlgItem(hWnd, IDC_EDIT_DX1);
			hEditDx2 = GetDlgItem(hWnd, IDC_EDIT_DX2);
			hEditDy1 = GetDlgItem(hWnd, IDC_EDIT_DY1);
			hEditDy2 = GetDlgItem(hWnd, IDC_EDIT_DY2);
			SetWindowText(hEditDx1, _itoa(g_pParams->dx1, val, 10));
			SetWindowText(hEditDx2, _itoa(g_pParams->dx2, val, 10));
			SetWindowText(hEditDy1, _itoa(g_pParams->dy1, val, 10));
			SetWindowText(hEditDy2, _itoa(g_pParams->dy2, val, 10));

			hSpinDx1 = GetDlgItem(hWnd, IDC_SPIN_DX1);
			hSpinDx2 = GetDlgItem(hWnd, IDC_SPIN_DX2);
			hSpinDy1 = GetDlgItem(hWnd, IDC_SPIN_DY1);
			hSpinDy2 = GetDlgItem(hWnd, IDC_SPIN_DY2);
			
			Range r = {0, 1024};
			SendMessage(hSpinDx1, UDM_SETBUDDY, (WPARAM)hEditDx1, 0);
			SendMessage(hSpinDx1, UDM_SETRANGE32, r.min, r.max);
			SendMessage(hSpinDx1, UDM_SETPOS, 0, MAKELONG(g_pParams->dx1, 0));
			SendMessage(hSpinDx2, UDM_SETBUDDY, (WPARAM)hEditDx2, 0);
			SendMessage(hSpinDx2, UDM_SETRANGE32, r.min, r.max);
			SendMessage(hSpinDx2, UDM_SETPOS, 0, MAKELONG(g_pParams->dx2, 0));
			SendMessage(hSpinDy1, UDM_SETBUDDY, (WPARAM)hEditDy1, 0);
			SendMessage(hSpinDy1, UDM_SETRANGE32, r.min, r.max);
			SendMessage(hSpinDy1, UDM_SETPOS, 0, MAKELONG(g_pParams->dy1, 0));
			SendMessage(hSpinDy2, UDM_SETBUDDY, (WPARAM)hEditDy2, 0);
			SendMessage(hSpinDy2, UDM_SETRANGE32, r.min, r.max);
			SendMessage(hSpinDy2, UDM_SETPOS, 0, MAKELONG(g_pParams->dy2, 0));
			return true;
		}
		default: return false;
	}
	return true;
}
