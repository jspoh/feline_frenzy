/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (50%)
 * \date   September 2024
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/GameLogic/sysGameLogic.h"
#include "Systems/GameLogic/sysInteraction.h"
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
			Transform::Transform(Vector2f(0.0f, -200.0f), Vector2f(375.0f, 75.0f), 0.0f, true),
			Render::Text(),
			Render::Texture("Play_Again_Spritesheet.png", Vector4f(), false, 0.5f, false, Vector2i(7, 1)));

		// Create Quit button
		NIKE_UI_SERVICE->createButton(quit_game_text,
			Transform::Transform(Vector2f(0.0f, -300.0f), Vector2f(375.0f, 75.0f), 0.0f, true),
			Render::Text(),
			Render::Texture("UI_QuitButton_Spritesheet.png", Vector4f(), false, 0.5f, false, Vector2i(7, 1)));

		// Set button input states
		NIKE_UI_SERVICE->setButtonInputState(play_again, UI::InputStates::TRIGGERED);
		NIKE_UI_SERVICE->setButtonInputState(quit_game_text, UI::InputStates::TRIGGERED);

		// Assign Lua scripts to buttons
		auto play_again_script = Lua::Script();
		play_again_script.script_id = "ChangeScene.lua";
		play_again_script.update_function = "Restart";
		NIKE_UI_SERVICE->setButtonScript(play_again, play_again_script, "OnClick");

		auto quit_script = Lua::Script();
		quit_script.script_id = "ChangeScene.lua";
		quit_script.update_function = "Quit";
		NIKE_UI_SERVICE->setButtonScript(quit_game_text, quit_script, "OnClick");
	}

	void GameLogic::Manager::handlePortalInteractions(const std::set<Entity::Type>& vents_entities) {
		for (const Entity::Type& vent : vents_entities) {
			const auto entity_texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(vent);
			const auto entity_animation_base = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(vent);
			const auto entity_animation_sprite = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(vent);

			if (entity_texture.has_value() && entity_animation_base.has_value() && entity_animation_sprite.has_value()) {
				// Change the sprite to be the animation sprite
				entity_texture.value().get().texture_id = "Front gate_animation_sprite.png";
				entity_texture.value().get().frame_size = { 5, 1 };
				entity_texture.value().get().frame_index = { 0, 0 };

				// Set Animation
				Interaction::animationSet(vent, 0, 0, 4, 0);
				Interaction::flipX(vent, false);

				// Check player interaction
				for (const auto& player : NIKE_METADATA_SERVICE->getEntitiesByTag("player")) {
					if (Interaction::isWithinWorldRange(vent, player) && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_E)) {
						// Handle change scene 
						if (NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl1_1.scn")
						{
							NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "lvl1_2.scn"));
						}
						else if (NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl1_2.scn")
						{
							NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "lvl2_1.scn"));
						}
					}
				}
			}
		}
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

				// Main Menu Background Scrolling
				if (NIKE_SCENES_SERVICE->getCurrSceneID() == "main_menu.scn") {
					std::set<Entity::Type> background_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("Background");

					for (auto& background : NIKE_METADATA_SERVICE->getEntitiesByTag("Background")) {
						const auto background_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(background);

						if (background_transform_comp) {
							// Scroll background
							const float scroll_speed = .2f;

							auto& background_transform_x = background_transform_comp.value().get().position.x;
							background_transform_x += scroll_speed;

							// Loop back
							const float start_pos = -1599.f;
							const float reset_pos = 1599.f;

							if (background_transform_x >= reset_pos) {
								background_transform_x = start_pos;
							}
						}
					}
				}

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

				// Check for Elemental Stack comp
				const auto e_combo_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Combo>(entity);
				const auto e_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
				if (e_combo_comp.has_value() && e_health_comp.has_value()) {
					auto& e_combo = e_combo_comp.value().get();
					auto& e_health = e_health_comp.value().get();

					// Entity has status effect
					if (e_combo.status_effect != Element::Status::NONE && e_combo.status_timer > 0.0f) {

						// Decrease tick timer
						e_combo.tick_timer = max(0.0f, e_combo.tick_timer - NIKE_WINDOWS_SERVICE->getFixedDeltaTime());
						
						// Applying status effect
						if (e_combo.tick_timer <= 0.0f) {
							switch (e_combo.status_effect) {
							case Element::Status::BURN:
								NIKEE_CORE_INFO("BURN TICK: -1 HP");
								e_health.health = max(0.0f, e_health.health - 1.0f); // Apply burn damage
								break;
							case Element::Status::FROSTBITE:
								NIKEE_CORE_INFO("FROSTBITE TICK:  -1 HP");
								e_health.health = max(0.0f, e_health.health - 1.0f); // Apply poison damage
								break;
							case Element::Status::POISON:
								NIKEE_CORE_INFO("POISON TICK: -1 HP");
								e_health.health = max(0.0f, e_health.health - 1.0f); // Apply poison damage
								break;
							}

							// Reset tick timer
							e_combo.tick_timer = Element::Combo::tick_interval;
						}

							// Decrease status time
							e_combo.status_timer = max(0.0f, e_combo.status_timer - NIKE_WINDOWS_SERVICE->getFixedDeltaTime());

							// Remove status effect if timer expires
							if (e_combo.status_timer <= 0.0f) {
								e_combo.status_effect = Element::Status::NONE;
							}

							NIKEE_CORE_INFO("Element Status Timer: {}", e_combo.status_timer);
						}
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
							if (e_spawner.last_spawn_time >= e_spawner.cooldown) {
								// Spawn enemy
								spawnEnemy(entity);

								// Reset last time spawned
								e_spawner.last_spawn_time = 0.f;
							}
						}

						// Check if enemies are all dead
						std::set<Entity::Type> enemy_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("enemy");
						// Check if entities with vent tag exists
						std::set<Entity::Type> vents_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("vent");
						// Win whole game overlay
						if (enemy_tags.empty() && e_spawner.enemies_spawned == e_spawner.enemy_limit &&
							NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl2_2.scn") {
							gameOverlay(entity, "You_Win_bg.png", "Play Again", "Quit");
							return;
						}

						// Portal animations and interactions
						if (enemy_tags.empty() && e_spawner.enemies_spawned == e_spawner.enemy_limit)
						{
							handlePortalInteractions(vents_entities);
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
							if (elementui_texture.has_value())
							{
								//elementui_texture.value().get().texture_id = Element::elementUI[static_cast<int>(player_element.value().get().element)];
								elementui_texture.value().get().frame_index.x = static_cast<int>(player_element.value().get().element);
							}
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
							//NIKE_ECS_MANAGER->destroyEntity(healthbar);
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

		// When enemy spwan from spawner, set the tag to enemy
		if(NIKE_METADATA_SERVICE->isTagValid("enemy") && !NIKE_METADATA_SERVICE->checkEntityTagExist(enemy_entity)){
			NIKE_METADATA_SERVICE->addEntityTag(enemy_entity, "enemy");
		}

		// Spawn normal enemy when only enemy tag exist
		std::string chosen_enemy{};

		// Get current scene
		std::string current_scene = NIKE_SCENES_SERVICE->getCurrSceneID();

		// auto const& entity_tags = NIKE_METADATA_SERVICE->getEntityTags(enemy_entity);
		// Spawn boss when only in level 2_2
		// (Not sure how can i remove this scene check, can be optmizxed?
		if (current_scene == "lvl2_2.scn") {
			chosen_enemy = enemyBossArr[getRandomNumber(0, 2)];
			// When enemy spwan from spawner, give boss tag
			if (NIKE_METADATA_SERVICE->isTagValid("boss") && !NIKE_METADATA_SERVICE->checkEntityTagExist(enemy_entity)) {
				NIKE_METADATA_SERVICE->addEntityTag(enemy_entity, "boss");
			}
		}
		else
		{
			chosen_enemy = enemyArr[getRandomNumber(0, 3)];
		}


		// Load entity from prefab
		NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(enemy_entity, chosen_enemy);


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

}
