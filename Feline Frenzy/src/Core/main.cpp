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
	
	//Init Engine
	NIKEEngine.init("src/Core/Config.txt", 60, "Welcome To Nikesaurus.");

	//Logging System Test
	NIKEEngine.getService<NIKESAURUS::Logging::Service>()->Init();
	NIKEEngine.getService<NIKESAURUS::Logging::Service>()->GetCoreLogger()->warn("Error Logged");
	NIKEEngine.getService<NIKESAURUS::Logging::Service>()->GetClientLogger()->info("Message");

	//Register Scenes
	NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->registerScene<Splash::Scene>("SPLASH");
	NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->registerScene<Menu::Scene>("MENU");

	//Change Scene To Main Menu
	//NIKEEngine.getService<NIKESAURUS::Scenes::Service>()->queueSceneEvent(NIKESAURUS::Scenes::SceneEvent(NIKESAURUS::Scenes::Actions::CHANGE, "MENU"));

	//Run Engine
	NIKEEngine.run();

	return 0;
}
