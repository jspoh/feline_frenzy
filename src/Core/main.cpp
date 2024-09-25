/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng (jingseng.poh@digipen.edu)
 * \date   11 September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/sysInput.h"

// debug stuff
bool DEBUG = false;
NullStream nullstream;

int main() {

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register input component
	NIKEEngine.registerComponent<Input::Mouse>();
	NIKEEngine.registerComponent<Input::Key>();

	//Add Input Singleton System
	NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());
	NIKEEngine.accessSystem<Input::Manager>()->setComponentsLinked(false);

	//Add component types to system
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Mouse>());
	NIKEEngine.addSystemComponentType<Input::Manager>(NIKEEngine.getComponentType<Input::Key>());

	//Create entity with mouse component
	Entity::Type e = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Mouse>(e, Input::Mouse());
	NIKEEngine.addEntityComponentObj<Input::Key>(e, Input::Key());

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

