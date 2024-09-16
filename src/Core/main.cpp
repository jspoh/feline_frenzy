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


// debug stuff
bool DEBUG = false;
NullStream nullstream;

int main() {

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt");

	//Add Input System
	Input::Manager::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<Input::Manager>(&Input::Manager::getInstance(), [](System::Base*) {}), "Input");

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

