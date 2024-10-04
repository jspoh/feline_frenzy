/*****************************************************************//**
 * \file   PerformanceScene.cpp
 * \brief
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Scenes/PerformanceScene.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cTransform.h"
#include "../headers/Core/Engine.h"
#include "../headers/Systems/GameLogic/sysObjectSpawner.h"
#include "../headers/Systems/Physics/sysPhysics.h"

void Performance::Scene::load() {
	
}

void Performance::Scene::init() {
	glClearColor(1, 1, 1, 1);

	NIKEEngine.accessSystem<Render::Manager>()->debug_mode = false
		;
	Entity::Type spawner = NIKEEngine.createEntity();
	NIKEEngine.addEntityComponentObj<ObjectSpawner::Spawn>(spawner, ObjectSpawner::Spawn(250, GLFW_MOUSE_BUTTON_LEFT, -1));
	NIKEEngine.addEntityComponentObj<Input::Mouse>(spawner, {Input::TriggerMode::TRIGGERED});

	glfwSwapInterval(0);
}

void Performance::Scene::exit() {

}

void Performance::Scene::unload() {

}
