#include "Core/pch.h"

//Splash Scene
#include "Scenes/SplashScene.h"

// debug stuff
bool DEBUG = true;
NullStream nullstream;

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
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register Scenes
	NIKEEngine.getService<NIKESAURUS::Scenes::Manager>()->registerScene<Splash::Scene>("SPLASH");

	//Run Engine
	NIKEEngine.run();

	return 0;
}
