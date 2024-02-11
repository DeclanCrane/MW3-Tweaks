#include "XProc.h"

#include <iostream>

bool GetProcessInfo(const wchar_t* procName, PROCESSENTRY32* pe32)
{
    HANDLE hProc = nullptr;
    pe32->dwSize = sizeof(PROCESSENTRY32);

    // Create a snapshot of all system processes
    hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hProc == INVALID_HANDLE_VALUE)
        return false;

    do {
        if (!_wcsicmp(procName, pe32->szExeFile)) {
            CloseHandle(hProc);
            return true;
        }
    } while (Process32Next(hProc, pe32));

    CloseHandle(hProc);
    return false;
}

DWORD GetProcID(const wchar_t* procName) {
    HANDLE hProc = nullptr;
    PROCESSENTRY32 pe32{};
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Create a snapshot of all system processes
    hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hProc == INVALID_HANDLE_VALUE)
        return 0;

    do {
        if (!_wcsicmp(procName, pe32.szExeFile)) {
            CloseHandle(hProc);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(hProc, &pe32));

    CloseHandle(hProc);
    return 0;
}

bool GetProcessModules(const DWORD pID, std::vector<MODULEENTRY32>& modules)
{
    MODULEENTRY32 me32{};
    me32.dwSize = sizeof(MODULEENTRY32);

    HANDLE hProc = nullptr;
    hProc = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    
    if (!hProc) {
        return false;
    }

    if (!Module32First(hProc, &me32)) {
        CloseHandle(hProc);
        return false;
    }

    do {
        modules.push_back(me32);
    } while (Module32Next(hProc, &me32));

    CloseHandle(hProc);
    return true;
}

HWND GetProcessWindow(const DWORD pID)
{
    WindowData wData{};
    wData.pID = pID;

    EnumWindows(EnumWindowsCallback, (LPARAM)&wData);
    return wData.hWnd;
}

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
    WindowData& wData = *(WindowData*)lParam;

    DWORD pID = 0;
    GetWindowThreadProcessId(hWnd, &pID);
    if (wData.pID != pID)
        return TRUE;

    wData.hWnd = hWnd;
    return FALSE;
}
