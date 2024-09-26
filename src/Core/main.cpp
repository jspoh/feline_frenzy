/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng (jingseng.poh@digipen.edu)
 * \date   11 September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Scenes/SplashScene.h"

// debug stuff
bool DEBUG = false;
NullStream nullstream;

int main() {

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register scene ( First scene registered will be default starting scene )
	NIKEEngine.registerScenes<Splash::Scene>("SPLASH");
	NIKEEngine.registerScenes<Menu::Scene>("MENU");

	// Add Audio System for testing
	AudioSystem::AudioSystem::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<AudioSystem::AudioSystem>(&AudioSystem::AudioSystem::getInstance(), [](System::Base*) {}), "Audio");

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

