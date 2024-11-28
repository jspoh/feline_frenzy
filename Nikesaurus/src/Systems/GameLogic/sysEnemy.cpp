/*****************************************************************//**
 * \file   sysEnemy.cpp
 * \brief  Enemy system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 *
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/GameLogic/sysEnemy.h"

namespace NIKE {
	void Enemy::Manager::init() {

	}

	void Enemy::Manager::update() {
		//Get layers
		auto& layers = NIKE_SCENES_SERVICE->getCurrScene()->getLayers();

		//Reverse Iterate through layers
		for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

			//Skip inactive layer
			if (!(*layer)->getLayerState())
				continue;

			//Iterate through all entities
			for (auto& entity : entities) {
				if (NIKE_ECS_MANAGER->checkEntity(entity)) {

					if ((*layer)->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
						continue;

					// If player entity is not saved
					//if (!player_entity) {
					//	// Check for player logic comp
					//	auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Movement>(entity);
					//	if (e_player_comp.has_value()) {
					//		// Save ref to player entity
					//		player_entity = entity; // Assign by value
					//		player_entity_cached = true;
					//	}
					}

					// Check for attack comp
					auto e_enemy_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(entity);
					if (e_enemy_comp.has_value()) {
						// Look for entity w player component
						for (auto& other_entity : entities) {
							auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Movement>(other_entity);
							if (e_player_comp.has_value()) {
								// Get player's position
								auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(other_entity);
								Vector2f player_pos = e_transform_comp.value().get().position;

								NIKEE_CORE_INFO("Player Position: x = {}, y = {}", player_pos.x, player_pos.y);
								// Calculate distance between player and enemy
								// Shoot if distance < range
							}
						}
					}
				}
			}
		}
	}