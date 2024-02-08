#include <Windows.h>
#include <iostream>
#include <stdlib.h>

#include "Process.h"

int main(int n, char* args[]) {
	float desiredFov = 90.f;
	float desiredFovScale = 1.f;
	int desiredFPS = 144;

	DWORD pID;
	HANDLE hProcess;

	// Offsets
	DWORD oFOV = 0x0A76130;
	DWORD oFOVScale = 0x0A7601C;
	DWORD oMaxFPS = 0x176B540;
	DWORD oPaused = 0x1769F34;
	DWORD oServerRunning = 0x1769F50;

	// Addresses
	int aFOV;
	int aFOVScale;
	int aFPS;
	int aPaused;
	int aServerRunning;

	int bInGame = 0;
	bool bWritten = false;

	if (n > 1 && n == 4) {
		std::cout << "Applying args\n";
		desiredFov = atof(args[1]);
		desiredFovScale = atof(args[2]);
		desiredFPS = atof(args[3]);
	}

	// Get a handle to the game
	pID = GetProcessID("iw5sp.exe");
	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pID);
	if (!hProcess)
		return 0;

	// Get the addresses of each offset
	ReadProcessMemory(hProcess, (LPCVOID)oFOV, &aFOV, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oFOVScale, &aFOVScale, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oMaxFPS, &aFPS, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oPaused, &aPaused, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oServerRunning, &aServerRunning, sizeof(int), NULL);

	/*
		Call of Duty games reset protected variables everytime the player loads a new level.
		This is a simple way to ensure the patched values stay applied when switching
		levels, without constantly writing memory to the game.
	*/
	while (true) {
		ReadProcessMemory(hProcess, (LPVOID)(aServerRunning + 0xC), &bInGame, sizeof(bInGame), NULL);

		// If they player is in-game, and the patch hasn't been applied.
		if (!bWritten && bInGame) {
			WriteProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &desiredFov, sizeof(desiredFov), NULL);
			WriteProcessMemory(hProcess, (LPVOID)(aFOVScale + 0xC), &desiredFovScale, sizeof(desiredFovScale), NULL);
			WriteProcessMemory(hProcess, (LPVOID)(aFPS + 0xC), &desiredFPS, sizeof(desiredFPS), NULL);
			bWritten = true;

			std::cout << "Patch applied.\n";
		}

		/*
			If the patch has been written to the game,
			but the player is loading a new level.
		*/
		if (bWritten && !bInGame) {
			bool bReset = false;
			float flFOV = 0.f;

			bWritten = false;
			std::cout << "Reset\n";

			/*
				Hacky, but works.
				Continuously check to see if the field of view value is 65 ( default ).
				If it is, that means the protected variables have been reset,
				and we're safe to reapply the patch.
			*/

			while (!bReset) {
				std::cout << "Reapplying patch...\n";
				ReadProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &flFOV, sizeof(flFOV), NULL);
				if (flFOV == 65.f)
					bReset = true;

				Sleep(500);
			}
		}

		Sleep(100);
	}

	return 0;
}