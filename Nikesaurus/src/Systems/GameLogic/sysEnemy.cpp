/*****************************************************************//**
 * \file   sysEnemy.cpp
 * \brief  Enemy system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 *
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/GameLogic/sysEnemy.h"

namespace NIKE {
	void Enemy::moveAlongPath(Entity::Type entity, int x_index, int y_index, float speed, float cell_offset) {

		// Get transform of entity for position mapping
		auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (transform.has_value()) {

			// Entity transform
			auto& e_transform = transform.value().get();

			// Get index of entity as the starting position
			auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(e_transform.position);

			// Get cell to travel to
			if (start.has_value()) {

				// Get start index
				auto start_index = start.value();

				static float path_recalc_timer = 0.0f;
				path_recalc_timer += NIKE_WINDOWS_SERVICE->getDeltaTime();
				static int path_stick_threshold = 5; // How many frames to stick to a path
				static int path_follow_counter = 0;

				// Check if path has been generated or if destination cell has changed
				if (!NIKE_MAP_SERVICE->checkPath(entity) ||
					NIKE_MAP_SERVICE->checkGridChanged() ||
					path_recalc_timer >= 0.5f) {
					// Search for path
					NIKE_MAP_SERVICE->findPath(entity, start_index, Vector2i(x_index, y_index));
					path_follow_counter = 0;
					path_recalc_timer = 0.0f;
				}
				else {
					++path_follow_counter;
				}

				// Get path
				auto path = NIKE_MAP_SERVICE->getPath(entity);

				// Check if there are cells left in path
				if (!path.path.empty()) {
					auto const& next_cell = path.path.front();

					float distance = (next_cell.position - e_transform.position).length();

					// Stop moving when close to the FINAL target
					if (distance < cell_offset * 1.5f && path.path.size() == 1) {
						auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
						if (dynamics.has_value()) {
							dynamics.value().get().force = { 0.0f, 0.0f };
						}
						//cout << "Stopping: Reached final goal" << endl;
						// Stop here to prevent force flipping
						return; 
					}

					// Remove the front of the path once the entity reaches it
					if (distance < cell_offset) {
						//cout << "Arrived at next cell, popping front" << endl;
						path.path.pop_front();
					}

					// Normal movement logic
					// Check again in case we removed the last step
					if (!path.path.empty()) {  
						auto const& next_target = path.path.front();

						float dir = atan2((next_target.position.y - e_transform.position.y),
							(next_target.position.x - e_transform.position.x));

						auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
						if (dynamics.has_value()) {
							dynamics.value().get().force = { cos(dir) * speed, sin(dir) * speed };
						}
						//cout << "Moving to next cell, Force: (" << cos(dir) * speed
						//	<< ", " << sin(dir) * speed << ")" << endl;
					}
				}
				else {
					// No path left, stop movement
					auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
					if (dynamics.has_value()) {
						dynamics.value().get().force = { 0.0f, 0.0f };
					}
					//cout << "No more path left, stopping movement." << endl;
					// Stop here if no valid path is available
					return; 
				}

				// If path follows the same direction for too long without progress, recalculate path
				if (path_follow_counter >= path_stick_threshold) {
					//cout << "Path is stuck, recalculating..." << endl;
					path_follow_counter = 0;
					path_recalc_timer = 0.0f;
					NIKE_MAP_SERVICE->findPath(entity, start_index, Vector2i(x_index, y_index));
				}
			}
		}
	}

	bool Enemy::withinRange(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform
		const auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		if (!player_transform_comp.has_value()) {
			NIKEE_CORE_WARN("withinRange: PLAYER missing TRANSFORM component!");
			return false;
		}
		const Vector2f player_pos = player_transform_comp.value().get().position;
		const Vector2f player_scale = player_transform_comp.value().get().scale;

		// Get enemy transform
		const auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		if (!enemy_transform_comp.has_value()) {
			NIKEE_CORE_WARN("withinRange: ENEMY missing TRANSFORM component!");
			return false;
		}
		const Vector2f enemy_pos = enemy_transform_comp.value().get().position;
		const Vector2f enemy_scale = enemy_transform_comp.value().get().scale;

		// Get enemy range
		const auto enemy_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		if (!enemy_attack_comp.has_value()) {
			NIKEE_CORE_WARN("withinRange: ENEMY missing ATTACK component!");
			return false;
		}
		const float enemy_range = enemy_attack_comp.value().get().range;

		// Calculations
		const float avg_scale_x = (enemy_scale.x + player_scale.x) / 2;
		const float avg_scale_y = (enemy_scale.y + player_scale.y) / 2;

		const float dist_x = (enemy_pos.x - player_pos.x) / avg_scale_x;
		const float dist_y = (enemy_pos.y - player_pos.y) / avg_scale_y;

		const float distance = (dist_x * dist_x) + (dist_y * dist_y);

		//NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);

		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}

	void Enemy::shootBullet(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform component
		const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		if (!p_transform_comp.has_value()) {
			NIKEE_CORE_WARN("shootBullet: PLAYER missing TRANSFORM component!");
			return;
		}
		const Vector2f& player_pos = p_transform_comp.value().get().position;

		// Get enemy components
		// Transform Comp
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("shootBullet: ENEMY missing TRANSFORM component!");
			return;
		}
		const Vector2f& enemy_pos = e_transform_comp.value().get().position;

		// Attack Comp
		const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		if (!e_attack_comp.has_value()) {
			NIKEE_CORE_WARN("shootBullet: ENEMY missing ATTACK component!");
			return;
		}
		const auto& enemy_attack_comp = e_attack_comp.value().get();
		//const std::string& bullet_prefab = enemy_attack_comp.prefab_path;

		// Element comp
		const auto e_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);

		// Create entity for bullet
		// Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(enemy_attack_comp.layer);
		// I don't think this layer number actually matters, since it depends on the prefab layer
		Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(0);

		// Load entity from prefab
		if (e_element_comp.has_value()) {
			// Shoot elemental bullet
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath(Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]).string());
		}
		else {
			// Missing Element Comp
			NIKEE_CORE_WARN("shootBullet: ENEMY missing Elemental Component");
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath("bullet.prefab").string());
		}
		// Calculate direction for bullet (Enemy Pos - Player Pos)
		Vector2f direction = player_pos - enemy_pos;
		direction.normalize();

		// Offset spawn position of bullet
		const float& offset = enemy_attack_comp.offset;
		const Vector2f bullet_pos = enemy_pos + (direction * offset);

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


//void Enemy::Manager::init() {

//	// Init variables
//	movement_speed = 100.0f;
//	waypoint_threshold = 1.0f;
//	target_threshold = 1.0f;

//}

//void Enemy::Manager::update() {
//	//Get layers
//	auto& layers = NIKE_SCENES_SERVICE->getLayers();

//	//Reverse Iterate through layers
//	for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

//		//Skip inactive layer
//		if (!(*layer)->getLayerState())
//			continue;

//		//Iterate through all entities
//		for (auto& entity : entities) {
//			if (NIKE_ECS_MANAGER->checkEntity(entity)) {

//				if ((*layer)->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
//					continue;

//				// Check for pathfinding comp
//				if (NIKE_ECS_MANAGER->checkEntityComponent<Pathfinding::Path>(entity)) {
//					//auto e_enemy_pathfind = NIKE_ECS_MANAGER->getEntityComponent<Pathfinding::Path>(entity);
//					//auto& enemy_pathfind = e_enemy_pathfind.value().get();

//					//auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
//					//auto& enemy_transform = e_enemy_transform.value().get();

//					//// Init cell
//					//// Vector2i start = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_transform.position).value();
//					//// Get grid to take ref from cell using the index
//					//auto grid = NIKE_MAP_SERVICE->getGrid();
//					//// Syntax: grid[y][x]
//					//enemy_pathfind.start_cell = grid[2][1];
//					//enemy_pathfind.goal_cell = grid[5][5];
//					//enemy_pathfind.path = NIKE_MAP_SERVICE->findPath(enemy_pathfind.start_cell, enemy_pathfind.goal_cell);
//					//enemy_pathfind.path_found = !enemy_pathfind.path.empty();

//					//if (enemy_pathfind.path_found)
//					//{
//					//	NIKE_MAP_SERVICE->PrintPath(enemy_pathfind.path);
//					//	//moveAlongPath(enemy_pathfind, enemy_transform);
//					//}

//				}



//			// Check for attack comp
//			auto e_enemy_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(entity);
//			if (e_enemy_comp.has_value()) {

//				auto& enemy_comp = e_enemy_comp.value().get();

//				// If shot on cooldown
//				if (enemy_comp.last_shot_time <= enemy_comp.cooldown) {
//					// Accumulate time since last shot
//					enemy_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
//				}

//				// Look for entity w player component
//				for (auto& other_entity : entities) {
//					auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);
//					// If player entity exists
//					if (e_player_comp.has_value()) {
//						// Check if player is within range & shot not on cooldown
//						if (enemy_comp.last_shot_time >= enemy_comp.cooldown && withinRange(entity, other_entity)) {
//							// Shoot bullet towards player pos from enemy pos
//							shootBullet(entity, other_entity);

//							// Reset the last shot time after shooting
//							enemy_comp.last_shot_time = 0.f;
//						}
//					}
//				}
//			}
//		}
//	}
//}
