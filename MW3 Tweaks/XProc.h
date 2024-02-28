#pragma once

#include <string>
#include <ctime>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

struct WindowData {
	DWORD pID;
	HWND hWnd;
};

class XProcess {
private:
	std::wstring procName;
	HWND hWnd;
	HANDLE hProc;
	PROCESSENTRY32 pe32;
public:
	XProcess(std::wstring procName);
	~XProcess();
public:
	// Waits the process if it isn't open. Then open a handle to it.
	// You can supply a timeout in seconds for the function to exit 
	// if the application isn't found 
	bool WaitForProcess(double timeoutSec = 0);

	// Fetches a list of all the present modules in an application.
	// Return true if module list was successfully fetched.
	bool GetModuleList(std::vector<MODULEENTRY32> &modules);

	// Returns true if a module with the given name is present in an application
	bool IsModulePresent(const std::wstring& moduleName);

	// Fetches all of the process info in the PROCESSENTRY32 struct
	// Return true if the process info is successfully fetched
	bool GetProcessInfo();

	bool GetProcessWindow();

	template <class T>
	bool WriteMemory(DWORD baseAddress, const T &buffer) {
		return WriteProcessMemory(hProc, (LPVOID)baseAddress, (LPCVOID)&buffer, sizeof(T), NULL);
	}

	template <class T>
	bool ReadMemory(DWORD baseAddress, T &buffer) {
		return ReadProcessMemory(hProc, (LPCVOID)baseAddress, &buffer, sizeof(T), NULL);
	}
	
	inline DWORD GetProcessID() const { return pe32.th32ProcessID; }
	inline HWND GetWindowHandle() const { return this->hWnd; }
};

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);