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

	//Register Scenes
	NIKEEngine.getService<NIKESAURUS::Scenes::Manager>()->registerScene<Splash::Scene>("SPLASH");
	NIKEEngine.getService<NIKESAURUS::Scenes::Manager>()->registerScene<Menu::Scene>("MENU");

	NIKEEngine.getService<NIKESAURUS::Scenes::Manager>()->queueSceneEvent(NIKESAURUS::Scenes::SceneEvent(NIKESAURUS::Scenes::Actions::CHANGE, "MENU"));

	//Run Engine
	NIKEEngine.run();

	return 0;
}
