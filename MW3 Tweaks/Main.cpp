#include <Windows.h>
#include <iostream>
#include <stdlib.h>

#include "XProc.h"

int main(int n, char* args[]) {
	float desiredFov = 90.f;
	float desiredFovScale = 1.f;
	int desiredFPS = 144;

	DWORD pID = 0;
	HANDLE hProcess = nullptr;

	// Offsets
	constexpr DWORD oFOV = 0x0A76130;
	constexpr DWORD oFOVScale = 0x0A7601C;
	constexpr DWORD oServerRunning = 0x1769F50;
	constexpr DWORD oMaxFPS = 0x176B540;
	//DWORD oPaused = 0x1769F34;

	// Addresses
	DWORD aFOV = 0;
	DWORD aFOVScale = 0;
	DWORD aServerRunning = 0;
	DWORD aMaxFPS = 0;

	// Values
	int bInGame = 0;
	float FOV = 0.f;

	if (n > 1 && n == 3) {
		desiredFov = atof(args[1]);
		desiredFovScale = atof(args[2]);
		desiredFPS = atof(args[3]);
	}

	// Get a handle to the game
	while (!hProcess) {
		pID = GetProcID(L"iw5sp.exe");

		if (pID) {
			hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pID);
			if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
				std::cout << "Cannot get handle to game...\n";
			}
		}

		std::cout << "Waiting for game...\n";
		Sleep(5000);
	}

	std::cout << "Found game...\n";
	std::cout << "\a";

	/* 
		Game variables are not initialized until loading into a level.
	*/
	while (!aFOV || !aFOVScale || !aServerRunning) {
		// Get the addresses of each offset
		ReadProcessMemory(hProcess, (LPCVOID)oFOV, &aFOV, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)oFOVScale, &aFOVScale, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)oServerRunning, &aServerRunning, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)oMaxFPS, &aMaxFPS, sizeof(int), NULL);

		Sleep(100);
	}

	/*
		Call of Duty games reset protected variables everytime the player loads a new level.
		This is a simple way to ensure the patched values stay applied when switching
		levels, without constantly writing memory to the game.
	*/
	while (true) {
		ReadProcessMemory(hProcess, (LPVOID)(aServerRunning + 0xC), &bInGame, sizeof(bInGame), NULL);

		/* Only apply patches if the player is in-game. Singleplayer, co-op, etc. */
		if (bInGame) {
			ReadProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &FOV, sizeof(FOV), NULL);

			if (FOV != desiredFov) {
				WriteProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &desiredFov, sizeof(desiredFov), NULL);
				WriteProcessMemory(hProcess, (LPVOID)(aFOVScale + 0xC), &desiredFovScale, sizeof(desiredFovScale), NULL);
				WriteProcessMemory(hProcess, (LPVOID)(aMaxFPS + 0xC), &desiredFPS, sizeof(desiredFPS), NULL);

				std::cout << "Applying patch...\n";
			}
		}
		Sleep(500);
	}

	CloseHandle(hProcess);

	return 0;
}