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

					// Check for attack comp
					auto e_enemy_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(entity);
					if (e_enemy_comp.has_value()) {
						// Look for entity w player component
						for (auto& other_entity : entities) {
							auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Movement>(other_entity);
							if (e_player_comp.has_value()) {
								
								if (withinRange(entity, other_entity)) {
									NIKEE_CORE_INFO("Player within range");
								}
								else {
									NIKEE_CORE_INFO("Not in range");
								}
							}
						}
					}
				}
			}
		}
	}

	 bool Enemy::Manager::withinRange(const Entity::Type enemy, const Entity::Type player) {
		// Get player transform
		auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		Vector2f player_pos = player_transform_comp.value().get().position;
		Vector2f player_scale = player_transform_comp.value().get().scale;

		// Get enemy transform
		auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		Vector2f enemy_pos = enemy_transform_comp.value().get().position;
		Vector2f enemy_scale = enemy_transform_comp.value().get().scale;

		// Get enemy range
		auto enemy_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		float enemy_range = enemy_attack_comp.value().get().range;

		// Calculations
		float avg_scale_x = (enemy_scale.x + player_scale.x) / 2;
		float avg_scale_y = (enemy_scale.y + player_scale.y) / 2;

		float dist_x = (enemy_pos.x - player_pos.x) / avg_scale_x;
		float dist_y = (enemy_pos.y - player_pos.y) / avg_scale_y;

		float distance = (dist_x * dist_x) + (dist_y * dist_y);

		//NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);
		
		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}
}