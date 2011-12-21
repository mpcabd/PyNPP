#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>

using namespace std;

const string keyPythonPath("Path");

wstring GetConfigFilePath(HWND nppHandle);

void loadOption(HWND nppHandle, StruOptions& struOptions);
void loadDefaultOption(StruOptions& struOptions);
void saveOption(HWND nppHandle, StruOptions struOptions);

#endif