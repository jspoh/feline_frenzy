/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (50%)
 * \date   September 2024
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/GameLogic/sysGameLogic.h"
#include "Core/Engine.h"

namespace NIKE {

	void GameLogic::Manager::init() {

		NIKE_FSM_SERVICE->init();
	}

	void GameLogic::Manager::update() {
		//Get layers
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		//Reverse Iterate through layers
		for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

			//Skip inactive layer
			if (!(*layer)->getLayerState())
				continue;

			//Iterate through all entities
			for (auto& entity : entities) {
				if ((*layer)->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;

				//Check for player logic comp
				const auto e_logic_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(entity);
				if (e_logic_comp.has_value()) {
					auto& e_logic = e_logic_comp.value().get();

					//Check if script is active
					if (e_logic.script.script_id == "")
						continue;

					//Default named argument passed to the script
					e_logic.script.named_args["entity"] = entity;

					//Execute script
					NIKE_LUA_SERVICE->executeScript(e_logic.script);
				}

				//Check for health comp
				//const auto e_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
				//if (e_health_comp.has_value()) {
				//	// Health bar does not exist
				//	if (e_health_comp.value().get().healthBarActive) {
				//		// Update health bar position to entity position

				//	}
				//	else {
				//		// Spawn health prefab
				//		spawnHealthBar(entity);
				//	}

				//}
				// Update of FSM will be called here
				NIKE_FSM_SERVICE->update(const_cast<Entity::Type&>(entity));

				// Destroy all entities that are marked for deletion
				NIKE_ECS_MANAGER->destroyMarkedEntities();
			}
		}
	}

	void GameLogic::Manager::spawnHealthBar(const Entity::Type& entity) {
		// Get entity transform component
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		const Vector2f& entity_pos = e_transform_comp.value().get().position;

		// Create entity for health bar
		Entity::Type health_entity = NIKE_ECS_MANAGER->createEntity(1);
		NIKE_SERIALIZE_SERVICE->loadEntityFromFile(health_entity, NIKE_ASSETS_SERVICE->getAssetPath("healthBar.prefab").string());

		// Offset spawn position of health bar
		const float& offset = 100;
		Vector2f health_pos = entity_pos;
		health_pos.y += offset;

		
		// Set health bar position
		auto health_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(health_entity);
		if (health_transform_comp.has_value()) {
			health_transform_comp.value().get().position = health_pos;
		}

		// Set healthBarActive to true
		NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity).value().get().healthBarActive = true;
		
	}
}
