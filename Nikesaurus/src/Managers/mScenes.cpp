/*****************************************************************//**
 * \file   StateManager.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mScenes.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cInput.h"
#include "../headers/Components/cScene.h"

namespace NIKESAURUS {
	void Scenes::Manager::initScene(std::string scene_id) {
		curr_scene = scenes.at(scene_id);
		prev_scene = curr_scene;
		curr_scene->load();
		curr_scene->init();
	}

	void Scenes::Manager::changeScene(std::string scene_id) {

		//Change scene only if its not the same scene
		if (scenes.at(scene_id) == curr_scene) {
			return;
		}

		//Free & Unload Old Scene
		if (curr_scene) {
			curr_scene->exit();
			curr_scene->unload();
		}

		//Change New SCene
		prev_scene = curr_scene;
		curr_scene = scenes.at(scene_id);

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

	std::string Scenes::Manager::getCurrSceneID() const {

		//Return curr scene id
		for (auto const& scene : scenes) {
			if (scene.second == curr_scene) {
				return scene.first;
			}
		}

		return "";
	}

	void Scenes::Manager::queueSceneEvent(Scenes::SceneEvent new_event) {
		event_queue = std::make_shared<Scenes::SceneEvent>(new_event);
	}

	void Scenes::Manager::update() {
		if (event_queue) {
			switch (event_queue->scene_action) {
			case Scenes::Actions::CHANGE:
				changeScene(event_queue->next_scene_id);
				break;
			case Scenes::Actions::PREVIOUS:
				previousScene();
				break;
			case Scenes::Actions::RESTART:
				restartScene();
				break;
			default:
				break;
			}

			//Reset queue
			event_queue.reset();
		}
	}
}
