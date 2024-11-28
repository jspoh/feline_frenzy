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
		lua_system = std::make_unique<Lua::System>();
		lua_system->init();
	}

	void Enemy::Manager::registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system) {
		//Add system to lua
		lua_system->registerLuaSystem(system);
	}

	sol::protected_function Enemy::Manager::executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function) {
		//Run script
		if (script_id == "") {
			script_id = lua_system->loadScript(file_path);
			b_loaded = true;
			return lua_system->executeScript(script_id, function);
		}
		else if (b_loaded) {
			return lua_system->executeScript(script_id, function);
		}
		else {
			lua_system->reloadScript(script_id);
			b_loaded = true;
			return lua_system->executeScript(script_id, function);
		}
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

						auto& enemy_comp = e_enemy_comp.value().get();

						// If shot on cooldown
						if (enemy_comp.last_shot_time <= enemy_comp.cooldown) {
							// Accumulate time since last shot
							enemy_comp.last_shot_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
						}

						// Look for entity w player component
						for (auto& other_entity : entities) {
							auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::Movement>(other_entity);
							// If player entity exists
							if (e_player_comp.has_value()) {
								// Check if player is within range & shot not on cooldown
								if (enemy_comp.last_shot_time >= enemy_comp.cooldown && withinRange(entity, other_entity)) {
									//NIKEE_CORE_INFO("Yes");
									shootBullet(entity, other_entity);
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

		NIKEE_CORE_INFO("Distance = {}, Enemy Range = {}", distance, enemy_range);
		
		// It is recommended to use enemy_range^2, but it's probably easier this way
		return distance < enemy_range;
	}

	 void Enemy::Manager::shootBullet(const Entity::Type enemy, const Entity::Type player) {
		 // Get player transform
		 auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
		 Vector2f player_pos = player_transform_comp.value().get().position;

		 // Get enemy transform
		 auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy);
		 Vector2f enemy_pos = enemy_transform_comp.value().get().position;

		 // Enemy attack
		 auto& enemy_attack_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Attack>(enemy).value().get();

		 // Load Lua Script
		 std::string script_id = lua_system->loadScript(enemy_attack_comp.script.script_path);

		 // Check if the script is loaded successfully
		 if (script_id.empty()) {
			 NIKEE_CORE_ERROR("Failed to load script.");
		 }

		 // Execute Lua Script
		 sol::protected_function enemy_bullet_func = lua_system->executeScript(script_id, enemy_attack_comp.script.function);

		 // Checking if something went wrong w cpp func
		 if (!enemy_bullet_func.valid()) {
			 NIKEE_CORE_ERROR("Failed to execute Lua script: " + enemy_attack_comp.script.function);
		 }
		 else {
			 // Function was valid 
			 sol::protected_function_result result = enemy_bullet_func(enemy_attack_comp.layer, enemy_attack_comp.prefab_path, enemy_pos.x, enemy_pos.y, player_pos.x, player_pos.y, enemy_attack_comp.offset);

			 // Checking if something went wrong with lua func
			 if (!result.valid()) {
				 sol::error err = result;
				 NIKEE_CORE_ERROR(fmt::format("Lua error: {}", err.what()));
			 }
		 }

		 // Reset the last shot time after shooting
		 enemy_attack_comp.last_shot_time = 0.f;
	 }
}