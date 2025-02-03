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
		for (auto layer = layers.rbegin(); layer != layers.rend(); ++layer) {

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

				// Check for Spawner comp
				const auto e_spawner_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Spawner>(entity);
				if (e_spawner_comp.has_value()) {
					auto& e_spawner = e_spawner_comp.value().get();

					// If spawn on cooldown
					if (e_spawner.last_spawn_time <= e_spawner.cooldown) {
						// Accumulate time since last shot
						e_spawner.last_spawn_time += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
					}
					
					// If enemies spawned less than spawn limit
					if (e_spawner.enemies_spawned < e_spawner.enemy_limit) {
						// If spawn on not cooldown
						if(e_spawner.last_spawn_time >= e_spawner.cooldown) {
							// Spawn enemy
							spawnEnemy(entity);

							// Reset last time spawned
							e_spawner.last_spawn_time = 0.f;
						}
					}
				}

				// Update of FSM will be called here
				NIKE_FSM_SERVICE->update(const_cast<Entity::Type&>(entity));
			}
		}
	}

	void GameLogic::Manager::spawnEnemy(const Entity::Type& spawner) {
		// Get spawner position
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(spawner);
		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("spawnEnemy: SPAWNER missing Transform Component, enemy not spawned");
			return;
		}
		const Vector2f& spawner_pos = e_transform_comp.value().get().position;

		// Create enemy entity
		Entity::Type enemy_entity = NIKE_ECS_MANAGER->createEntity(0);

		// Load entity from prefab
		std::string chosen_enemy = enemyArr[getRandomNumber(0, 3)];
		NIKE_SERIALIZE_SERVICE->loadEntityFromFile(enemy_entity, NIKE_ASSETS_SERVICE->getAssetPath(chosen_enemy).string());

		// Randomly offset from spawner position
		float offset_x = static_cast<float>(getRandomNumber(-20, 20));
		float offset_y = static_cast<float>(getRandomNumber(-20, 20));

		// Set Enemy Position
		auto enemy_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(enemy_entity);
		if (enemy_transform_comp.has_value()) {
			enemy_transform_comp.value().get().position = { spawner_pos.x + offset_x, spawner_pos.y + offset_y };
		}

		// Increment Enemies Spawned
		int& e_enemies_spawned = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Spawner>(spawner).value().get().enemies_spawned;
		++e_enemies_spawned;
	}

	//void GameLogic::Manager::spawnHealthBar(const Entity::Type& entity) {
	//	// Get entity transform component
	//	const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
	//	const Vector2f& entity_pos = e_transform_comp.value().get().position;

	//	// Create entity for health bar
	//	Entity::Type health_entity = NIKE_ECS_MANAGER->createEntity(1);
	//	NIKE_SERIALIZE_SERVICE->loadEntityFromFile(health_entity, NIKE_ASSETS_SERVICE->getAssetPath("healthBar.prefab").string());

	//	// Offset spawn position of health bar
	//	const float& offset = 100;
	//	Vector2f health_pos = entity_pos;
	//	health_pos.y += offset;

	//	
	//	// Set health bar position
	//	auto health_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(health_entity);
	//	if (health_transform_comp.has_value()) {
	//		health_transform_comp.value().get().position = health_pos;
	//	}

	//	// Set healthBarActive to true
	//	NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity).value().get().healthBarActive = true;
	//	
	//}
}
