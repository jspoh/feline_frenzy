#include "../headers/Core/pch.h"
#include <Nikesaurus.h>

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
	NIKESAURUS::NIKEEngine.init("src/Core/Config.txt", 60);

	//Run Engine
	NIKESAURUS::NIKEEngine.run();

	return 0;
}
