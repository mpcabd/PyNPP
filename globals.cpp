#include "globals.h"
#include "PluginInterface.h"

StruOptions struOptions;
NppData nppData;
HINSTANCE _hInst;
wstring pythonPath = L"";

bool getPythonLocationFromRegistry(std::wstring &loc, HKEY baseKey) {
	HKEY hKey;
	if(RegOpenKeyEx(baseKey, TEXT("Software\\Python\\PythonCore"), 0,
		KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return false;
	}
	DWORD dwIdx=0;
	TCHAR szKeyName[1024];
	DWORD dwSize=1024;
	FILETIME fTime;
	TCHAR pyPath[MAX_PATH];
	DWORD length = MAX_PATH;

	if (RegEnumKeyEx(hKey, dwIdx, szKeyName, &dwSize, NULL, NULL, NULL, &fTime) == ERROR_SUCCESS)
	{
		HKEY hSubKey;
		if (RegOpenKeyEx(hKey, szKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{
			LONG x = RegOpenKeyEx(hSubKey, TEXT("InstallPath"), 0, KEY_READ, &hSubKey);
			if (x == ERROR_SUCCESS)
			{
				if(RegQueryValueEx(
					hSubKey,
					TEXT(""),
					NULL,
					NULL,
					(LPBYTE)pyPath,
					&length) != ERROR_SUCCESS)
				{
					return false;
				}

				loc = loc.append(pyPath);
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool getPythonLocation(std::wstring &loc)
{
	if (getPythonLocationFromRegistry(loc, HKEY_CURRENT_USER))
		return true;
	else if (getPythonLocationFromRegistry(loc, HKEY_LOCAL_MACHINE))
		return true;
	else
		return false;
}

bool pythonExists(std::wstring foldername) {
	std::wstring path = L"C:\\";
	path += foldername;
	path += L"\\python.exe";
	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(const_cast<LPCWSTR>(path.c_str()), &data);
	return (h != INVALID_HANDLE_VALUE);
}

wstring GetPythonPath() {
	std::wstring pythonLoc = L"";
	bool pythonInstalled = false;

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(L"c:\\python*", &data);
	if( h != INVALID_HANDLE_VALUE ) 
	{
		do 
		{
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			char*   nPtr = new char [lstrlen( data.cFileName ) + 1];
			for( int i = 0; i < lstrlen( data.cFileName ); i++ )
				nPtr[i] = char( data.cFileName[i] );

			nPtr[lstrlen( data.cFileName )] = '\0';
			if (pythonExists(const_cast<LPWSTR>(data.cFileName)))
			{
				pythonLoc = L"C:\\";
				pythonLoc += const_cast<LPWSTR>(data.cFileName);
				pythonLoc += L"\\";
				pythonInstalled = true;
				break;
			}

			delete[] nPtr;
		} while (FindNextFile(h, &data));
	}

	if (!pythonInstalled)
		pythonInstalled = getPythonLocation(pythonLoc);

	return pythonLoc;
}