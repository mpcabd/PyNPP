#include <Windows.h>
#include <tchar.h>
#include <Shlwapi.h>

#include "PluginInterface.h"
#include "IniFileProcessor.h"
#include "utility.h"

wstring StringToWString(const string& s)
{
	wstring temp(s.length(),L' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp; 
}

wstring GetConfigFilePath(HWND nppHandle)
{
	wstring strConfigFilePath;

	TCHAR szConfigDir[MAX_PATH];
	TCHAR szBuffer[MAX_PATH];
	szConfigDir[0] = 0;
	::SendMessage(nppHandle, NPPM_GETPLUGINSCONFIGDIR, 
		MAX_PATH, (LPARAM)szConfigDir);
	::PathCombine(szBuffer, szConfigDir, TEXT("PyNPP.ini"));
	strConfigFilePath = szBuffer;

	return strConfigFilePath;
}

void loadOption(HWND nppHandle, StruOptions& struOptions)
{
	loadDefaultOption(struOptions);
	wstring tsConfigFilePath = GetConfigFilePath(nppHandle);

	IniFileProcessor processor(tsConfigFilePath);
	IniFileProcessor::IniMap map;

	map = processor.GetInfo(true);
	IniFileProcessor::IniMap::iterator itrEnd = map.end();

	if(map.find(keyPythonPath) != itrEnd)
	{
		struOptions.pythonPath = StringToWString(map[keyPythonPath].GetStrValue());
		pythonPath = struOptions.pythonPath;
	}
}

void loadDefaultOption(StruOptions& struOptions)
{
	struOptions.pythonPath = GetPythonPath();
	pythonPath = struOptions.pythonPath;
}

void saveOption(HWND nppHandle, StruOptions struOptions)
{
	wstring tsConfigFilePath = GetConfigFilePath(nppHandle);

	IniFileProcessor processor(tsConfigFilePath);
	IniFileProcessor::IniMap map;
	string value(struOptions.pythonPath.begin(), struOptions.pythonPath.end());
	value.assign(struOptions.pythonPath.begin(), struOptions.pythonPath.end());
	map[keyPythonPath] = value;
	processor.SetMap(map);

	processor.Save();
}