#include "Process.h"

#include <iostream>

DWORD GetProcessID(const char* procName)
{
    HANDLE hSnap;
    PROCESSENTRY32 pe32{};

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Create a snapshot of active processes
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    // Loop through processes until target is found
    do {
        if (!_stricmp(pe32.szExeFile, procName)) {
            return pe32.th32ProcessID;
        }
    } while (Process32Next(hSnap, &pe32));

    return 0;
}
