/*****************************************************************//**
 * \file   StateService.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sScenes.h"
#include "Core/Engine.h"

namespace NIKESAURUS {
	void Scenes::Service::initScene(std::string scene_id) {
		curr_scene = scenes.at(scene_id);
		prev_scene = curr_scene;
		curr_scene->load();
		curr_scene->init();
	}

	void Scenes::Service::changeScene(std::string scene_id) {

		//Check if scene has been registered
		if (scenes.find(scene_id) == scenes.end()) {
			throw std::runtime_error("Scene Not Registered Yet.");
		}

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

	void Scenes::Service::restartScene() {
		if (curr_scene) {
			curr_scene->exit();
			curr_scene->init();
		}
	}

	void Scenes::Service::previousScene() {
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

	std::string Scenes::Service::getCurrSceneID() const {

		//Return curr scene id
		for (auto const& scene : scenes) {
			if (scene.second == curr_scene) {
				return scene.first;
			}
		}

		return "";
	}

	void Scenes::Service::queueSceneEvent(Scenes::SceneEvent&& new_event) {
		if(event_queue == nullptr)
		event_queue = std::make_shared<Scenes::SceneEvent>(std::move(new_event));
	}

	void Scenes::Service::update() {
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
			case Scenes::Actions::CLOSE:
				NIKEEngine.getService<Windows::Service>()->getWindow()->terminate();
				break;
			default:
				break;
			}

			//Reset queue
			event_queue.reset();
		}
	}
}
