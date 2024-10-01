/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng (jingseng.poh@digipen.edu)
 * \date   11 September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/sysScene.h"

// debug stuff
bool DEBUG = true;
NullStream nullstream;

int main() {

	//// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register scenes manager
	NIKEEngine.registerSystem<Scenes::Manager>(Scenes::Manager::getInstance());
	NIKEEngine.accessEvents()->addEventListeners<Scenes::ChangeSceneEvent>(NIKEEngine.accessSystem<Scenes::Manager>());

	// Transform
	//NIKEEngine.registerComponent<Transform::Transform>();
	//NIKEEngine.registerComponent<Transform::Velocity>();
	//NIKEEngine.registerComponent<Move::Move>();

	// Add Input System
	//NIKEEngine.registerSystem<Input::Manager>(Input::Manager::getInstance());

	// Add Physics System
	//NIKEEngine.registerSystem<Physics::Manager>(Physics::Manager::getInstance());

	// Add Collision System
	//NIKEEngine.registerSystem<Collision::Manager>(Collision::Manager::getInstance());

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

