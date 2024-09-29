/*****************************************************************//**
 * \file   StateManager.cpp
 * \brief  
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mScene.h"

// all possible states for initialization (registration)
#include "../headers/Scenes/MenuScene.h"

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
