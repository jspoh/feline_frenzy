/*****************************************************************//**
 * \file   StateManager.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysScene.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cInput.h"
#include "../headers/Components/cScene.h"

void Scenes::Manager::initScene(std::string scene_id) {
	curr_scene = states.at(scene_id);
	prev_scene = curr_scene;
	curr_scene->load();
	curr_scene->init();
}

void Scenes::Manager::changeScene(std::string scene_id) {

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
}

bool Scenes::Manager::update() {
	for (auto& entity : entities) {

		//Looking for key change event
		if (NIKEEngine.checkEntityComponent<Scenes::ChangeScene>(entity)) {
			auto& e_event = NIKEEngine.getEntityComponent<Scenes::ChangeScene>(entity);

			if (e_event.mouse_type == -1) {
				//Assert if key not present
				assert(NIKEEngine.checkEntityComponent<Input::Key>(entity) && "Key change scene entity created without key object.");
				auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);

				//Switch cases to perform scene management actions
				if (e_key.b_output && e_key.key_type == e_event.key_type) {
					switch (e_event.scene_action) {
					case Scenes::Actions::CHANGE:
						changeScene(e_event.next_scene_id);
						return true;
					case Scenes::Actions::RESTART:
						restartScene();
						return false;
					case Scenes::Actions::PREVIOUS:
						previousScene();
						return true;
					default:
						break;
					}
				}
			}
			if (e_event.key_type == -1) {
				//Assert if mouse not present
				assert(NIKEEngine.checkEntityComponent<Input::Mouse>(entity) && "Mouse change scene entity created without mouse object.");
				auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

				//Switch cases to perform scene management actions
				if (e_mouse.b_output && e_mouse.button_type == e_event.mouse_type) {
					switch (e_event.scene_action) {
					case Scenes::Actions::CHANGE:
						changeScene(e_event.next_scene_id);
						return true;
					case Scenes::Actions::RESTART:
						restartScene();
						return false;
					case Scenes::Actions::PREVIOUS:
						previousScene();
						return true;
					default:
						break;
					}
				}
			}
		}
	}

	return false;
}
