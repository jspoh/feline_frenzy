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
	NIKE_ENGINE.init("src/Core/Config.txt", 60, "Welcome To Nikesaurus.");

	//Register Scenes
	NIKE_SCENES_SERVICE->registerScene<Splash::Scene>("SPLASH");
	NIKE_SCENES_SERVICE->registerScene<Menu::Scene>("MENU");

	//NIKE_EVENTS_SERVICE.getService<NIKE::Coordinator::Service>()->

	//Change Scene To Main Menu
	//NIKE_SCENES_SERVICE->queueSceneEvent(NIKE::Scenes::SceneEvent(NIKE::Scenes::Actions::CHANGE, "MENU"));

	//Run Engine
	NIKE_ENGINE.run();

	return 0;
}
