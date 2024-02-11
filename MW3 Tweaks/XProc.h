#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

struct WindowData {
	DWORD pID;
	HWND hWnd;
};

bool GetProcessInfo(const wchar_t* procName, PROCESSENTRY32* pe32);

DWORD GetProcID(const wchar_t* procName);

bool GetProcessModules(const DWORD pID, std::vector<MODULEENTRY32>& modules);

// Returns window handle for specified process ID
HWND GetProcessWindow(const DWORD pID);

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);