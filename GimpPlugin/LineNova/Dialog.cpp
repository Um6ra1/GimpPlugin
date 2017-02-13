#include "Dialog.h"
#include "Resource.h"
#include <commctrl.h>


#define REP(idx, max)	for(int idx = 0, idx##Max = max; idx < idx##Max; idx ++)
	
PLUGIN_PARAMS *g_pParams;

int	DoModal(HINSTANCE hInst, PLUGIN_PARAMS *pParams)
{
	g_pParams = pParams;
	int ret = ::DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)::DlgProc);
	
	return ret == IDOK;
}

typedef struct _RANGE
{	int min, max;	} RANGE;

LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
#define CTRL_NUM	4
	static HWND	hEdits[CTRL_NUM];
	static HWND	hSpins[CTRL_NUM];
	static RANGE	ranges[CTRL_NUM] = {{1, 4096}, {0, 10 * 100}, {0, 4096}, {0, 4096}};
	static int	*pParam = (int *)g_pParams;
	
	switch(msg)
	{
		case WM_COMMAND:
		{
			switch(LOWORD(wp))
			{
				case IDOK:
					REP(i, CTRL_NUM)
					{
						char sValue[0x10];
						::GetWindowText(hEdits[i], sValue, sizeof(sValue));
						pParam[i]	= ::atoi(sValue);
					}
					::EndDialog(hWnd, IDOK);	break;
				case IDCANCEL:
					::EndDialog(hWnd, IDCANCEL);	break;
				default:	return false;
			}
			/*if(HIWORD(wp) == EN_CHANGE)
			REP(i, CTRL_NUM)
			if((LPARAM)hEdits[i] == lp)
			{
				::MessageBox(0, "", "", MB_OK);
				char sValue[0x10];
				::GetWindowText(hEdits[i], sValue, sizeof(sValue));
				pParam[i]	= ::atoi(sValue);
			}*/

			return false;
		}
		
		case WM_INITDIALOG:
		{
			hEdits[0]	= ::GetDlgItem(hWnd, IDC_EDIT_LINE);
			hEdits[1]	= ::GetDlgItem(hWnd, IDC_EDIT_SHARP);
			hEdits[2]	= ::GetDlgItem(hWnd, IDC_EDIT_OFFSET);
			hEdits[3]	= ::GetDlgItem(hWnd, IDC_EDIT_RAND);
			
			hSpins[0]	= ::GetDlgItem(hWnd, IDC_SPIN_LINE);
			hSpins[1]	= ::GetDlgItem(hWnd, IDC_SPIN_SHARP);
			hSpins[2]	= ::GetDlgItem(hWnd, IDC_SPIN_OFFSET);
			hSpins[3]	= ::GetDlgItem(hWnd, IDC_SPIN_RAND);
			
			REP(i, CTRL_NUM)
			{
				::SendMessage(hSpins[i], UDM_SETBUDDY, (WPARAM)hEdits[i], 0);
				::SendMessage(hSpins[i], UDM_SETRANGE32, ranges[i].min, ranges[i].max);
				::SendMessage(hSpins[i], UDM_SETPOS, 0, (LPARAM)MAKELONG(pParam[i], 0));
				//::SetWindowText();
			}
		}
		
		default:	return false;
	}
	return(true);
}
