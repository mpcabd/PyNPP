#include "OptionsDlg.h"
#include "globals.h"
#include "utility.h"
#include "resource.h"

#include <shlobj.h>

std::wstring C2W(const char* src){
	if (strlen(src) == 0)
		return L"";
	return wstring(src, src + strlen(src));
}

int CALLBACK BrowseForFolderCallBack(HWND hwnd, UINT message, LPARAM lParam, LPARAM lpData) {
	if (message == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, 1, (LPARAM)struOptions.pythonPath.c_str());
	}
	return 0;
}

BOOL CALLBACK dlgProcOptions(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND editPath = GetDlgItem(hwnd, IDC_EDIT_PATH);
	switch (message) {
		case WM_INITDIALOG:
		{
			SendMessage(editPath,
				WM_SETTEXT,
				0,
				(LPARAM)(struOptions.pythonPath.c_str()));
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_BUTTON_BROWSE: {
					TCHAR path[MAX_PATH];
					BROWSEINFO bi = { 0 };
					bi.lpfn = BrowseForFolderCallBack;
					bi.lpszTitle = TEXT("Select the folder that contains Python executable:");
					LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	
					if (pidl != 0) {
						SHGetPathFromIDList(pidl, path);
						SetDlgItemText(hwnd, IDC_EDIT_PATH, path);
					}

					return TRUE;
				}
				case IDCLOSE:
				case IDCANCEL:
					loadOption(nppData._nppHandle, struOptions);
					::EndDialog(hwnd, 0);
					return TRUE;
				case IDOK:
					int textLength = SendMessage(editPath, WM_GETTEXTLENGTH, 0, 0);
					wchar_t* buffer = new wchar_t[textLength + 1];
					GetDlgItemText(hwnd, IDC_EDIT_PATH, buffer, textLength + 1);
					wstring value(buffer);
					struOptions.pythonPath = value;
					saveOption(nppData._nppHandle, struOptions);
					loadOption(nppData._nppHandle, struOptions);
					::EndDialog(hwnd, 0);
					return TRUE;
			}
			return FALSE;
	}
	return FALSE;
}