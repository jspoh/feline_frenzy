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

namespace NIKE {
	/*****************************************************************//**
	* Layer
	*********************************************************************/
	unsigned int Scenes::Layer::getLayerID() const {
		return id;
	}

	void Scenes::Layer::setLayerState(bool state) {
		b_state = state;
	}

	bool Scenes::Layer::getLayerState() const {
		return b_state;
	}

	void Scenes::Layer::setLayerMask(unsigned int mask_id, bool state) {
		mask.set(mask_id, state);
	}

	std::bitset<64> Scenes::Layer::getLayerMask() const {
		return mask;
	}

	nlohmann::json Scenes::Layer::serialize() const {
		return	{
				{"ID", id},
				{"Mask", mask.to_ulong()},
				{"B_State", b_state}
				};
	}

	void Scenes::Layer::deserialize(nlohmann::json const& data) {
		id = data.at("ID").get<unsigned int>();
		mask = LayerMask(data.at("Mask").get<unsigned long>());
		b_state = data.at("B_State").get<bool>();
	}

	/*****************************************************************//**
	* Scene Interface
	*********************************************************************/
	void Scenes::IScene::setScnPath(std::string const& path) {
		scn_file_path = path;
	}

	std::string Scenes::IScene::getScnPath() const {
		return scn_file_path;
	}

	std::shared_ptr<Scenes::Layer> Scenes::IScene::createLayer(int index) {
		std::shared_ptr<Layer> layer = std::make_shared<Layer>();
		//Insert layers at back
		if (index >= 0) {
			layers.emplace(layers.begin() + index, layer);
		}
		else {
			layers.push_back(layer);
			index = static_cast<int>(layers.size()) - 1;
		}

		layer->id = index;
		layer->mask.set(layer->id, true);

		return layer;
	}

	std::shared_ptr<Scenes::Layer> Scenes::IScene::getLayer(unsigned int layer_id) {
		//Check if layer has been added
		if (layer_id >= static_cast<unsigned int>(layers.size())) {
			throw std::runtime_error("Layer has not been registered.");
		}

		return layers.at(layer_id);
	}

	void Scenes::IScene::removeLayer(unsigned int layer_id) {
		//Check if layer has been added
		if (layer_id >= static_cast<unsigned int>(layers.size())) {
			throw std::runtime_error("Layer has not been registered.");
		}

		//Set all of removed layer bit to false
		for (auto& layer : layers) {
			//Set bit removed id bit to false
			layer->mask.set(layer_id, false);
		}

		//Erase layer
		layers.erase(layers.begin() + layer_id);

		//Update layers index & mask
		unsigned int index = 0;
		for (auto& layer : layers) {
			//Get new index
			layer->id = index++;

			//Shift all bits down
			for (unsigned int i = layer_id + 1; i < layers.size(); i++) {
				layer->mask[i - 1] = layer->mask[i];
			}

			//Clear the last bit, as it has shifted left by one position
			layer->mask[layers.size() - 1] = false;
		}
	}

	bool Scenes::IScene::checkLayer(unsigned int layer_id) {
		return layer_id < static_cast<unsigned int>(layers.size());
	}

	unsigned int Scenes::IScene::getLayerCount() const {
		return static_cast<unsigned int>(layers.size());
	}

	std::vector<std::shared_ptr<Scenes::Layer>>& Scenes::IScene::getLayers() {
		return layers;
	}

	/*****************************************************************//**
	* Scene maanger
	*********************************************************************/
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

	std::shared_ptr<Scenes::IScene> Scenes::Service::getCurrScene() {
		return curr_scene;
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
		if (!NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {
			curr_scene->exit();
			curr_scene->unload();
		}

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
				NIKE_WINDOWS_SERVICE->getWindow()->terminate();
				break;
			default:
				break;
			}

			//Reset queue
			event_queue.reset();
		}
	}
}
