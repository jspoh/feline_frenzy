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
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

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

						auto& enemy_comp = e_enemy_comp.value().get();

						// If shot on cooldown
						if (enemy_comp.last_shot_time <= enemy_comp.cooldown) {
							// Accumulate time since last shot
							enemy_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
						}

						// Look for entity w player component
						for (auto& other_entity : entities) {
							auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
							// If player entity exists
							if (e_player_comp.has_value()) {
								// Check if player is within range & shot not on cooldown
								if (enemy_comp.last_shot_time >= enemy_comp.cooldown && withinRange(entity, other_entity)) {
									// Shoot bullet towards player pos from enemy pos
									shootBullet(entity, other_entity);

									// Reset the last shot time after shooting
									enemy_comp.last_shot_time = 0.f;
								}
							}
						}
					}
				}
			}
		}
	}

	 bool Enemy::Manager::withinRange(const Entity::Type& enemy, const Entity::Type& player) {
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

		NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);
		
		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}

	 void Enemy::Manager::shootBullet(const Entity::Type& enemy, const Entity::Type& player) {
		 // Get player transform component
		 const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		 const Vector2f& player_pos = p_transform_comp.value().get().position;

		 // Get enemy components
		 const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		 const Vector2f& enemy_pos = e_transform_comp.value().get().position;
		 const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		 const auto& enemy_attack_comp = e_attack_comp.value().get();
		 const std::string& bullet_prefab = enemy_attack_comp.prefab_path;

		 // Create entity for bullet
		 //Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(enemy_attack_comp.layer);
		 Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(0);

		 // Load entity from prefab
		 NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath(bullet_prefab).string());

		 // Calculate direction for bullet (Enemy Pos - Player Pos)
		 Vector2f direction = player_pos - enemy_pos;
		 direction.normalize();

		 // Offset spawn position of bullet
		 const float& offset = enemy_attack_comp.offset;
		 Vector2f bullet_pos = enemy_pos + (direction * offset);

		 // Set bullet's position
		 auto bullet_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
		 if (bullet_transform_comp.has_value()) {
			 bullet_transform_comp.value().get().position = bullet_pos;
		 }

		 // Set bullet physics
		 auto bullet_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
		 if (bullet_physics_comp.has_value()) {
			 // Set force
			 bullet_physics_comp.value().get().force = { direction.x, direction.y };
		 }
	 }
}