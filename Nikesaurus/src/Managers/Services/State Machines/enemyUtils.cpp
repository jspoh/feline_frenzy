/*****************************************************************//**
 * \file   enemyUtils.cpp
 * \brief  Enemy system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (70%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (30%)
 *
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/State Machine/enemyUtils.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Systems/GameLogic/sysGameLogic.h"

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
				auto& start_index = start.value();

				static float path_recalc_timer = 0.0f;
				path_recalc_timer += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
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

	bool Enemy::isWithinGridRange(const Entity::Type& enemy, const Entity::Type& player) {
		// Get player transform
		const auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		// Get enemy transform
		const auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		// Get enemy attack range (in grid cells)
		const auto enemy_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);

		if (player_transform_comp.has_value() && enemy_transform_comp.has_value() && enemy_attack_comp.has_value()) {
			const Vector2f player_pos = player_transform_comp.value().get().position;
			const Vector2f enemy_pos = enemy_transform_comp.value().get().position;

			// Range in grid cells
			const float enemy_range = enemy_attack_comp.value().get().range;  

			// Convert player and enemy positions to grid cell indexes
			auto player_cell = NIKE_MAP_SERVICE->getCellIndexFromCords(player_pos);
			auto enemy_cell = NIKE_MAP_SERVICE->getCellIndexFromCords(enemy_pos);

			if (player_cell.has_value() && enemy_cell.has_value()) {
				// Calculate the distance between player and enemy in grid cells
				const int dist_x = std::abs(player_cell.value().x - enemy_cell.value().x);
				const int dist_y = std::abs(player_cell.value().y - enemy_cell.value().y);

				// Calculate the Manhattan distance (or use Euclidean if desired)
				const int distance = dist_x + dist_y;

				// Check if within the specified range (in grid cells)
				return distance <= enemy_range;
			}
		}

		return false;
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
		Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		if (e_element_comp.has_value()) {
			// Shoot elemental bullet
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]);
		}
		else {
			// Missing Element Comp
			NIKEE_CORE_WARN("shootBullet: ENEMY missing Elemental Component");
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, "bullet.prefab");
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
			auto& transform = bullet_transform_comp.value().get();
			transform.position = bullet_pos;

			// Calculate rotation angle in radians
			float angle = std::atan2(direction.y, direction.x); // Radians

			// Convert to degrees if your system uses degrees
			transform.rotation = (angle * (180.0f / static_cast<float>(M_PI))) - 90.0f;   // Convert radians to degrees
		}

		// Set bullet physics
		auto bullet_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
		if (bullet_physics_comp.has_value()) {
			// Set force
			bullet_physics_comp.value().get().force = { direction.x, direction.y };
		}

		//Set bullet SFX
		auto bullet_sfx = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity);
		if (bullet_sfx.has_value()) {
			bullet_sfx.value().get().b_play_sfx = true;
			// float rand_pitch = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);		
			std::random_device rd;
			// Mersenne Twister random engine
			std::mt19937 gen(rd());  
			// Rand pitch between 0.5 and 2
			std::uniform_real_distribution<float> dist(0.5f, 2.0f);

			float rand_pitch = GameLogic::getRandomNumber(0.5f,2.f);
			bullet_sfx.value().get().pitch = rand_pitch;
		}
	}

	void Enemy::bossShoot(const Entity::Type& enemy, const Entity::Type& player)
	{
		// Get player transform component
		const auto p_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		if (!p_transform_comp.has_value()) {
			NIKEE_CORE_WARN("PLAYER missing TRANSFORM component!");
			return;
		}
		const Vector2f& player_pos = p_transform_comp.value().get().position;

		// Get enemy components
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing TRANSFORM component!");
			return;
		}

		// Get enemy physics component
		const auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(enemy);
		if (!e_physics_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing Physics component!");
			return;
		}

		const auto e_ani_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(enemy);
		if (!e_ani_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing Ani component!");
			return;
		}

		// For boss to move slightly left and right while shooting
		static bool move_left = true;
		float move_force = 50.0f;
		// float move_dir = (move_left) ? -static_cast<float>(M_PI) : 0;
		if (move_left) {
			// Move left
			e_physics_comp.value().get().force.x -= move_force;  
		}
		else {
			// Move right
			e_physics_comp.value().get().force.x += move_force;  
		}

		// Get enemy physics component
		const auto e_elem_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);
		if (!e_elem_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing Physics component!");
			return;
		}

		// Play moving animation (left or right)
		// Get element
		//std::string elem_string = Element::getElementString(e_elem_comp.value().get().element);
		//State::setBossAnimation(enemy, "BossAttack", elem_string, move_dir, 0, 6);

		// Flip direction 
		move_left = !move_left;


		const Vector2f& enemy_pos = e_transform_comp.value().get().position;

		// Attack Comp
		const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		if (!e_attack_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing ATTACK component!");
			return;
		}
		const auto& enemy_attack_comp = e_attack_comp.value().get();

		// Create bullets for left and right streams
		Entity::Type bullet_entity_left = NIKE_ECS_MANAGER->createEntity();
		Entity::Type bullet_entity_right = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		const auto e_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);
		if (e_element_comp.has_value()) {
			// Shoot elemental bullets
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity_left, Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]);
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity_right, Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]);
		}
		else {
			NIKEE_CORE_WARN("ENEMY missing Elemental Component");
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity_left, "bullet.prefab");
			NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity_right, "bullet.prefab");
		}

		// Calculate direction for bullet 
		Vector2f direction = player_pos - enemy_pos;
		direction.normalize();

		// Adjust for bullet spread distance
		float spread_angle = 0.2f;  
		// Shoot left
		float angle_offset_left = -spread_angle; 
		// Shoot right
		float angle_offset_right = spread_angle;  

		// Left Bullet
		Vector2f direction_left = direction;
		direction_left.x = direction.x * std::cos(angle_offset_left) - direction.y * std::sin(angle_offset_left);
		direction_left.y = direction.x * std::sin(angle_offset_left) + direction.y * std::cos(angle_offset_left);

		// Right Bullet
		Vector2f direction_right = direction;
		direction_right.x = direction.x * std::cos(angle_offset_right) - direction.y * std::sin(angle_offset_right);
		direction_right.y = direction.x * std::sin(angle_offset_right) + direction.y * std::cos(angle_offset_right);

		// Set bullet positions for left and right streams 
		Vector2f bullet_pos_left = enemy_pos + direction_left * enemy_attack_comp.offset;
		Vector2f bullet_pos_right = enemy_pos + direction_right * enemy_attack_comp.offset;

		// Set bullet's position for left stream
		auto bullet_transform_left = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity_left);
		if (bullet_transform_left.has_value()) {
			auto& transform_left = bullet_transform_left.value().get();
			transform_left.position = bullet_pos_left;

			// Calculate rotation angle in radians for left stream
			float angle_left = std::atan2(direction_left.y, direction_left.x);
			transform_left.rotation = (angle_left * (180.0f / static_cast<float>(M_PI))) - 90.0f;
		}

		// Set bullet's position for right stream
		auto bullet_transform_right = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity_right);
		if (bullet_transform_right.has_value()) {
			auto& transform_right = bullet_transform_right.value().get();
			transform_right.position = bullet_pos_right;

			// Calculate rotation angle in radians for right stream
			float angle_right = std::atan2(direction_right.y, direction_right.x);
			transform_right.rotation = (angle_right * (180.0f / static_cast<float>(M_PI))) - 90.0f;
		}

		// Set bullet physics for left stream
		auto bullet_physics_left = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity_left);
		if (bullet_physics_left.has_value()) {
			bullet_physics_left.value().get().force = { direction_left.x, direction_left.y };
		}

		// Set bullet physics for right stream
		auto bullet_physics_right = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity_right);
		if (bullet_physics_right.has_value()) {
			bullet_physics_right.value().get().force = { direction_right.x, direction_right.y };
		}

		// Set bullet SFX for both streams
		// Play sound once is fine, if right bullet played too it will be funny
		auto bullet_sfx_left = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity_left);
		if (bullet_sfx_left.has_value()) {
			bullet_sfx_left.value().get().b_play_sfx = true;
			bullet_sfx_left.value().get().pitch = GameLogic::getRandomNumber(0.5f, 2.0f);
		}

		//auto bullet_sfx_right = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity_right);
		//if (bullet_sfx_right.has_value()) {
		//	bullet_sfx_right.value().get().b_play_sfx = true;
		//	bullet_sfx_right.value().get().pitch = GameLogic::getRandomNumber(0.5f, 2.0f);
		//}
	}
}
