/*****************************************************************//**
 * \file   sScenes.cpp
 * \brief  
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sScenes.h"
#include "Core/Engine.h"
#include "Systems/sysParticle.h"

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

	std::bitset<Scenes::MAXLAYERS> Scenes::Layer::getLayerMask() const {
		return mask;
	}

	void Scenes::Layer::insertEntity(Entity::Type entity) {
		for (auto const& e : entities) {

			//Check for duplicates
			if (e == entity) {
				return;
			}
		}

		//Insert entity
		entities.push_back(entity);
	}

	void Scenes::Layer::removeEntity(Entity::Type entity) {
		auto it = std::find(entities.begin(), entities.end(), entity);
		if (it != entities.end()) {
			entities.erase(it);
		}
	}

	bool Scenes::Layer::checkEntity(Entity::Type entity) {
		return std::find(entities.begin(), entities.end(), entity) != entities.end();
	}

	void Scenes::Layer::sortEntitiesBasedOnMetaData() {
		if (entities.empty()) {
			return;
		}

		std::sort(entities.begin(), entities.end(), [](Entity::Type a, Entity::Type b) {
			return NIKE_METADATA_SERVICE->getEntityLayerOrder(a) < NIKE_METADATA_SERVICE->getEntityLayerOrder(b);
			});
	}

	void Scenes::Layer::setEntityOrder(Entity::Type entity, size_t order_in_layer) {

		//Check for valid order
		if (entities.empty() || order_in_layer >= entities.size()) {
			return;
		}

		//Get curr index of entity
		size_t current_index = getEntityOrder(entity);

		//Check if entity is already in correct order
		if (current_index == (std::numeric_limits<size_t>::max)() || current_index == order_in_layer) {
			return;
		}

		//Reorder entity
		if (current_index < order_in_layer) {
			std::rotate(entities.begin() + current_index, entities.begin() + current_index + 1, entities.begin() + order_in_layer + 1);
		}
		else {
			std::rotate(entities.begin() + order_in_layer, entities.begin() + current_index, entities.begin() + current_index + 1);
		}

		//Update all entities layer order in metadata
		for (size_t i = 0; i < entities.size(); ++i) {
			NIKE_METADATA_SERVICE->setEntityLayerOrder(entities.at(i), i);
		}
	}

	size_t Scenes::Layer::getEntityOrder(Entity::Type entity) const {
		auto it = std::find(entities.begin(), entities.end(), entity);
		if (it == entities.end()) {
			return (std::numeric_limits<size_t>::max)();
		}
		else {
			return std::distance(entities.begin(), it);
		}
	}

	std::vector<Entity::Type> Scenes::Layer::getEntitites() const {
		return entities;
	}

	size_t Scenes::Layer::getEntitiesSize() const {
		return entities.size();
	}

	nlohmann::json Scenes::Layer::serialize() const {
		return	{
				{"ID", id},
				{"Mask", mask.to_ulong()},
				{"B_State", b_state}
				};
	}

	void Scenes::Layer::deserialize(nlohmann::json const& data) {
		id = data.value("ID", static_cast<unsigned int>(0));
		mask = LayerMask(data.value("Mask", static_cast<unsigned long>(0)));
		b_state = data.value("B_State", true);
	}

	/*****************************************************************//**
	* Scene manager
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

		//Stop all audios
		NIKE_AUDIO_SERVICE->clearAllChannelGroups();

		//Reset Camera
		NIKE_CAMERA_SERVICE->clearCameraEntities();

		//Reset metadata service
		NIKE_METADATA_SERVICE->reset();

		// Reset grid here
		NIKE_MAP_SERVICE->resetGrid();

		//Clear layers
		layers.clear();

		//Create the first layer
		createLayer();

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

		//Stop all audios
		NIKE_AUDIO_SERVICE->clearAllChannelGroups();

		//Reset metadata service
		NIKE_METADATA_SERVICE->reset();

		// Reset grid here
		NIKE_MAP_SERVICE->resetGrid();

		//Reset Camera
		NIKE_CAMERA_SERVICE->clearCameraEntities();

		// Reset particle systems
		NIKE::SysParticle::Manager::getInstance().resetEntityParticleSystems();

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

		//Stop all audios
		NIKE_AUDIO_SERVICE->clearAllChannelGroups();

		//Reset metadata service
		NIKE_METADATA_SERVICE->reset();

		// Reset grid here
		NIKE_MAP_SERVICE->resetGrid();
		
		//Reset Camera
		NIKE_CAMERA_SERVICE->clearCameraEntities();

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

	void Scenes::Service::resetScene() {

		//Clear entities
		NIKE_ECS_MANAGER->destroyAllEntities();

		//Clear UI Entities
		NIKE_UI_SERVICE->destroyAllButtons();

		//Stop all audios
		NIKE_AUDIO_SERVICE->clearAllChannelGroups();

		//Reset metadata service
		NIKE_METADATA_SERVICE->reset();

		// Reset grid here
		NIKE_MAP_SERVICE->resetGrid();
		
		//Reset Camera
		NIKE_CAMERA_SERVICE->clearCameraEntities();

		//Clear layers
		layers.clear();

		//Create the first layer
		createLayer();
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

	void Scenes::Service::clearLayers() {
		layers.clear();
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

	void Scenes::Service::queueSceneEvent(Scenes::SceneEvent&& new_event) {
		event_queue.push(std::move(new_event));
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

		//Check if curr scene path is still active
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene)) {
			curr_scene.clear();
		}

		//Execute new scene queue
		while (!event_queue.empty()) {

			//Get front of the queue and pop
			auto& action = event_queue.front();

			//Execute action
			switch (action.scene_action) {
			case Scenes::Actions::CHANGE:
				changeScene(action.next_scene_id);
				break;
			case Scenes::Actions::PREVIOUS:
				previousScene();
				break;
			case Scenes::Actions::RESTART:
				restartScene();
				break;
			case Scenes::Actions::RESET:
				resetScene();
				break;
			case Scenes::Actions::CLOSE:
				NIKE_WINDOWS_SERVICE->getWindow()->terminate();
				break;
			default:
				break;
			}

			event_queue.pop();
		}
	}
}
