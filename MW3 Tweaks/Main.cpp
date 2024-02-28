#include <Windows.h>
#include <iostream>
#include <map>

#include "XFiles.h"
#include "XConfig.h"
#include "XProc.h"
#include "CODVar.h"

int main() {
	// Vars
	CODVar<float> fov = { 0x0A76130, 0, 0.f, 65.f };
	CODVar<float> fovScale = { 0x0A7601C, 0, 0.f, 1.f };
	CODVar<int> maxFPS = { 0x176B540, 0, 0, 0 };
	CODVar<int> serverRunning = { 0x1769F50, 0, 0, 0 };

	// Setup config
	XFile config("tweak_config.txt");
	std::map<std::string, std::string> params;

	// If the config file doesn't exist, create it, and write default settings.
	if (!config.Exists() || config.Empty())
	{
		std::cout << "Creating config...\n";
		config.Create();
		config.Write("cg_fov=90\ncom_maxfps=144\ncg_fovScale=1.0");
	}
	std::cout << "Loading config...\n";

	// Read config and parse parameters
	std::vector<std::string> lines;
	config.Read(lines);

	if (!lines.size()) {
		std::cout << "Error loading config...\n";
		return 1;
	}

	// Loading config parameters into a map
	ConvertToMap(lines, params);

	// Apply config settings
	fov.desiredValue = std::stof(params["cg_fov"]);
	fovScale.desiredValue = std::stof(params["cg_fovScale"]);
	maxFPS.desiredValue = std::stoi(params["com_maxfps"]);

	XProcess MW3(L"iw5sp.exe");
	if (!MW3.WaitForProcess(60.f))
		std::cout << "Please start Modern Warfare 3 and try again.\n";

	/* 
		Game variables are not initialized until loading into a level.
	*/
	while (!fov.address || !fovScale.address|| !maxFPS.address) {
		// Get the addresses of each offset
		MW3.ReadMemory(fov.offset, fov.address);
		MW3.ReadMemory(fovScale.offset, fovScale.address);
		MW3.ReadMemory(maxFPS.offset, maxFPS.address);
		MW3.ReadMemory(serverRunning.offset, serverRunning.address);

		Sleep(100);
	}

	/*
		Call of Duty games reset protected variables everytime the player loads a new level.
		This is a simple way to ensure the patched values stay applied when switching
		levels, without constantly writing memory to the game.
	*/
	while (true) {
		MW3.ReadMemory(serverRunning.address + 0xC, serverRunning.value);

		/* Only apply patches if the player is in-game. Singleplayer, co-op, etc. */
		if (serverRunning.value) {
			MW3.ReadMemory(fov.address + 0xC, fov.value);

			if (fov.value != fov.desiredValue) {
				MW3.WriteMemory(fov.address + 0xC, fov.desiredValue);
				MW3.WriteMemory(fovScale.address + 0xC, fovScale.desiredValue);
				MW3.WriteMemory(maxFPS.address + 0xC, maxFPS.desiredValue);
				std::cout << "Applying patch...\n";
			}
		}
		Sleep(500);
	}

	return 0;
}