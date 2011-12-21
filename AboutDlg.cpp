#include "AboutDlg.h"

BOOL CALLBACK AboutDlg_dlgProcOptions(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_COMMAND && LOWORD(wParam) == IDOK) {
		::EndDialog(hwnd, 0);
		return TRUE;
	}
	return FALSE;
}