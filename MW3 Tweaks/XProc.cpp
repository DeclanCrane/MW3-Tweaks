#include "XProc.h"

#include <iostream>

XProcess::XProcess(std::wstring procName) : procName(L""), hWnd(NULL), hProc(NULL), pe32({})
{
    this->procName = procName;
    pe32.dwSize = sizeof(PROCESSENTRY32);
}

XProcess::~XProcess()
{
    if (hProc)
        CloseHandle(hProc);
}

bool XProcess::WaitForProcess(double timeoutSec)
{
    clock_t c = clock();

    do {
        if (GetProcessInfo()) {
            hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        }

        // Check time
        if (timeoutSec && double(clock() - c) / CLOCKS_PER_SEC >= timeoutSec) {
            return false;
        }

    } while (!hProc);

    return true;
}

bool XProcess::GetModuleList(std::vector<MODULEENTRY32>& modules)
{
    MODULEENTRY32 me32{};
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!pe32.th32ProcessID) {
        GetProcessInfo();
    }

    HANDLE hProc = nullptr;
    hProc = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->pe32.th32ProcessID);

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

bool XProcess::IsModulePresent(const std::wstring& moduleName)
{
    std::vector<MODULEENTRY32> modules;

    if (GetModuleList(modules)) {
        for (int i = 0; i < modules.size(); i++) {
            if (!_wcsicmp(modules.at(i).szModule, moduleName.c_str()))
                return true;
        }
    }
    return false;
}

bool XProcess::GetProcessInfo()
{
    HANDLE hSnap = nullptr;

    // Create a snapshot of all system processes
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hSnap == INVALID_HANDLE_VALUE)
        return false;

    do {
        if (!_wcsicmp(procName.c_str(), pe32.szExeFile)) {
            CloseHandle(hSnap);
            return true;
        }
    } while (Process32Next(hSnap, &pe32));

    CloseHandle(hSnap);
    return false;
}

bool XProcess::GetProcessWindow() {
    WindowData wData{};
    
    // Get the process info if not already
    if (!this->pe32.th32ProcessID)
        if (!GetProcessInfo())
            return false;

    wData.pID = this->pe32.th32ProcessID;

    EnumWindows(EnumWindowsCallback, (LPARAM)&wData);

    if (wData.hWnd) {
        this->hWnd = wData.hWnd;
        return true;
    }
    else
        return false;
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