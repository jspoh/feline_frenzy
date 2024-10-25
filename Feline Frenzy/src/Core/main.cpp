/*****************************************************************//**
 * \file   main.cpp
 * \brief  Main entry point for Demo
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/pch.h"

//Splash Scene
#include "Scenes/SplashScene.h"
#include "Scenes/MainMenu.h"

int WINAPI WinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPSTR lpCmdLine,
	[[maybe_unused]] _In_ int nCmdShow
)
{
	//// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	try
	{
		//Init Engine
		NIKEEngine.init("src/Core/Config.txt", 60, "Welcome To Nikesaurus.");

		//Register Scenes
		NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->registerScene<Splash::Scene>("SPLASH");
		NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->registerScene<Menu::Scene>("MENU");

		//NIKEEngine.getService<NIKESAURUS::Coordinator::Service>()->

		//Change Scene To Main Menu
		//NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->queueSceneEvent(NIKESAURUS::Scenes::SceneEvent(NIKESAURUS::Scenes::Actions::CHANGE, "MENU"));

		//Run Engine
		NIKEEngine.run();

		return 0;
	}
	catch (const std::exception& e)
	{
		std::string message = std::string(e.what()) + " : the crash has been logged to 'logs/crash-log.txt'"; // Crash Message

		NIKESAURUS::Log::GetCrashFileLogger()->flush(); // Flush output

		MessageBoxA(nullptr, message.c_str(), "Crash Occured", MB_ICONERROR | MB_OK); // Message box popup

		return -1; 
	}
}
