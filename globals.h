#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <windows.h>
#include <vector>
#include <string>

using namespace std;

struct StruOptions {
	wstring pythonPath;
};

struct NppData {
	HWND _nppHandle;
	HWND _scintillaMainHandle;
	HWND _scintillaSecondHandle;
};

wstring GetPythonPath();

extern StruOptions struOptions;
extern NppData nppData;
extern HINSTANCE _hInst;
extern wstring pythonPath;
#endif