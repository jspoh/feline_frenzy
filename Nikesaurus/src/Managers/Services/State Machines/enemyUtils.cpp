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

	void Enemy::changeBossElem(const Entity::Type& enemy)
	{
		auto element_com = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);
		if (element_com.has_value())
		{
			// Get delta time
			static float elapsed_time = 0.f;
			elapsed_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
			// Change boss elem every 2 seconds
			if (elapsed_time >= 2.0f)
			{
				// Change boss element between 1,3 (enum idexes)
				int random_number = GameLogic::getRandomNumber(1, 3);
				element_com.value().get().element = static_cast<Element::Elements>(random_number);
				elapsed_time = 0;
			}
			
		}
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

		// get gun child entity
		auto const& relation = NIKE_METADATA_SERVICE->getEntityRelation(enemy);
		//auto* child = std::get_if<MetaData::Child>(&relation);
		auto* parent = std::get_if<MetaData::Parent>(&relation);

		auto getGunEntity = [&parent]() {
			for (const auto& child : parent->childrens) {
				// get entity
				auto opt_child_entity = NIKE_METADATA_SERVICE->getEntityByName(child);
				if (!opt_child_entity.has_value()) {
					NIKEE_CORE_WARN("shootBullet: CHILD entity not found");
					continue;
				}
				Entity::Type child_entity = opt_child_entity.value();

				// get child entity prefab
				const std::string child_prefab = NIKE_METADATA_SERVICE->getEntityPrefabID(child_entity);
				if (child_prefab == "gun_enemy_n.prefab" || child_prefab == "gun_enemy_n_2.prefab") {
					return child_entity;
				}
			}
			return (unsigned short)-1;
		};

		// get gun entity
		Entity::Type gun_entity = getGunEntity();
		if (gun_entity == (unsigned short)-1) {
			NIKEE_CORE_WARN("shootBullet: GUN entity not found");
			return;
		}

		// set gun animation
		auto gun_animation = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(gun_entity);
		if (gun_animation.has_value()) {
			gun_animation.value().get().animation_mode = Animation::Mode::PLAYING;
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
		const auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(enemy);
		if (!e_physics_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing Physics component!");
			return;
		}

		// Get enemy attack component
		const auto e_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy);
		if (!e_attack_comp.has_value()) {
			NIKEE_CORE_WARN("ENEMY missing ATTACK component!");
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

		// Flip dir
		move_left = !move_left;


		const auto& enemy_attack_comp = e_attack_comp.value().get();

		const Vector2f& enemy_pos = e_transform_comp.value().get().position;

		// Define semi-circle radius (spread angle) and number of bullets
		const int num_bullets = 7; // Number of bullets in the spray
		const float spread_angle = 100.0f; // 180 degrees for the semi-circle
		const float angle_step = spread_angle / (num_bullets - 1); // Divide the arc by the number of bullets

		// Create bullets in the semi-circle pattern
		for (int i = 0; i < num_bullets; ++i) {
			// Create bullet entities
			Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity();

			// Load entity from prefab (assuming bullets can have an element component)
			const auto e_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(enemy);
			if (e_element_comp.has_value()) {
				// Shoot elemental bullets
				NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, Element::enemyBullet[static_cast<int>(e_element_comp.value().get().element)]);
			}
			else {
				NIKEE_CORE_WARN("ENEMY missing Elemental Component");
				NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, "bullet.prefab");
			}

			// Calculate direction for each bullet
			float angle_offset = (i - (num_bullets / 2)) * angle_step; // Spread across the semi-circle

			// Create direction vector for bullet's trajectory
			Vector2f direction = player_pos - enemy_pos;
			direction.normalize();

			// Rotate the direction to create a spread
			// Convert to radians
			float angle_radians = static_cast<float>(angle_offset * (M_PI / 180.0f)); 
			Vector2f rotated_direction;
			rotated_direction.x = direction.x * cos(angle_radians) - direction.y * sin(angle_radians);
			rotated_direction.y = direction.x * sin(angle_radians) + direction.y * cos(angle_radians);

			// Set bullet's initial position and direction
			Vector2f bullet_pos = enemy_pos + rotated_direction * enemy_attack_comp.offset;

			// Set bullet's transform component (position and rotation)
			auto bullet_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
			if (bullet_transform.has_value()) {
				auto& transform = bullet_transform.value().get();
				transform.position = bullet_pos;

				// Calculate rotation angle for bullet
				float bullet_angle = atan2(rotated_direction.y, rotated_direction.x);
				transform.rotation = (bullet_angle * 180.0f / static_cast<float>(M_PI)) - 90.0f; // Convert to degrees
			}

			// Set bullet's physics force (velocity)
			auto bullet_physics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
			if (bullet_physics.has_value()) {
				// Apply force along the rotated direction
				bullet_physics.value().get().force = rotated_direction; 
			}

			// Bullet SFX
			auto bullet_sfx = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity);
			if (bullet_sfx.has_value()) {
				bullet_sfx.value().get().b_play_sfx = true;
				bullet_sfx.value().get().pitch = GameLogic::getRandomNumber(0.5f, 2.0f);
			}
		}
	}

}
