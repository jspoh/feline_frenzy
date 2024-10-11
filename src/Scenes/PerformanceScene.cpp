/*****************************************************************//**
 * \file   PerformanceScene.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/PerformanceScene.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/GameLogic/sysObjectSpawner.h"
#include "../headers/Systems/Physics/sysPhysics.h"

void Performance::Scene::load() {
	// Regiser object spawner component
	NIKEEngine.registerComponent<ObjectSpawner::Spawn>();

	//Register spawner system before physics
	NIKEEngine.registerSystem<ObjectSpawner::Manager>(nullptr, NIKEEngine.getSystemIndex<Physics::Manager>());
	NIKEEngine.addSystemComponentType<ObjectSpawner::Manager>(NIKEEngine.getComponentType<ObjectSpawner::Spawn>());
}

void Performance::Scene::init() {
	glClearColor(1, 1, 1, 1);

	//Increase target FPS for stress test
	NIKEEngine.accessWindow()->setTargetFPS(150);

	//Disable vsync
	glfwSwapInterval(0);

	//Create object spawner
	Entity::Type spawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<ObjectSpawner::Spawn>(spawner, ObjectSpawner::Spawn(250, GLFW_MOUSE_BUTTON_LEFT, -1));
	NIKEEngine.addEntityComponentObj<Input::Mouse>(spawner, {Input::TriggerMode::TRIGGERED});

	//Close program and toggle
	Entity::Type key = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<Input::Key>(key, { Input::TriggerMode::TRIGGERED });
	NIKEEngine.addEntityComponentObj<Render::Debug>(key, Render::Debug());
}

void Performance::Scene::exit() {
	//Set FPS back to standard 60
	NIKEEngine.accessWindow()->setTargetFPS(60);

	//Enable vsync
	glfwSwapInterval(1);

	//Remove component and systems that are not in use
	NIKEEngine.removeComponent<ObjectSpawner::Spawn>();
	NIKEEngine.removeSystem<ObjectSpawner::Manager>();
}

void Performance::Scene::unload() {

}
