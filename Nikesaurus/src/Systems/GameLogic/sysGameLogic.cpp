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

	}

	void GameLogic::Manager::gameOverlay(Entity::Type const& entity, const std::string& background_texture, const std::string& play_again, const std::string& quit_game_text)
	{
		// Destroy the player's health UI container if applicable
		NIKE_ECS_MANAGER->destroyEntity(entity);

		// Create the overlay entity
		auto overlay_entity = NIKE_ECS_MANAGER->createEntity();
		NIKE_METADATA_SERVICE->setEntityLayerID(overlay_entity, NIKE_SCENES_SERVICE->getLayerCount() - 1);
		NIKE_ECS_MANAGER->addEntityComponent<Render::Texture>(
			overlay_entity, Render::Texture(background_texture, Vector4f()));

		// Get viewport size and adjust transform
		auto viewport = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();
		NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(
			overlay_entity, Transform::Transform(
				Vector2f(0.0f, 0.0f),
				viewport * (NIKE_CAMERA_SERVICE->getCameraHeight() / viewport.y),
				0.0f,
				true));

		// Create Play Again button
		NIKE_UI_SERVICE->createButton(play_again,
			Transform::Transform(Vector2f(0.0f, -200.0f), Vector2f(375.0f, 75.0f), 0.0f),
			Render::Text(),
			Render::Texture("UI_PlayGame_spritesheet.png", Vector4f(), false, 0.5f, false, Vector2i(7, 1)));

		// Create Quit button
		NIKE_UI_SERVICE->createButton(quit_game_text,
			Transform::Transform(Vector2f(0.0f, -300.0f), Vector2f(375.0f, 75.0f), 0.0f),
			Render::Text(),
			Render::Texture("UI_QuitButton_Spritesheet.png", Vector4f(), false, 0.5f, false, Vector2i(7, 1)));

		// Set button input states
		NIKE_UI_SERVICE->setButtonInputState(play_again, UI::InputStates::TRIGGERED);
		NIKE_UI_SERVICE->setButtonInputState(quit_game_text, UI::InputStates::TRIGGERED);

		// Assign Lua scripts to buttons
		auto play_again_script = Lua::Script();
		play_again_script.script_id = "ChangeScene.lua";
		play_again_script.function = "Restart";
		NIKE_UI_SERVICE->setButtonScript(play_again, play_again_script);

		auto quit_script = Lua::Script();
		quit_script.script_id = "ChangeScene.lua";
		quit_script.function = "Quit";
		NIKE_UI_SERVICE->setButtonScript(quit_game_text, quit_script);
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
			for (auto& entity : (*layer)->getEntitites()) {

				//Skip entity not registered to this system
				if (entities.find(entity) == entities.end()) continue;

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

					// Check if enemies are all dead
					std::set<Entity::Type> enemy_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("enemy");
					// Win whole game overlay
					if (enemy_tags.empty() && e_spawner.enemies_spawned == e_spawner.enemy_limit) {

						gameOverlay(entity, "You_Win_bg.png", "Play Again", "Quit");
						return;
					}
				}

				// Check if player tag exists
				std::set<Entity::Type> player_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("player");


				// Elemental UI 
				for (auto& elementui : NIKE_METADATA_SERVICE->getEntitiesByTag("elementui")) {
					// If player not dead
					if (player_entities.empty()) {
						continue;
					}

					// Look for player
					for (auto& player : player_entities) {
						const auto player_element = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player);
						const auto elementui_texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(elementui);
						
						// Set element ui to player's element
						elementui_texture.value().get().texture_id = Element::elementUI[static_cast<int>(player_element.value().get().element)];
					}


				}

				for (Entity::Type const& hp_container : NIKE_METADATA_SERVICE->getEntitiesByTag("hpcontainer")) {
					// If no player exists, destroy the health bar
					if (player_entities.empty()) {

						gameOverlay(hp_container, "Defeat_screen_bg.png", "Play Again", "Quit");
						return;
					}
				}

				// Health bar logic
				for (auto& healthbar : NIKE_METADATA_SERVICE->getEntitiesByTag("healthbar")) {
					// If no player exists, destroy the health bar
					if (player_entities.empty()) {
						NIKE_ECS_MANAGER->destroyEntity(healthbar);
						return;
					}

					// Look for player
					for (auto& player : player_entities) {
						const auto e_player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
						const auto e_healthbar_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(healthbar);
						const auto e_player_health = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(player);

						// Check for missing component
						if (e_player_health.has_value() == false || e_player_transform.has_value() == false || e_healthbar_transform.has_value() == false) {
							NIKEE_CORE_WARN("sysGameLogic: healthbar/player missing component(s)");
							continue;
						}

						//const auto& player_pos = e_player_transform.value().get().position;
						//const auto& player_scale = e_player_transform.value().get().scale;
						auto& healthbar_pos = e_healthbar_transform.value().get().position;
						auto& healthbar_scale = e_healthbar_transform.value().get().scale;
						//const float offset_y = player_scale.y * 0.9f;

						// Set healthbar to player health
			
						// Update healthbar location
						// !TODO: Offset the healthbar to the left
						//healthbar_pos.x = player_pos.x;
						//healthbar_pos.y = player_pos.y + offset_y * 0.8f;

						// Constants
						static float original_healthbar_width = healthbar_scale.x;
						static float original_healthbar_x = healthbar_pos.x;  // Store original X position once

						// Get health percentage
						float health_percentage = e_player_health.value().get().health / e_player_health.value().get().max_health;

						// Update health bar scale
						healthbar_scale.x = health_percentage * original_healthbar_width;

						// Offset the health bar so it shrinks from right to left
						healthbar_pos.x = original_healthbar_x - (original_healthbar_width * (1.0f - health_percentage)) * 0.5f;
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
		Entity::Type enemy_entity = NIKE_ECS_MANAGER->createEntity();

		// Load entity from prefab
		std::string chosen_enemy = enemyArr[getRandomNumber(0, 3)];
		NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(enemy_entity, chosen_enemy);

		// When enemy spwan from spawner, set the tag to enemy
		if(NIKE_METADATA_SERVICE->isTagValid("enemy")){
			NIKE_METADATA_SERVICE->addEntityTag(enemy_entity, "enemy");
		}
		// Randomly offset from spawner position		
		float offset_x = getRandomNumber(-20.f, 20.f);
		float offset_y = getRandomNumber(-20.f, 20.f);

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
