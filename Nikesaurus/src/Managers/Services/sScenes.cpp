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
	unsigned int Scenes::Layer::getLayerIndex() const {
		return index;
	}

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
				{"Index", index},
				{"Mask", mask.to_ulong()},
				{"B_State", b_state}
				};
	}

	void Scenes::Layer::deserialize(nlohmann::json const& data) {
		id = data.at("ID").get<unsigned int>();
		index = data.at("Index").get<unsigned int>();
		mask = LayerMask(data.at("Mask").get<unsigned long>());
		b_state = data.at("B_State").get<bool>();
	}

	/*****************************************************************//**
	* Scene Interface
	*********************************************************************/
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

		layer->id = layer_count++;
		layer->index = index;
		layer->mask.set(layer->id, true);
		layers_map.emplace(std::piecewise_construct, std::forward_as_tuple(layer->id), std::forward_as_tuple(layer));

		return layer;
	}

	std::shared_ptr<Scenes::Layer> Scenes::IScene::getLayer(unsigned int mask_id) {
		//Check if layer has been added
		auto it = layers_map.find(mask_id);
		if (it == layers_map.end()) {
			throw std::runtime_error("Layer has not been registered.");
		}

		return it->second;
	}

	void Scenes::IScene::removeLayer(unsigned int mask_id) {
		//Check if layer has been added
		auto it = layers_map.find(mask_id);
		if (it == layers_map.end()) {
			throw std::runtime_error("Layer has not been registered.");
		}

		layers.erase(layers.begin() + it->second->getLayerIndex());

		//Update layers index & mask
		unsigned int index = 0;
		for (auto& layer : layers) {
			layer->index = index++;
			layer->mask.set(it->second->id, false);
		}

		layers_map.erase(it);
	}

	bool Scenes::IScene::checkLayer(unsigned int mask_id) {
		return layers_map.find(mask_id) != layers_map.end();
	}

	unsigned int Scenes::IScene::getLayerCount() const {
		return layer_count;
	}

	std::vector<std::shared_ptr<Scenes::Layer>> const& Scenes::IScene::getLayers() const {
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
