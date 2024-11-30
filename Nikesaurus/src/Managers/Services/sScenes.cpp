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
	* Scene maanger
	*********************************************************************/
	void Scenes::Service::initScene(std::string const& scene_id) {

		//Set curr & prev scene id references
		curr_scene = scene_id;
		prev_scene = curr_scene;

		//Check if scene exists
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			throw std::runtime_error("Error scene file does not exist");
		}

		//Run scene
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	void Scenes::Service::changeScene(std::string const& scene_id) {

		//Change scene only if its not the same scene
		if (scene_id == curr_scene) {
			return;
		}
		
		//Set curr & prev scene id references
		prev_scene = curr_scene;
		curr_scene = scene_id;

		//Clear entities
		NIKE_ECS_MANAGER->destroyAllEntities();

		//Clear UI Entities
		NIKE_UI_SERVICE->destroyAllButtons();

		//Clear layers
		layers.clear();

		//Create the first layer
		createLayer();

		//Check if scene exists
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			throw std::runtime_error("Error scene file does not exist");
		}

		//Run scene
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	void Scenes::Service::restartScene() {
		//Check if scene exists
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			return;
		}

		//Clear entities
		NIKE_ECS_MANAGER->destroyAllEntities();

		//Clear UI Entities
		NIKE_UI_SERVICE->destroyAllButtons();

		//Clear layers
		layers.clear();

		//Create the first layer
		createLayer();

		//ReRun scene
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	void Scenes::Service::previousScene() {
		//If prev scene is same as curr scene, restart curr scene
		if (prev_scene == curr_scene || !NIKE_ASSETS_SERVICE->isAssetRegistered(prev_scene)) {
			restartScene();
			return;
		}

		//Change New State
		std::swap(prev_scene, curr_scene);

		//Clear entities
		NIKE_ECS_MANAGER->destroyAllEntities();

		//Clear UI Entities
		NIKE_UI_SERVICE->destroyAllButtons();

		//Clear layers
		layers.clear();

		//Create the first layer
		createLayer();

		//Check if scene exists
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			throw std::runtime_error("Error scene file does not exist");
		}

		//Run scene
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	std::shared_ptr<Scenes::Layer> Scenes::Service::createLayer(int index) {
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

	std::shared_ptr<Scenes::Layer> Scenes::Service::getLayer(unsigned int layer_id) {
		//Check if layer has been added
		if (layer_id >= static_cast<unsigned int>(layers.size())) {
			throw std::runtime_error("Layer has not been registered.");
		}

		return layers.at(layer_id);
	}

	void Scenes::Service::removeLayer(unsigned int layer_id) {
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

	bool Scenes::Service::checkLayer(unsigned int layer_id) {
		return layer_id < static_cast<unsigned int>(layers.size());
	}

	unsigned int Scenes::Service::getLayerCount() const {
		return static_cast<unsigned int>(layers.size());
	}

	std::vector<std::shared_ptr<Scenes::Layer>>& Scenes::Service::getLayers() {
		return layers;
	}

	void Scenes::Service::queueSceneEvent(Scenes::SceneEvent const& new_event) {
		if (!event_queue) {
			event_queue = std::make_shared<Scenes::SceneEvent>(new_event);
		}
		else {
			NIKEE_CORE_INFO("Multiple new scene event. First scene event will be used");
		}
	}

	void Scenes::Service::setCurrSceneID(std::string const& new_scene_id) {
		curr_scene = new_scene_id;
	}

	std::string Scenes::Service::getCurrSceneID() const {
		return curr_scene;
	}

	std::string Scenes::Service::getPrevSceneID() const {
		return prev_scene;
	}

	void Scenes::Service::init() {
		//Create the first layer
		createLayer();
	}

	void Scenes::Service::update() {
		if (!NIKE_WINDOWS_SERVICE->getWindow()->windowState()) {
		}

		//Check if curr scene path is still active
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			curr_scene.clear();
		}

		//Execute new scene queue
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
