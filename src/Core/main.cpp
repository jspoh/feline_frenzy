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
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register input component
	NIKEEngine.registerComponent<Input::Mouse>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());

	//Set system signature
	Component::Signature sign;
	sign.set(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.setSystemSignature<Input::Manager>(sign);

	//Create entity with mouse component
	Entity::Type e = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponent<Input::Mouse>(e, Input::Mouse());

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

