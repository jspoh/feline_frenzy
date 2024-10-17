#include "../headers/Core/pch.h"
#include <Nikesaurus.h>

int WINAPI WinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPSTR lpCmdLine,
	[[maybe_unused]] _In_ int nCmdShow
)
{
	NIKE::Application* app = new NIKE::Application();
	app->run();
	delete app;
	return 0;
}
