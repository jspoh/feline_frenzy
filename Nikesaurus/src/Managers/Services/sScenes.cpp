/*****************************************************************//**
 * \file   sScenes.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#include "Core/stdafx.h"
#include "Managers/Services/sScenes.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

namespace NIKE {

	nlohmann::json Scenes::Service::saved_player_data = nlohmann::json();

	/*****************************************************************//**
	 * Layer functions
	 *********************************************************************/
	unsigned int Scenes::Layer::getLayerID() const { return id; }
	void Scenes::Layer::setLayerState(bool state) { b_state = state; }
	bool Scenes::Layer::getLayerState() const { return b_state; }
	void Scenes::Layer::setLayerYSort(bool y_sort_state) { b_ysort = y_sort_state; }
	bool Scenes::Layer::getLayerYSort() const { return b_ysort; }
	void Scenes::Layer::setLayerMask(unsigned int mask_id, bool state) { mask.set(mask_id, state); }
	std::bitset<Scenes::MAXLAYERS> Scenes::Layer::getLayerMask() const { return mask; }
	void Scenes::Layer::insertEntity(Entity::Type entity) {
		for (auto const& e : entities)
			if (e == entity) return;
		entities.push_back(entity);
	}
	void Scenes::Layer::removeEntity(Entity::Type entity) {
		auto it = std::find(entities.begin(), entities.end(), entity);
		if (it != entities.end()) { entities.erase(it); }
	}
	bool Scenes::Layer::checkEntity(Entity::Type entity) {
		return std::find(entities.begin(), entities.end(), entity) != entities.end();
	}
	void Scenes::Layer::sortEntitiesBasedOnMetaData() {
		if (entities.empty()) return;
		std::sort(entities.begin(), entities.end(), [](Entity::Type a, Entity::Type b) {
			return NIKE_METADATA_SERVICE->getEntityLayerOrder(a) < NIKE_METADATA_SERVICE->getEntityLayerOrder(b);
			});
	}
	void Scenes::Layer::sortEntitiesBasedOnYPosition() {
		if (entities.empty()) return;
		std::vector<Entity::Type> sortedEntities = entities;
		std::sort(sortedEntities.begin(), sortedEntities.end(), [](Entity::Type a, Entity::Type b) {
			size_t orderA = NIKE_METADATA_SERVICE->getEntityLayerOrder(a);
			size_t orderB = NIKE_METADATA_SERVICE->getEntityLayerOrder(b);
			const auto& transformA = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(a);
			const auto& transformB = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(b);
			if (!transformA || !transformB) return orderA < orderB;
			const auto& tA = transformA.value().get();
			const auto& tB = transformB.value().get();
			float bottomA = tA.position.y - (tA.scale.y * 0.5f);
			float bottomB = tB.position.y - (tB.scale.y * 0.5f);
			return bottomA > bottomB;
			});
		for (size_t i = 0; i < sortedEntities.size(); ++i)
			setEntityOrder(sortedEntities[i], i);
	}
	void Scenes::Layer::setEntityOrder(Entity::Type entity, size_t order_in_layer) {
		if (entities.empty() || order_in_layer >= entities.size()) return;
		size_t current_index = getEntityOrder(entity);
		if (current_index == (std::numeric_limits<size_t>::max)() || current_index == order_in_layer)
			return;
		if (current_index < order_in_layer)
			std::rotate(entities.begin() + current_index, entities.begin() + current_index + 1, entities.begin() + order_in_layer + 1);
		else
			std::rotate(entities.begin() + order_in_layer, entities.begin() + current_index, entities.begin() + current_index + 1);
		for (size_t i = 0; i < entities.size(); ++i)
			NIKE_METADATA_SERVICE->setEntityLayerOrder(entities.at(i), i);
	}
	size_t Scenes::Layer::getEntityOrder(Entity::Type entity) const {
		auto it = std::find(entities.begin(), entities.end(), entity);
		return (it == entities.end()) ? (std::numeric_limits<size_t>::max)() : std::distance(entities.begin(), it);
	}
	std::vector<Entity::Type> Scenes::Layer::getEntitites() const { return entities; }
	size_t Scenes::Layer::getEntitiesSize() const { return entities.size(); }
	nlohmann::json Scenes::Layer::serialize() const {
		return {
			{"ID", id},
			{"Mask", mask.to_ulong()},
			{"B_State", b_state},
			{"B_YSort", b_ysort}
		};
	}
	void Scenes::Layer::deserialize(nlohmann::json const& data) {
		id = data.value("ID", static_cast<unsigned int>(0));
		mask = LayerMask(data.value("Mask", static_cast<unsigned long>(0)));
		b_state = data.value("B_State", true);
		b_ysort = data.value("B_YSort", false);
	}

	/*****************************************************************//**
	 * Scene manager functions
	 *********************************************************************/
	void Scenes::Service::initScene(std::string const& scene_id) {
		curr_scene = scene_id;
		prev_scene = curr_scene;
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene))
			throw std::runtime_error("Error scene file does not exist");
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	// In changeScene we no longer clear the audio channel groups (which would remove the JSON‐stored playlist).
	// Instead, we get the old and new BGM track and, if they differ, we stop the BGM channel and immediately play the new track.
	void Scenes::Service::changeScene(std::string const& scene_id) {
		if (scene_id == curr_scene)
			return;

		std::string oldBGM = NIKE_AUDIO_SERVICE->getBGMTrackForScene();
		std::string oldBGMC = NIKE_AUDIO_SERVICE->getBGMCTrackForScene();

		prev_scene = curr_scene;
		curr_scene = scene_id;

		NIKE_ECS_MANAGER->destroyAllEntities();
		NIKE_UI_SERVICE->destroyAllButtons();
		// Do not clear audio channel groups so as not to modify the .scn file data.
		NIKE_CAMERA_SERVICE->clearCameraEntities();
		NIKE_METADATA_SERVICE->reset();
		NIKE_MAP_SERVICE->resetGrid();
		layers.clear();
		createLayer();
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
		if (curr_scene.substr(0, 3) == "lvl" && scene_id != "lvl1_1.scn")
			restorePlayerData();

		std::string newBGM = NIKE_AUDIO_SERVICE->getBGMTrackForScene();
		std::string newBGMC = NIKE_AUDIO_SERVICE->getBGMCTrackForScene();

		if (!newBGM.empty() && newBGM != oldBGM) {
			auto bgmGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMChannelGroupID());
			auto bgmcGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMCChannelGroupID());

			if (bgmGroup)
				bgmGroup->stop();
			if (bgmcGroup)
				bgmcGroup->stop();

			// Immediately play the new BGM track (this temporary override does not modify the scene file)
			NIKE_AUDIO_SERVICE->playAudio(newBGM, "", NIKE_AUDIO_SERVICE->getBGMChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalBGMVolume(), 1.f, true, true);
			// BGMC will also restart (and muted at first) if BGM track is different
			NIKE_AUDIO_SERVICE->playAudio(newBGMC, "", NIKE_AUDIO_SERVICE->getBGMCChannelGroupID(), 0.8f * NIKE_AUDIO_SERVICE->getGlobalBGMVolume(), 1.f, true, true);
			bgmcGroup->setVolume(0.01f); // setVolume() is based on the volume set in playAudio()!!!
		}
	}

	void Scenes::Service::restartScene() {
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene))
			return;

		std::string oldBGM = NIKE_AUDIO_SERVICE->getBGMTrackForScene();
		std::string oldBGMC = NIKE_AUDIO_SERVICE->getBGMCTrackForScene();

		NIKE_ECS_MANAGER->destroyAllEntities();
		NIKE_UI_SERVICE->destroyAllButtons();
		// Do not clear audio channel groups.
		NIKE_METADATA_SERVICE->reset();
		NIKE_MAP_SERVICE->resetGrid();
		NIKE_CAMERA_SERVICE->clearCameraEntities();
		layers.clear();
		createLayer();
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);

		std::string newBGM = NIKE_AUDIO_SERVICE->getBGMTrackForScene();
		std::string newBGMC = NIKE_AUDIO_SERVICE->getBGMCTrackForScene();

		if (!newBGM.empty() && newBGM != oldBGM) {
			auto bgmGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMChannelGroupID());
			auto bgmcGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMCChannelGroupID());

			if (bgmGroup)
				bgmGroup->stop();
			if (bgmcGroup)
				bgmcGroup->stop();

			NIKE_AUDIO_SERVICE->playAudio(newBGM, "", NIKE_AUDIO_SERVICE->getBGMChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalBGMVolume(), 1.f, true, true);
			NIKE_AUDIO_SERVICE->playAudio(newBGMC, "", NIKE_AUDIO_SERVICE->getBGMCChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalBGMVolume(), 1.f, true, true); // No volume if not "init" to global... debug volume later
			bgmcGroup->setVolume(0.01f); // setVolume() is based on the volume set in playAudio()!!!
		}
	}

	void Scenes::Service::previousScene() {
		if (prev_scene == curr_scene || !NIKE_ASSETS_SERVICE->isAssetRegistered(prev_scene)) {
			restartScene();
			return;
		}
		std::swap(prev_scene, curr_scene);
		NIKE_ECS_MANAGER->destroyAllEntities();
		NIKE_UI_SERVICE->destroyAllButtons();
		// Do not clear audio channel groups.
		NIKE_CAMERA_SERVICE->clearCameraEntities();
		NIKE_METADATA_SERVICE->reset();
		NIKE_MAP_SERVICE->resetGrid();
		layers.clear();
		createLayer();
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene))
			throw std::runtime_error("Error scene file does not exist");
		NIKE_ASSETS_SERVICE->getExecutable(curr_scene);
	}

	void Scenes::Service::pauseScene()
	{
		//Get all ecs systems
		auto& systems = NIKE_ECS_MANAGER->getAllSystems();

		std::for_each(systems.begin(), systems.end(),
			[](std::shared_ptr<System::ISystem>& system) {
				if (system->getSysName() == "Physics System" || system->getSysName() == "Game Logic System") {
					system->setActiveState(false);
				}
			});

		// Hide pause overlay
		Interaction::togglePauseOverlay(true);
	}

	void Scenes::Service::resumeScene()
	{
		//Get all ecs systems
		auto& systems = NIKE_ECS_MANAGER->getAllSystems();

		std::for_each(systems.begin(), systems.end(),
			[](std::shared_ptr<System::ISystem>& system) {
				system->setActiveState(true);
			});

		// Hide pause overlay
		Interaction::togglePauseOverlay(false);
	}

	void Scenes::Service::resetScene() {
		NIKE_ECS_MANAGER->destroyAllEntities();
		NIKE_UI_SERVICE->destroyAllButtons();
		// For resetScene, we may still clear audio channel groups if desired;
		// if not, remove the call below.
		// NIKE_AUDIO_SERVICE->clearAllChannelGroups();
		NIKE_METADATA_SERVICE->reset();
		NIKE_MAP_SERVICE->resetGrid();
		NIKE_CAMERA_SERVICE->clearCameraEntities();
		layers.clear();
		createLayer();
	}

	std::shared_ptr<Scenes::Layer> Scenes::Service::createLayer(int index) {
		std::shared_ptr<Layer> layer = std::make_shared<Layer>();
		if (index >= 0)
			layers.emplace(layers.begin() + index, layer);
		else {
			layers.push_back(layer);
			index = static_cast<int>(layers.size()) - 1;
		}
		layer->id = index;
		layer->mask.set(layer->id, true);
		return layer;
	}

	std::shared_ptr<Scenes::Layer> Scenes::Service::getLayer(unsigned int layer_id) {
		if (layer_id >= static_cast<unsigned int>(layers.size()))
			throw std::runtime_error("Layer has not been registered.");
		return layers.at(layer_id);
	}

	void Scenes::Service::clearLayers() { layers.clear(); }
	void Scenes::Service::removeLayer(unsigned int layer_id) {
		if (layer_id >= static_cast<unsigned int>(layers.size()))
			throw std::runtime_error("Layer has not been registered.");
		for (auto& layer : layers)
			layer->mask.set(layer_id, false);
		layers.erase(layers.begin() + layer_id);
		unsigned int index = 0;
		for (auto& layer : layers) {
			layer->id = index++;
			for (unsigned int i = layer_id + 1; i < layers.size(); i++) {
				layer->mask[i - 1] = layer->mask[i];
			}
			layer->mask[layers.size() - 1] = false;
		}
	}
	bool Scenes::Service::checkLayer(unsigned int layer_id) {
		return layer_id < static_cast<unsigned int>(layers.size());
	}
	unsigned int Scenes::Service::getLayerCount() const {
		return static_cast<unsigned int>(layers.size());
	}
	std::vector<std::shared_ptr<Scenes::Layer>>& Scenes::Service::getLayers() { return layers; }
	void Scenes::Service::queueSceneEvent(SceneEvent&& new_event) {
		event_queue.push(std::move(new_event));

		// fade out current scene
		//static constexpr float fade_dur = 2.f;
		//if (NIKE_RENDER_SERVICE->getFadeState() == NIKE::Render::FADE_STATE::NONE) {
		//	NIKE_RENDER_SERVICE->fadeOut(fade_dur);
		//}
	}
	void Scenes::Service::setCurrSceneID(std::string const& new_scene_id) { curr_scene = new_scene_id; }
	std::string Scenes::Service::getCurrSceneID() const { return curr_scene; }
	std::string Scenes::Service::getPrevSceneID() const { return prev_scene; }
	void Scenes::Service::restorePlayerData() {
		if (saved_player_data.empty()) return;
		std::set<Entity::Type> players = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
		if (players.empty()) return;
		Entity::Type playerEntity = *players.begin();
		NIKE_SERIALIZE_SERVICE->deserializePlayerData(playerEntity, saved_player_data);
	}
	void Scenes::Service::savePlayerData(nlohmann::json data) { saved_player_data = std::move(data); }
	nlohmann::json Scenes::Service::loadPlayerData() { return saved_player_data; }
	void Scenes::Service::init() { createLayer(); }
	void Scenes::Service::update() {
		if (!NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene))
			curr_scene.clear();
		while (!event_queue.empty()) {

			//     // fade in current scene
			   //  static constexpr float fade_dur = 2.f;
			   //  if (NIKE_RENDER_SERVICE->getFadeState() == NIKE::Render::FADE_STATE::NONE) {
					 //NIKE_RENDER_SERVICE->fadeIn(fade_dur);
			   //  }
			   //  if (NIKE_RENDER_SERVICE->getFadeState() != NIKE::Render::FADE_STATE::NONE) {
			   //      break;
			   //  }

			auto& action = event_queue.front();
			switch (action.scene_action) {
			case Actions::CHANGE:
#ifndef NDEBUG
				if (NIKE_LVLEDITOR_SERVICE->getGameState()) {
					NIKE_RENDER_SERVICE->fadeOut(1.0f);
				}
#endif 
#ifdef NDEBUG
				NIKE_RENDER_SERVICE->fadeOut(1.0f);
#endif

				changeScene(action.next_scene_id);

#ifndef NDEBUG
				if (NIKE_LVLEDITOR_SERVICE->getGameState()) {
					NIKE_RENDER_SERVICE->fadeIn(1.0f);
				}
#endif 
#ifdef NDEBUG
				NIKE_RENDER_SERVICE->fadeIn(1.0f);
#endif

				break;
			case Actions::PREVIOUS:
#ifndef NDEBUG
				if (NIKE_LVLEDITOR_SERVICE->getGameState()) {
					NIKE_RENDER_SERVICE->fadeOut(1.0f);
				}
#endif 
#ifdef NDEBUG
				NIKE_RENDER_SERVICE->fadeOut(1.0f);
#endif


				previousScene();

#ifndef NDEBUG
				if (NIKE_LVLEDITOR_SERVICE->getGameState()) {
					NIKE_RENDER_SERVICE->fadeIn(1.0f);
				}
#endif 
#ifdef NDEBUG
				NIKE_RENDER_SERVICE->fadeIn(1.0f);
#endif

				break;
			case Actions::RESTART:
				restartScene();

				break;
			case Actions::RESET:
				resetScene();

				break;
			case Actions::CLOSE:
				NIKE_RENDER_SERVICE->fadeOut(1.0f);
				NIKE_WINDOWS_SERVICE->getWindow()->terminate();
				break;
			case Actions::RESUME:
				resumeScene();
				break;
			case Actions::PAUSE:
				pauseScene();
				break;
			default:
				break;
			}
			event_queue.pop();
		}
	}
} // namespace NIKE
