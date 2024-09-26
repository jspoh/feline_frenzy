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
#include "PhysicsSystem.h"
#include "CollisionSystem.h"

// debug stuff
bool DEBUG = false;
NullStream nullstream;

int main() {

	//Initialize Engine With Config File
	NIKEEngine.init("src/Core/Config.txt", 60);

	//Register scene ( First scene registered will be default starting scene )
	NIKEEngine.registerScenes<Splash::Scene>("SPLASH");
	NIKEEngine.registerScenes<Menu::Scene>("MENU");

	//Add Physics System
	Physics::Manager::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<Physics::Manager>(&Physics::Manager::getInstance(), [](System::Base*) {}), "Physics");
	
	// Add Collision System
	Collision::Manager::getInstance().init();
	NIKEEngine.addSystem(std::shared_ptr<Collision::Manager>(&Collision::Manager::getInstance(), [](System::Base*) {}), "Collision");

	//Engine Game Loop
	NIKEEngine.run();

	return 0;
}

