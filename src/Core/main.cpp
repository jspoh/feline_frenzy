/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng (jingseng.poh@digipen.edu)
 * \date   11 September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Engine.h"
#include "InputSystem.h"
#include "AudioSystem.h"


// debug stuff
bool DEBUG = false;
NullStream nullstream;

int main() {

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt");

	//Add Input System
	Input::Manager::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<Input::Manager>(&Input::Manager::getInstance(), [](System::Base*) {}), "Input");

	// Add Audio System for testing
	AudioSystem::AudioSystem::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<AudioSystem::AudioSystem>(&AudioSystem::AudioSystem::getInstance(), [](System::Base*) {}), "Audio");

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

