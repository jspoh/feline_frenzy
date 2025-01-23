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
	void Enemy::Manager::init() {

		// Init variables
		movement_speed = 100.0f;
		waypoint_threshold = 1.0f;
		target_threshold = 1.0f;

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

					// Check for pathfinding comp
					if (NIKE_ECS_MANAGER->checkEntityComponent<Pathfinding::Path>(entity)) {
						//auto e_enemy_pathfind = NIKE_ECS_MANAGER->getEntityComponent<Pathfinding::Path>(entity);
						//auto& enemy_pathfind = e_enemy_pathfind.value().get();

						//auto e_enemy_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
						//auto& enemy_transform = e_enemy_transform.value().get();

						//// Init cell
						//// Vector2i start = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_transform.position).value();
						//// Get grid to take ref from cell using the index
						//auto grid = NIKE_MAP_SERVICE->getGrid();
						//// Syntax: grid[y][x]
						//enemy_pathfind.start_cell = grid[2][1];
						//enemy_pathfind.goal_cell = grid[5][5];
						//enemy_pathfind.path = NIKE_MAP_SERVICE->findPath(enemy_pathfind.start_cell, enemy_pathfind.goal_cell);
						//enemy_pathfind.path_found = !enemy_pathfind.path.empty();

						//if (enemy_pathfind.path_found)
						//{
						//	NIKE_MAP_SERVICE->PrintPath(enemy_pathfind.path);
						//	//moveAlongPath(enemy_pathfind, enemy_transform);
						//}

					}



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

	void NIKE::Enemy::Manager::moveAlongPath(Pathfinding::Path& path, Transform::Transform& transform) {
		if (!path.path_found || path.path.empty()) {
			return;
		}

		// Use the first position in the path as the current target
		Vector2i current_target_index = path.goal_cell.index;

		// Convert grid index to world position
		auto grid = NIKE_MAP_SERVICE->getGrid();
		if (current_target_index.x < 0 || current_target_index.x >= grid.size() ||
			current_target_index.y < 0 || current_target_index.y >= grid[0].size()) {
			path.path_found = false;
			return;
		}

		Map::Cell& target_cell = grid[current_target_index.y][current_target_index.x];
		if (target_cell.b_blocked) {
			path.path_found = false;
			return;
		}

		Vector2f target_world_position = target_cell.position;

		// Calculate direction to the target position
		Vector2f direction = (target_world_position - transform.position).normalized();

		// Clamp direction to cardinal directions only
		if (fabs(direction.x) > fabs(direction.y)) {
			direction.y = 0;  
		}
		else {
			direction.x = 0;  
		}

		// Move the entity towards the target position
		transform.position += direction * movement_speed * NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

		// Check if the entity has reached the target position
		if ((transform.position - target_world_position).length() <= waypoint_threshold) {
			// Remove the current target from the path once reached
			path.path.erase(path.path.begin());

			if (path.path.empty()) {
				// Completed the path traversal
				path.path_found = false;
			}
		}
	}



	//bool Enemy::Manager::hasTargetMoved(Vector2f const& target_pos, const Pathfinding::Path& path) const {
	//	return (path.path.empty() || (target_pos - path.path.back()).length() > target_threshold);
	//}

	//void Enemy::Manager::chasing(Pathfinding::Path& path, Transform::Transform& enemy, Transform::Transform& player_target)
	//{
	//	// Compute a path if not already found or if the target has moved significantly
	//	if (!path.path_found || hasTargetMoved(player_target.position, path)) {
	//		path.path = NIKE_MAP_SERVICE->findPath(enemy.position, player_target.position);
	//		path.current_index = 0;
	//		path.path_found = !path.path.empty();
	//	}

	//	// Move along the path
	//	moveAlongPath(path, enemy);
	//}

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

		//NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);

		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}

	void Enemy::Manager::shootBullet(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform component
		const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		const Vector2f& player_pos = p_transform_comp.value().get().position;

		// Get enemy components
		// Transform Comp
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		const Vector2f& enemy_pos = e_transform_comp.value().get().position;

		// Attack Comp
		const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		const auto& enemy_attack_comp = e_attack_comp.value().get();
		//const std::string& bullet_prefab = enemy_attack_comp.prefab_path;

		// Element comp
		const auto e_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);

		// Create entity for bullet
		// Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(enemy_attack_comp.layer);
		// I don't think this layer number actually matters, since it depends on the prefab layer
		Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity(1); 

		// Load entity from prefab
		if (e_element_comp.has_value()) {
			// Shoot elemental bullet
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath(Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]).string());
		}
		else {
			// Missing Element Comp
			NIKEE_CORE_WARN("ENEMY missing Elemental Component");
			NIKE_SERIALIZE_SERVICE->loadEntityFromFile(bullet_entity, NIKE_ASSETS_SERVICE->getAssetPath("bullet.prefab").string());
		}
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