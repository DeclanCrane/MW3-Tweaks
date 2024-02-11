#include <Windows.h>
#include <iostream>
#include <stdlib.h>

#include "XProc.h"

int main(int n, char* args[]) {
	float desiredFov = 90.f;
	float desiredFovScale = 1.f;
	//int desiredFPS = 144;

	DWORD pID = 0;
	HANDLE hProcess = nullptr;

	// Offsets
	DWORD oFOV = 0x0A76130;
	DWORD oFOVScale = 0x0A7601C;
	//DWORD oMaxFPS = 0x176B540;
	//DWORD oPaused = 0x1769F34;
	DWORD oServerRunning = 0x1769F50;

	// Addresses
	int aFOV;
	int aFOVScale;
	//int aFPS;
	int aServerRunning;

	int bInGame = 0;
	float FOV = 0.f;
	//bool bWritten = false;

	if (n > 1 && n == 3) {
		std::cout << "Applying args\n";
		desiredFov = atof(args[1]);
		desiredFovScale = atof(args[2]);
		//desiredFPS = atof(args[3]);
	}

	// Get a handle to the game
	while (!pID && !hProcess) {
		pID = GetProcID(L"iw5sp.exe");

		if (pID) {
			hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pID);

			if (hProcess == NULL)
				std::cout << "Open process error...\n";
			else if (hProcess)
				break;
		}

		std::cout << "Waiting for game...\n";
		Sleep(5000);
	}

	// Get the addresses of each offset
	ReadProcessMemory(hProcess, (LPCVOID)oFOV, &aFOV, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oFOVScale, &aFOVScale, sizeof(int), NULL);
	//ReadProcessMemory(hProcess, (LPCVOID)oMaxFPS, &aFPS, sizeof(int), NULL);
	//ReadProcessMemory(hProcess, (LPCVOID)oPaused, &aPaused, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)oServerRunning, &aServerRunning, sizeof(int), NULL);

	/*
		Call of Duty games reset protected variables everytime the player loads a new level.
		This is a simple way to ensure the patched values stay applied when switching
		levels, without constantly writing memory to the game.
	*/
	while (true) {
		ReadProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &FOV, sizeof(FOV), NULL);

		if (GetAsyncKeyState(VK_DOWN) & 0x1) {
			ReadProcessMemory(hProcess, (LPVOID)(aServerRunning + 0xC), &bInGame, sizeof(bInGame), NULL);
			std::cout << "In Game?: " << bInGame << "\n";
		}

		if (FOV < desiredFov) {
			WriteProcessMemory(hProcess, (LPVOID)(aFOV + 0xC), &desiredFov, sizeof(desiredFov), NULL);
			WriteProcessMemory(hProcess, (LPVOID)(aFOVScale + 0xC), &desiredFovScale, sizeof(desiredFovScale), NULL);

			std::cout << "Applied patch!\n";
		}

		Sleep(100);
	}

	return 0;
}