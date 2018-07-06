
#define _CRT_SECURE_NO_WARNINGS 1
#include "Dialog.h"
#include "resource.h"
#include <commctrl.h>

// If 'primary language id not a number' caused, insert '#include <Windows.h>' to Resource.rc
// If 'DS_SETFONT' error caused, insert '#include <winres.h>'

#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)
	
const PLUGIN_PARAMS	g_defaultParams = { "./fc.bmp",0 };
PLUGIN_PARAMS *g_pParams;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams) {
	g_pParams = pParams;
	return IDOK == DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

typedef struct _RANGE { int min, max; } RANGE;

LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
//	static RANGE ranges[VAL_NUM] = {{1, 4096}, {0, 10 * 100}, {0, 4096}, {0, 4096}};
	static HWND hEditFilename, hEditRadius, hEditFactor;
	static HWND hSpinRadius, hSpinFactor;
	static HWND hRadioUFile, hRadioUKer;
	char val[512];
	
	switch(msg) {
		case WM_COMMAND: {
			switch(LOWORD(wp)) {
				case IDOK:
					GetWindowText(hEditFilename, g_pParams->noyauNonFichier, sizeof(g_pParams->noyauNonFichier));
					GetWindowText(hEditRadius, val, sizeof(val));
					g_pParams->rayon = atoi(val);
					GetWindowText(hEditFactor, val, sizeof(val));
					g_pParams->facteur = atof(val);

					//pParam[i]	= ::atoi(sValue);
					//g_pParams->bFill = ::SendMessage(hChecks[0], BM_GETCHECK, 0, 0);
					//g_pParams->bLeaveSelection = ::SendMessage(hChecks[1], BM_GETCHECK, 0, 0);
					EndDialog(hWnd, IDOK);	break;
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);	break;
				//case IDRESET:
				//	*g_pParams = g_defaultParams;	break;
				default:	return false;
			}
			return false;
		}
		
		case WM_INITDIALOG: {
			hEditFilename = GetDlgItem(hWnd, IDC_EDIT_KerFilename);
			hEditRadius = GetDlgItem(hWnd, IDC_EDIT_Radius);
			hEditFactor = GetDlgItem(hWnd, IDC_EDIT_Factor);

			SetWindowText(hEditFilename, g_pParams->noyauNonFichier);
			SetWindowText(hEditRadius, _itoa(g_pParams->rayon, val, 10));
			SetWindowText(hEditFactor, _itoa(g_pParams->facteur, val, 10));

			hSpinRadius = GetDlgItem(hWnd, IDC_SPIN_Radius);
			hSpinFactor = GetDlgItem(hWnd, IDC_SPIN_Factor);
			hRadioUFile = GetDlgItem(hWnd, IDC_RADIO_UseFile);
			hRadioUKer = GetDlgItem(hWnd, IDC_RADIO_UseKer);

/*			hEdits[0]	= ::GetDlgItem(hWnd, IDC_EDIT_LINE);
			hEdits[1]	= ::GetDlgItem(hWnd, IDC_EDIT_SHARP);
			hEdits[2]	= ::GetDlgItem(hWnd, IDC_EDIT_OFFSET);
			hEdits[3]	= ::GetDlgItem(hWnd, IDC_EDIT_RAND);
			
			hSpins[0]	= ::GetDlgItem(hWnd, IDC_SPIN_LINE);
			hSpins[1]	= ::GetDlgItem(hWnd, IDC_SPIN_SHARP);
			hSpins[2]	= ::GetDlgItem(hWnd, IDC_SPIN_OFFSET);
			hSpins[3]	= ::GetDlgItem(hWnd, IDC_SPIN_RAND);
			
			REP(i, VAL_NUM) {
				::SendMessage(hSpins[i], UDM_SETBUDDY, (WPARAM)hEdits[i], 0);
				::SendMessage(hSpins[i], UDM_SETRANGE32, ranges[i].min, ranges[i].max);
				::SendMessage(hSpins[i], UDM_SETPOS, 0, (LPARAM)MAKELONG(pParam[i], 0));
			}

			hChecks[0]	= ::GetDlgItem(hWnd, IDC_CHECK_FILL);
			hChecks[1] = ::GetDlgItem(hWnd, IDC_CHECK_SELECTION);
			::SendMessage(hChecks[0], BM_SETCHECK, g_pParams->bFill, 0);
			::SendMessage(hChecks[1], BM_SETCHECK, g_pParams->bLeaveSelection, 0);
*/
		}
		default:	return false;
	}
	return true;
}
