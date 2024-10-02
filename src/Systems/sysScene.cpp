/*****************************************************************//**
 * \file   StateManager.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysScene.h"
#include "../headers/Scenes/MenuScene.h"
#include "../headers/Scenes/SplashScene.h"

void Scenes::Manager::initScene(std::string const& scene_id) {
	curr_scene = states.at(scene_id);
	prev_scene = curr_scene;
	curr_scene->load();
	curr_scene->init();
}

void Scenes::Manager::changeScene(std::string const& scene_id) {
	//Change scene only if its not the same scene
	if (states.at(scene_id) == curr_scene) {
		return;
	}

	//Free & Unload Old Scene
	if (curr_scene) {
		curr_scene->exit();
		curr_scene->unload();
	}

	//Change New SCene
	prev_scene = curr_scene;
	curr_scene = states.at(scene_id);

	//Load & Init New Scene
	if (curr_scene) {
		curr_scene->load();
		curr_scene->init();
	}
}

void Scenes::Manager::restartScene() {
	if (curr_scene) {
		curr_scene->exit();
		curr_scene->init();
	}
}

void Scenes::Manager::previousScene() {
	//If prev scene is same as curr scene, restart curr scene
	if (prev_scene == curr_scene) {
		restartScene();
		return;
	}

	//Free & Unload Old State
	if (curr_scene) {
		curr_scene->exit();
		curr_scene->unload();
	}

	//Change New State
	std::swap(prev_scene, curr_scene);

	//Load & Init New State
	if (curr_scene) {
		curr_scene->load();
		curr_scene->init();
	}
}

void Scenes::Manager::init() {
	//Register scene ( First scene registered will be default starting scene )
	registerScenes<Splash::Scene>("SPLASH");
	registerScenes<Menu::Scene>("MENU");
}

void Scenes::Manager::update() {
	//Switch cases to perform scene management actions
	switch (new_scene.scene_action) {
	case Scenes::Actions::CHANGE:
		changeScene(new_scene.next_scene_id);
		break;
	case Scenes::Actions::RESTART:
		restartScene();
		break;
	case Scenes::Actions::PREVIOUS:
		previousScene();
		break;
	default:
		break;
	}
}

void Scenes::Manager::executeEvent(std::shared_ptr<Events::IEvent> event) {
	
	new_scene = *std::dynamic_pointer_cast<Scenes::ChangeSceneEvent>(event);
}
