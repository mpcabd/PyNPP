//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "resource.h"
#include "OptionsDlg.h"
#include "AboutDlg.h"
#include <string>
#include <sstream>
#include <map>
using namespace std;
#include <windows.h>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
map<std::wstring, PROCESS_INFORMATION> pi_map;


void pluginInit(HANDLE hModule)
{
	_hInst = (HINSTANCE)hModule;
}

void pluginCleanUp()
{
}

void commandMenuInit()
{
	int idx = 0;
	ShortcutKey* key = new ShortcutKey();
	key->_isAlt = true;
	key->_isCtrl = true;
	key->_isShift = false;
	key->_key = 0x74; //F5
	setCommand(idx++, TEXT("Run file in Python"), runFile, key, false);

	key = new ShortcutKey();
	key->_isAlt = true;
	key->_isCtrl = false;
	key->_isShift = true;
	key->_key = 0x74;
	setCommand(idx++, TEXT("Run file in Python Interactive"), runFileInteractive, key, false);

	key = new ShortcutKey();
	key->_isAlt = true;
	key->_isCtrl = true;
	key->_isShift = true;
	key->_key = 0x74;
	setCommand(idx++, TEXT("Run file in PythonW"), runFileW, key, false);

	key = new ShortcutKey();
	setCommand(idx++, TEXT("Debug the file (PDB)"), runFilePDB, key, false);

	lstrcpy(funcItem[idx++]._itemName, TEXT("-SEPARATOR-"));

	setCommand(idx++, TEXT("Options"), showOptionsDlg, 0, false);
	setCommand(idx++, TEXT("About PyNPP"), showAboutDlg, 0, false);
}

void commandMenuCleanUp()
{

}

void showOptionsDlg() {
	INT_PTR nRet = ::DialogBox(_hInst, 
							MAKEINTRESOURCE(IDD_OPTIONS_DIALOG),
							nppData._nppHandle, 
							(DLGPROC)dlgProcOptions);
}

void showAboutDlg() {
	INT_PTR nRet = ::DialogBox(_hInst, 
							MAKEINTRESOURCE(IDD_ABOUT_DIALOG),
							nppData._nppHandle, 
							(DLGPROC)AboutDlg_dlgProcOptions);
}

bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
	if (index >= nbFunc)
		return false;

	if (!pFunc)
		return false;

	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}

// My Functions Side :)
std::wstring intToString(int value) {
	stringstream ss;
	ss << value;
	std::wstring result;
	for (int i = 0; i < ss.str().length(); i++)
		result += ss.str()[i];
	return result;
}

std::wstring getCurrentFile(bool &ok)
{
	LRESULT result = ::SendMessage(nppData._nppHandle, NPPM_SAVECURRENTFILE, 0, 0);
	TCHAR path[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, MAX_PATH, (LPARAM)path);
	std::wstring wPath(path);
	if (result == 0) {
		ok = false;
		for (int i = 0; i < wPath.length() && !ok; i++)
			if (wPath[i] == '\\')
				ok = true;
	}
	else
		ok = true;
	return wPath;
}

std::wstring buildRunCommand(std::wstring &filePath, std::wstring &pypath, bool isW = false, bool isI = false, bool isPDB = false)
{
	std::wstring command = pypath;
	if (command[command.length() - 1] != '\\')
		command += TEXT("\\");
	command += TEXT("python");
	if (isW)
		command += TEXT("w");
	command += TEXT(".exe ");
	if (isI)
		command += TEXT("-i ");
	if (isPDB)
		command += TEXT("-m pdb ");
	command += TEXT("\"");
	command += filePath;
	command += TEXT("\"");
	return command;
}

bool launchPython(std::wstring &command, std::wstring &path)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	bool ok = false;
	std::wstring filename = getCurrentFile(ok);
	if (ok) {
		map<std::wstring, PROCESS_INFORMATION>::iterator it = pi_map.find(filename);
		if (it != pi_map.end()) {
			TerminateProcess(it->second.hProcess, 1);
			pi_map.erase(it);
		}
	}

	bool result = CreateProcess(
		NULL,
		const_cast<LPWSTR>(command.c_str()),
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		NULL,
		path.c_str(),
		&si,
		&pi) != 0;

	if (result)
		pi_map[filename] = pi;

	return result;
}

void run(bool isW, bool isI, bool isPDB) {
	std::wstring pythonLoc = pythonPath;

	if(pythonPath == L"")
	{
		MessageBox(NULL, TEXT("Could not locate Python"), TEXT("Run Failed"), 0);
		return;
	}

	bool ok = false;

	std::wstring file = getCurrentFile(ok);
	if (ok) {
		std::wstring path = L"";
		for (int i = file.length() - 1; i >= 0; i--) {
			if (file[i] == '\\' || file[i] == '/') {
				path = file.substr(0, i + 1);
				break;
			}
		}
		std::wstring command = buildRunCommand(file, pythonLoc, isW, isI, isPDB);

		if(!launchPython(command, path)) {
			int errorCode = GetLastError();
			stringstream ss;
			ss << "Couldn't launch Python.\nError Code: " << errorCode;
			wstring message;
			for (int i = 0; i < ss.str().length(); i++) {
				message += ss.str()[i];
			}
			message += TEXT(".\nCommand: [");
			message += command;
			message += TEXT("]\nPath: [");
			message += path;
			message += TEXT("].");
			MessageBox(NULL, message.c_str(), TEXT("Run Failed"), 0);
		}
	} else {
		MessageBox(NULL, TEXT("You should save your file to be able to run it."), TEXT("Run Failed"), 0);
	}
}

void runFile() {
	run(false, false, false);
}

void runFileInteractive() {
	run(false, true, false);
}

void runFileW() {
	run(true, false, false);
}

void runFilePDB() {
	run(false, false, true);
}
