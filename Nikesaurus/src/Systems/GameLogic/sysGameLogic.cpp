/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (40%)
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (40%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (20%)
 * \date   September 2024
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/GameLogic/sysGameLogic.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Core/Engine.h"

namespace NIKE {

	void GameLogic::Manager::init() {

	}

	void GameLogic::Manager::update() 
	{
		//Get layers
		auto& layers = NIKE_SCENES_SERVICE->getLayers();

		//Get Tags
		auto background_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("Background");

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

					for (auto& background : background_tags) {
						const auto background_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(background);

						if (background_transform_comp) {
							// Scroll background
							const float scroll_speed = 10.f;

							auto& background_transform_x = background_transform_comp.value().get().position.x;
							background_transform_x += scroll_speed * NIKE_WINDOWS_SERVICE->getDeltaTime();

							// Loop back
							const float start_pos = -1599.f;
							const float reset_pos = 1599.f;

							if (background_transform_x >= reset_pos) {
								background_transform_x = start_pos;
							}
						}
					}
				}

				// Handle Logic component 
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

				// Player tag
				auto player_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("player");

				updateStatusEffects(entity);

				// Spawner Logic
				const auto e_spawner_comp = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Spawner>(entity);
				if (e_spawner_comp.has_value()) {
					auto& e_spawner = e_spawner_comp.value().get();

					// If spawn on cooldown
					if (e_spawner.last_spawn_time <= e_spawner.cooldown) {
						// Accumulate time since last shot
						e_spawner.last_spawn_time += NIKE_WINDOWS_SERVICE->getDeltaTime();
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

					// Enemy tag
					auto enemy_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("enemy");

					static bool is_spawn_portal = false;

					// Cheat codes
					if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_H)) { is_spawn_portal = true; }
					
					if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_G)) { resetHealth(); }

					static float elapsed_time_before = 0.0f;

					// Portal animations and interactions
					if ((enemy_tags.empty() && e_spawner.enemies_spawned == e_spawner.enemy_limit) || 
						is_spawn_portal)
					{
						auto overlay_text_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("screen_text");

						// UI Overlay Opacity
						for (const auto& overlay_entity : overlay_text_tag)
						{
							auto text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(overlay_entity);


							elapsed_time_before += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
							// Update text alpha
							if (text_comp.has_value())
							{
								Vector4f& color = text_comp.value().get().color;

								float fade_duration = 0.5f;
								if (elapsed_time_before < fade_duration) // Fade-in
								{
									color.a = std::clamp(elapsed_time_before / fade_duration, 0.0f, 1.0f);
								}
								else if (elapsed_time_before < 2.0f) // Full visible
								{
									color.a = 1.0f;
								}
								else if (elapsed_time_before < 3.0f) // Fade-out
								{
									color.a = std::clamp(1.0f - ((elapsed_time_before - 2.0f) / fade_duration), 0.0f, 1.0f);
								}
								else // Reset
								{
									color.a = 0.0f;
								}
							}
						}
						handlePortalInteractions(is_spawn_portal, elapsed_time_before);
					}
				}

				// Elemental UI 
				auto elem_ui_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("elementui");
				auto hp_container_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("hpcontainer");

				for (auto& elementui : elem_ui_tag) {
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
							int element_state = static_cast<int>(player_element.value().get().element);
							//elementui_texture.value().get().texture_id = Element::elementUI[static_cast<int>(player_element.value().get().element)];
							elementui_texture.value().get().frame_index.x = element_state;
							
							// Hardcoded af
							for (auto& hp_container : hp_container_tag) {
								const auto e_container_texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(hp_container);
								if (!e_container_texture.has_value()) {
									continue;
								}
								e_container_texture.value().get().frame_index.x = element_state;

							}
						}
					}
				}

				// Health bar logic
				auto healthbar_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("healthbar");

				for (auto& healthbar : healthbar_tag) {

					if (player_entities.empty()) {

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

				// Cutscene transitions will be called here
				cutsceneTransitions(entity);

				// Update of FSM will be called here
				NIKE_FSM_SERVICE->update(const_cast<Entity::Type&>(entity));
			}
		}
		// Update BGMC volume
		updateBGMCVolume();
	}

	void GameLogic::Manager::updateStatusEffects(Entity::Type entity) {
		// Check for Elemental Stack comp
		const auto e_combo_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Combo>(entity);
		const auto e_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
		const auto e_dynamic_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);

		if (!e_combo_comp.has_value() || !e_health_comp.has_value()) return;

		auto& e_combo = e_combo_comp.value().get();
		auto& e_health = e_health_comp.value().get();
		auto& e_dynamic = e_dynamic_comp.value().get();

		// Exit if no status effect
		if (e_combo.status_effect == Element::Status::NONE || e_combo.status_timer <= 0.0f) { 
			return; 
		}

		// Decrease tick timer
		e_combo.tick_timer = max(0.0f, e_combo.tick_timer - NIKE_WINDOWS_SERVICE->getFixedDeltaTime());

		// Apply status effect if tick timer reached
		if (e_combo.tick_timer <= 0.0f) {
			applyStatusEffect(e_combo, e_health, e_dynamic, entity);
			e_combo.tick_timer = Element::Combo::tick_interval; // Reset tick timer
		}

		// Decrease status effect duration
		e_combo.status_timer = max(0.0f, e_combo.status_timer - NIKE_WINDOWS_SERVICE->getFixedDeltaTime());

		// Remove status effect if expired
		if (e_combo.status_timer <= 0.0f) {
			removeStatusEffect(e_combo, e_dynamic, entity);
		}

		NIKEE_CORE_INFO("Element Status Timer: {}", e_combo.status_timer);
	}

	void GameLogic::Manager::applyStatusEffect(Element::Combo& e_combo, Combat::Health& e_health, Physics::Dynamics& e_dynamic, const Entity::Type entity) {

		UNREFERENCED_PARAMETER(entity);

		switch (e_combo.status_effect) {
		case Element::Status::BURN:
			NIKEE_CORE_INFO("BURN TICK: -1 HP");
			applyBurn(e_health.health, 10.f);
			break;

		case Element::Status::FREEZE:
			NIKEE_CORE_INFO("FROZEN TICK");
			applyFreeze(e_dynamic.max_speed, e_dynamic.max_speed / 2, e_combo.temp_max_speed);
			break;

		case Element::Status::POISON:
			NIKEE_CORE_INFO("POISON TICK: -1 HP");
			applyLifesteal(e_health.health, 5.f);
			break;
		}
	}

	void GameLogic::Manager::removeStatusEffect(Element::Combo& e_combo, Physics::Dynamics& e_dynamic, Entity::Type entity) {
		// Restore speed if frozen
		if (e_combo.status_effect == Element::Status::FREEZE && e_combo.temp_max_speed >= 0) {
			e_dynamic.max_speed = e_combo.temp_max_speed;
			e_combo.temp_max_speed = -1; // Reset
		}

		// Remove status effect
		e_combo.status_effect = Element::Status::NONE;

		// Remove Status Animation
		//std::set<Entity::Type> players = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
		


		auto relation = NIKE_METADATA_SERVICE->getEntityRelation(entity);
		auto* parent = std::get_if<MetaData::Parent>(&relation);

		if (parent) {
			for (const auto child : parent->childrens) {
				auto e = NIKE_METADATA_SERVICE->getEntityByName(child);

				if (!e.has_value()) {
					continue;
				}

				Entity::Type de = e.value();
				std::set<std::string> entity_tags = NIKE_METADATA_SERVICE->getEntityTags(de);
				for (auto& tag : entity_tags) {
					if (tag == "statusAnimation") {
						NIKE_METADATA_SERVICE->destroyEntity(de);
					}
				}
			}
		}
	}


	void GameLogic::Manager::applyLifesteal(float& health, float lifesteal_amount) {
		// Damage enemy
		applyBurn(health, lifesteal_amount);

		// Get all player entities
		std::set<Entity::Type> player_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("player");

		// Check if player alive
		if (player_entities.empty()) {
			return;
		}

		for (auto& player : player_entities) {
			const auto player_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(player);
			if (player_health_comp) {
				auto& player_health = player_health_comp.value().get();

				// Heal player
				if (player_health.health < player_health.max_health) {
					player_health.health = min(player_health.max_health, player_health.health + lifesteal_amount);
				}
			}
		}
	}

	void GameLogic::Manager::applyBurn(float& health, float burn_damage) {
		// Decrement by burn amount
		health = max(0.0f, health - burn_damage);
	}

	void GameLogic::Manager::applyFreeze(float& max_speed, float freeze_speed, float& temp_max_speed) {
		// Only store max speed once
		if (temp_max_speed < 0) {
			temp_max_speed = max_speed;
		}
		// Setting max speed to lowered speed
		max_speed = freeze_speed;
	}

	void GameLogic::Manager::resetHealth()
	{
		std::set<Entity::Type> players = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
		for (auto player : players)
		{
			auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(player);
			if (health_comp.has_value())
			{
				health_comp.value().get().health = 100.0f;
			}
		}
	}

	void GameLogic::Manager::cutsceneTransitions(Entity::Type& entity)
	{
		//Get texture comp
		auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
		auto video_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Video>(entity);

		/************************
		* Video type transitions
		************************/

		// Cutscene transitions
		if (NIKE_SCENES_SERVICE->getCurrSceneID() == "cut_scene_before_boss.scn" && video_comp.has_value())
		{
			//Trigger video to start playing
			if (video_comp.value().get().video_mode == Render::VideoMode::PAUSED) {
				video_comp.value().get().video_mode = Render::VideoMode::PLAYING;
			}

			// Check if video has ended
			if (video_comp.value().get().video_mode == Render::VideoMode::END) {
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "lvl2_2.scn"));
			}
		}

		// Cutscene transitions
		if (NIKE_SCENES_SERVICE->getCurrSceneID() == "cut_after_before_boss.scn" && video_comp.has_value())
		{
			//Trigger video to start playing
			if (video_comp.value().get().video_mode == Render::VideoMode::PAUSED) {
				video_comp.value().get().video_mode = Render::VideoMode::PLAYING;
			}

			// Check if video has ended
			if (video_comp.value().get().video_mode == Render::VideoMode::END) {
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "main_menu.scn"));
			}
		}

		// Cutscene transitions
		if (NIKE_SCENES_SERVICE->getCurrSceneID() == "cut_scene_1.scn" && video_comp.has_value())
		{
			//Trigger video to start playing
			if (video_comp.value().get().video_mode == Render::VideoMode::PAUSED) {
				video_comp.value().get().video_mode = Render::VideoMode::PLAYING;
			}

			// Check if video has ended
			if (video_comp.value().get().video_mode == Render::VideoMode::END) {
				NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, "lvl1_1.scn"));
			}
		}
	}

	void GameLogic::Manager::updateBGMCVolume() {
		// Get the current enemy entities.
		std::set<Entity::Type> enemy_tags = NIKE_METADATA_SERVICE->getEntitiesByTag("enemy");

		// Get the BGM and BGMC channel groups.
		auto bgmGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMChannelGroupID());
		auto bgmcGroup = NIKE_AUDIO_SERVICE->getChannelGroup(NIKE_AUDIO_SERVICE->getBGMCChannelGroupID());

		// Check if groups are valid
		if (!bgmcGroup) { // Need BGMC group at minimum to proceed
			static bool bgmcErrorLogged = false;
			if (!bgmcErrorLogged) {
				NIKEE_CORE_ERROR("updateBGMCVolume: BGMC channel group not found!");
				bgmcErrorLogged = true;
			}
			return;
		}
		// Log error for BGM group if missing, but potentially continue if only BGMC is needed
		if (!bgmGroup) {
			static bool bgmErrorLogged = false;
			if (!bgmErrorLogged) {
				NIKEE_CORE_ERROR("updateBGMCVolume: BGM channel group not found!");
				bgmErrorLogged = true;
			}
		}

		// --- ADDED: Check if BGMC playlist has tracks ---
		bool bgmcHasTrack = false;
		try {
			// Get the BGMC playlist - NOTE: original getChannelPlaylist crashes if ID invalid
			const auto& bgmcPlaylist = NIKE_AUDIO_SERVICE->getChannelPlaylist(NIKE_AUDIO_SERVICE->getBGMCChannelGroupID());
			bgmcHasTrack = !bgmcPlaylist.tracks.empty();
		}
		catch (const std::exception& e) {
			// Catch potential issues if getChannelPlaylist throws instead of crashing (safer)
			static bool playlistErrorLogged = false;
			if (!playlistErrorLogged) {
				NIKEE_CORE_ERROR("updateBGMCVolume: Error retrieving BGMC playlist: {}", e.what());
				playlistErrorLogged = true;
			}
			// Assume no track if playlist access fails
			bgmcHasTrack = false;
		}
		// --- END ADDED CHECK ---

		// Retrieve the target volume (using the global BGM volume as the max).
		float targetMaxVolume = NIKE_AUDIO_SERVICE->getGlobalBGMVolume();
		const float targetMinVolume = 0.0f; // Fade completely out

		// Define the fade duration (in seconds) and compute the fade rate.
		const float fadeTime = 1.5f; // Duration for the fade
		float fadeRate = (fadeTime > 0.0f) ? (targetMaxVolume / fadeTime) : targetMaxVolume;

		// Use a fixed delta time for consistent fading 
		float fixedDelta = static_cast<float>(0.017); // Using the fixed step from original code
		float fadeAmount = fadeRate * fixedDelta;

		// Get current volumes safely (checking if groups are valid)
		float currentBGMVolume = bgmGroup ? bgmGroup->getVolume() : 0.0f;
		float currentBGMCVolume = bgmcGroup->getVolume(); // bgmcGroup guaranteed non-null here

		if (!enemy_tags.empty()) {
			// --- Enemies are present ---
			if (bgmcHasTrack) {
				// Enemies present AND BGMC has track: Fade in BGMC, Fade out BGM.

				// Fade in BGMC
				float newBGMCVolume = currentBGMCVolume + fadeAmount;
				newBGMCVolume = Utility::getMin(newBGMCVolume, targetMaxVolume); // Clamp at max volume
				bgmcGroup->setVolume(newBGMCVolume);

				// Fade out BGM (only if bgmGroup is valid)
				if (bgmGroup) {
					float newBGMVolume = currentBGMVolume - fadeAmount;
					newBGMVolume = Utility::getMax(newBGMVolume, targetMinVolume); // Clamp at min volume (0.0f)
					bgmGroup->setVolume(newBGMVolume);
				}
			}
			else {
				// Enemies present BUT BGMC has NO track: Keep BGMC silent, Fade in BGM.

				 // Keep BGMC Faded OUT
				if (currentBGMCVolume > targetMinVolume) { // Only adjust if not already minimum
					float newBGMCVolume = currentBGMCVolume - fadeAmount; // Fade out if it was somehow playing
					newBGMCVolume = Utility::getMax(newBGMCVolume, targetMinVolume);
					bgmcGroup->setVolume(newBGMCVolume);
				}

				// Fade in BGM (only if bgmGroup is valid)
				if (bgmGroup) {
					float newBGMVolume = currentBGMVolume + fadeAmount;
					newBGMVolume = Utility::getMin(newBGMVolume, targetMaxVolume); // Clamp at max volume
					bgmGroup->setVolume(newBGMVolume);
				}
			}
		}
		else {
			// --- No enemies: Fade out BGMC, Fade in BGM (Original logic here) ---

			// Fade out BGMC
			float newBGMCVolume = currentBGMCVolume - fadeAmount;
			newBGMCVolume = Utility::getMax(newBGMCVolume, targetMinVolume); // Clamp at min volume (0.0f)
			bgmcGroup->setVolume(newBGMCVolume);

			// Fade in BGM (only if bgmGroup is valid)
			if (bgmGroup) {
				float newBGMVolume = currentBGMVolume + fadeAmount;
				newBGMVolume = Utility::getMin(newBGMVolume, targetMaxVolume); // Clamp at max volume
				bgmGroup->setVolume(newBGMVolume);
			}
		}
	} // End updateBGMCVolume


	void GameLogic::Manager::gameOverlay(const std::string& background_texture, const std::string& play_again, const std::string& quit_game_text)
	{
		// Destroy the player's health UI container if applicable
		// NIKE_ECS_MANAGER->destroyEntity(entity);

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

	void GameLogic::Manager::handlePortalInteractions(bool& is_spawn_portal, float& elapsed_time_before)
	{
		auto players = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
		auto vents_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("vent");

		for (auto vent : vents_entities)
		{
			auto texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(vent);
			auto anim_base = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(vent);
			auto anim_sprite = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(vent);

			if (!(texture && anim_base && anim_sprite))
				continue;

			for (const auto& player : players)
			{
				bool in_range = Interaction::isWithinWorldRange(vent, player);

				NIKE_UI_SERVICE->inRangeEntities[vent] = in_range;

				// Scene Change
				if (in_range && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_E))
				{
					// Reset timer for portal text
					elapsed_time_before = 0.f;
					NIKE_SCENES_SERVICE->savePlayerData(NIKE_SERIALIZE_SERVICE->serializePlayerData(player));
					NIKE_AUDIO_SERVICE->playAudio("Laser3.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 0.5f, false, false);

					static int level_counter = 1;
					static int stage_counter = 1;

					std::string curr_scene = NIKE_SCENES_SERVICE->getCurrSceneID();

					if (NIKE_ASSETS_SERVICE->isAssetRegistered(curr_scene))
					{
						if (curr_scene == "lvl1_1.scn")
						{
							++stage_counter;
							curr_scene = "lvl" + std::to_string(level_counter) + "_" + std::to_string(stage_counter) + ".scn";
						}
						else if (curr_scene == "lvl1_2.scn")
						{
							++level_counter;
							stage_counter = 1;
							curr_scene = "lvl" + std::to_string(level_counter) + "_" + std::to_string(stage_counter) + ".scn";
						}
						else if (curr_scene == "lvl2_1.scn" && NIKE_ASSETS_SERVICE->isAssetRegistered("cut_scene_before_boss.scn"))
						{
							curr_scene = "cut_scene_before_boss.scn";
							level_counter = 1;
							stage_counter = 1;
						}

						NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::CHANGE, curr_scene));
						is_spawn_portal = false;
					}
				}
			}

			// Portal Animation
			if (texture.value().get().texture_id == "Front gate_animation_sprite.png" || texture.value().get().texture_id == "Frontdoor_animation_sprite.png")
				continue;

			NIKE_AUDIO_SERVICE->playAudio("EnemySpawn1.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);

			if (NIKE_SCENES_SERVICE->getCurrSceneID() == "lvl2_1.scn")
			{
				texture.value().get().texture_id = "Frontdoor_animation_sprite.png";
				anim_base.value().get().frame_duration = 0.09f;
			}
			else
			{
				texture.value().get().texture_id = "Front gate_animation_sprite.png";
				anim_base.value().get().frame_duration = 0.1f;
			}

			texture.value().get().frame_size = { 5, 1 };
			anim_sprite.value().get().start_index = { 0, 0 };
			anim_sprite.value().get().end_index = { 4, 0 };
			anim_sprite.value().get().sheet_size = { 5, 1 };

			Interaction::flipX(vent, false);
		}
	}


	void GameLogic::Manager::spawnEnemy(const Entity::Type& spawner) {
		// Get spawner position
		const auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(spawner);
		const auto enemy_element = NIKE_ECS_MANAGER->getEntityComponent<Enemy::Spawner>(spawner).value().get().enemy_element;

		if (!e_transform_comp.has_value()) {
			NIKEE_CORE_WARN("spawnEnemy: SPAWNER missing Transform Component, enemy not spawned");
			return;
		}
		const Vector2f& spawner_pos = e_transform_comp.value().get().position;

		// Create enemy entity
		Entity::Type enemy_entity = NIKE_ECS_MANAGER->createEntity();

		// When enemy spawn from spawner, set the tag to enemy
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
			// Random enemy spawn
			if (enemy_element == Element::Elements::NONE) {
				chosen_enemy = enemyArr[getRandomNumber(1, 3)];
			}
			else {
				// Spawn enemy of chosen element
				chosen_enemy = enemyArr[static_cast<int>(enemy_element)];
			}
		}


		// Load entity from prefab
		NIKE_METADATA_SERVICE->setEntityPrefab(enemy_entity, chosen_enemy);

		// set enemy entity as parent
		NIKE_METADATA_SERVICE->setEntityParentRelation(enemy_entity);

		// create gun entity
		{
			Entity::Type gun_entity = NIKE_ECS_MANAGER->createEntity();

			if (Utility::randFloat() < 0.5f) {
				// load gun entity from prefab
				NIKE_METADATA_SERVICE->setEntityPrefab(gun_entity, "gun_enemy_n.prefab");
			}
			else {
				// load gun entity from prefab
				NIKE_METADATA_SERVICE->setEntityPrefab(gun_entity, "gun_enemy_n_2.prefab");
			}

			// set gun entity as child
			NIKE_METADATA_SERVICE->setEntityChildRelation(gun_entity);
			//const std::string dbg_gun_prefab = NIKE_METADATA_SERVICE->getEntityPrefabID(gun_entity);

			// get enemy entity name
			std::string enemy_entity_name = NIKE_METADATA_SERVICE->getEntityName(enemy_entity);

			// set gun entity as child of enemy entity
			NIKE_METADATA_SERVICE->setEntityChildRelationParent(gun_entity, enemy_entity_name);

			// check that gun has a parent
			auto const& relation = NIKE_METADATA_SERVICE->getEntityRelation(gun_entity);
			//auto* parent = std::get_if<MetaData::Parent>(&relation);
			auto* child = std::get_if<MetaData::Child>(&relation);

			if (!child || !NIKE_METADATA_SERVICE->getEntityByName(child->parent).has_value()) {
				NIKEE_CORE_ERROR("Gun entity has no parent");
			}
		}

		// Create Shadow Entity
		{
			Entity::Type shadow_entity = NIKE_ECS_MANAGER->createEntity();

			NIKE_METADATA_SERVICE->setEntityPrefab(shadow_entity, "shadow.prefab");

			// set gun entity as child
			NIKE_METADATA_SERVICE->setEntityChildRelation(shadow_entity);
		
			// get enemy entity name
			std::string enemy_entity_name = NIKE_METADATA_SERVICE->getEntityName(enemy_entity);

			// set gun entity as child of enemy entity
			NIKE_METADATA_SERVICE->setEntityChildRelationParent(shadow_entity, enemy_entity_name);

			// check that gun has a parent
			auto const& relation = NIKE_METADATA_SERVICE->getEntityRelation(shadow_entity);
			auto* child = std::get_if<MetaData::Child>(&relation);

			if (!child || !NIKE_METADATA_SERVICE->getEntityByName(child->parent).has_value()) {
				NIKEE_CORE_ERROR("Shadow entity has no parent");
			}
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

}
