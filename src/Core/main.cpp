/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   11 September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Managers/mScenes.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Scenes/SplashScene.h"
#include "../headers/Scenes/PerformanceScene.h"

// debug stuff
bool DEBUG = true;
NullStream nullstream;

//Create console
void createConsole() {
	// Create a new console window
	AllocConsole();

	FILE* fp;

	// Redirect cin and cout to the new console
	// Redirect stdout
	freopen_s(&fp, "CONOUT$", "w", stdout);
	// Redirect stderr
	freopen_s(&fp, "CONOUT$", "w", stderr);
	// Redirect stdin
	freopen_s(&fp, "CONIN$", "r", stdin);
}

int WINAPI WinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPSTR lpCmdLine,
	[[maybe_unused]] _In_ int nCmdShow
)
{
	// Function call to create console for debug
	#ifndef NDEBUG
	createConsole();
	#endif
	
	//// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register scenes manager
	NIKEEngine.accessScenes()->registerScenes<Splash::Scene>("SPLASH");
	NIKEEngine.accessScenes()->registerScenes<Menu::Scene>("MENU");
	NIKEEngine.accessScenes()->registerScenes<Performance::Scene>("PERFORMANCE");

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

