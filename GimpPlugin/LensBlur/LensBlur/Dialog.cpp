#include "Dialog.h"
#include "Resource.h"
#include <commctrl.h>

// If 'primary language id not a number' caused, insert '#include <Windows.h>' to Resource.rc

#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)
	
const PLUGIN_PARAMS	g_defaultParams = { "./fc.bmp",0 };
PLUGIN_PARAMS *g_pParams;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams) {
	g_pParams = pParams;
	return IDOK == DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

typedef struct _RANGE { int min, max; } RANGE;

LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
//	static HWND	hEdits[VAL_NUM];
//	static HWND	hSpins[VAL_NUM];
//	static HWND	hChecks[CHK_NUM];
//	static RANGE ranges[VAL_NUM] = {{1, 4096}, {0, 10 * 100}, {0, 4096}, {0, 4096}};
	static HWND hEditFilename;
	//static int	*pParam = (int *)g_pParams;
	
	switch(msg) {
		case WM_COMMAND: {
			switch(LOWORD(wp)) {
				case IDOK:
					GetWindowText(hEditFilename, g_pParams->kernelFilename, sizeof(g_pParams->kernelFilename));
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
			SetWindowText(hEditFilename, g_pParams->kernelFilename);
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
