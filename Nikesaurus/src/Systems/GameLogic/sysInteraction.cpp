/*****************************************************************//**
 * \file   sysInteraction.cpp
 * \brief  Interaction manager
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/GameLogic/sysInteraction.h"
#include "Systems/GameLogic/sysGameLogic.h"
#include "Managers/Services/State Machine/bossEnemyStates.h"
#include "Systems/Render/sysRender.h"

namespace NIKE {
    namespace Interaction {

        void Manager::init() {
            show_pause_menu = false;

        }

        void Manager::update() {

            // Pause Menu
            if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_ESCAPE)) {

                // Get player tag
                std::set<Entity::Type> player_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("player");

                // Prevent pausing when there is no player
                if (player_tag.empty()) {
                    return;
                }

                show_pause_menu = !show_pause_menu;

                static float saved_mouse_offset = 0.f;

                if (show_pause_menu) {

                    // Change cursor
                    for (auto& player : player_tag) {
                        const auto player_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(player);
                        auto& cam = player_cam_comp.value().get();


                        saved_mouse_offset = cam.mouse_offset;
                        cam.mouse_offset = 0.f;
                    }


                    initPauseOverlay("Paused_UI.png", "Resume", "Settings", "How_To_Play", "Quit");

                    NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::PAUSE, ""));
                }
                else {
                    // Change cursor

                    for (auto& player : player_tag) {
                        const auto player_cam_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Cam>(player);
                        auto& cam = player_cam_comp.value().get();

                        cam.mouse_offset = saved_mouse_offset;
                    }

                    NIKE_SCENES_SERVICE->queueSceneEvent(Scenes::SceneEvent(Scenes::Actions::RESUME, ""));
                }

            }

            if (show_pause_menu) {
                return;
            }

            // Player Tab Element Swapping
            std::set<Entity::Type> player_tag = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
            for (auto& player : player_tag) {
                const auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player);
                auto& player_element = player_element_comp.value().get();

                if (NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_R)) {
                    if (player_element.element == Element::Elements::GRASS) {
                        player_element.element = Element::Elements::FIRE;
                    }
                    else {
                        player_element.element = static_cast<Element::Elements>(static_cast<int>(player_element.element) + 1);
                    }
                }
            }

            // When hitting objects
            for (auto& [entity, isHit] : hitEntities) {
                if (!isHit) continue; // Skip objects that haven't been hit

                auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
                if (!texture_comp) continue;

                auto& intensity = texture_comp.value().get().intensity;
                float alpha_speed = 10.0f * NIKE_WINDOWS_SERVICE->getDeltaTime(); // Adjust based on deltaTime

                intensity += (0.0f - intensity) * alpha_speed; // Fade towards 0
                intensity = std::clamp(intensity, 0.0f, 1.0f);

                // Stop updating once intensity is very close to 0
                if (intensity <= 0.01f) {
                    isHit = false;
                }
            }

            // Get layers
            auto& layers = NIKE_SCENES_SERVICE->getLayers();

            // Reverse iterate through layers
            for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

                //Skip inactive layers
                if (!(*layer)->getLayerState())
                    continue;

                // Update heal animation
                for (auto& heal_entity : NIKE_METADATA_SERVICE->getEntitiesByTag("healAnimation")) {
                   
                    // Updating to player location
                    std::set<Entity::Type> player_entities = NIKE_METADATA_SERVICE->getEntitiesByTag("player");
                    if (!player_entities.empty()) {
                        // Get heal animation position
                        auto &heal_animation_pos = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(heal_entity).value().get().position;

                        // Get player position
                        for (auto& player : player_entities) {
                            const auto player_pos = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player).value().get().position;

                            // Set heal animation to player position
                            const float offset_y = 17.f;
                            heal_animation_pos = { player_pos.x, player_pos.y - offset_y };
                        }
                    }

                    // Delete animation when done playing
                    auto animation_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(heal_entity);
                    if (animation_comp.has_value())
                    {
                        if (animation_comp.value().get().completed_animations >= 1) {
                            NIKE_METADATA_SERVICE->destroyEntity(heal_entity);
                        }
                    }
                }

                // Iterate through all entities
                for (auto& entity : (*layer)->getEntitites()) {

                    //Skip entity not registered to this system
                    if (entities.find(entity) == entities.end()) continue;

                    if (NIKE_ECS_MANAGER->checkEntity(entity)) {
                        // Check for Elemental Source component
                        const auto e_source_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Source>(entity);
                        if (e_source_comp.has_value()) {

                            std::set<NIKE::Entity::Type> e_player_comp = NIKE_METADATA_SERVICE->getEntitiesByTag("player");

                            if (e_player_comp.empty()) { // Player entity exists
                                continue;
                            }

                            auto player_entity = *e_player_comp.begin();


                            // Get Render Component
                            const auto source_render_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
                            auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
                            //float& source_intensity = source_render_comp.value().get().intensity;
                            Vector4f& source_alpha = source_render_comp.value().get().color;

                            float target_alpha = isWithinWorldRange(entity, player_entity) ? 1.0f : 0.0f; // Set target alpha
                            float alpha_speed = 10.0f * NIKE_WINDOWS_SERVICE->getDeltaTime(); // Adjust based on deltaTime

                            // Smoothly interpolate alpha
                            source_alpha.a += (target_alpha - source_alpha.a) * alpha_speed;

                            // Clamp alpha between 0 and 1.0
                            source_alpha.a = std::clamp(source_alpha.a, 0.0f, 1.0f);

                            // Player Element Swapping
                            if (isWithinWorldRange(entity, player_entity) && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_E)) {
                                changeElement(player_entity, entity);
                                if (e_sfx_comp.has_value()) {
                                    e_sfx_comp.value().get().b_play_sfx = true;
                                }
                            }


                        }
                    }
                }
            }
        }

        /*****************************************************************//**
         * UI Interactions
         *********************************************************************/


        void initPauseOverlay(const std::string& background_texture, const std::string& resume, const std::string& options, const std::string& how_to_play, const std::string& quit)
        {

            static bool is_initialized = false;
            // Prevent duplicate creation
            if (is_initialized) return;

            is_initialized = true;


            if (!NIKE_METADATA_SERVICE->getEntityByName(background_texture)) {
                // Create the overlay entity
                auto overlay_entity = NIKE_ECS_MANAGER->createEntity();
                NIKE_METADATA_SERVICE->setEntityLayerID(overlay_entity, NIKE_SCENES_SERVICE->getLayerCount() - 1);
                NIKE_METADATA_SERVICE->setEntityName(overlay_entity, background_texture);
                NIKE_ECS_MANAGER->addEntityComponent<Render::Texture>(
                    overlay_entity, Render::Texture(background_texture, Vector4f(), true));

                // Get viewport size and adjust transform
                auto viewport = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();
                NIKE_ECS_MANAGER->addEntityComponent<Transform::Transform>(
                    overlay_entity, Transform::Transform(
                        Vector2f(0.0f, 0.0f),
                        viewport * (NIKE_CAMERA_SERVICE->getCameraHeight() / viewport.y),
                        0.0f,
                        true));
            }

            if (!NIKE_METADATA_SERVICE->getEntityByName(resume)) {
                // Create Resume button
                NIKE_UI_SERVICE->createButton(resume,
                    Transform::Transform(Vector2f(-10.0f, 120.0f), Vector2f(275.0f, 55.0f), 0.0f, true),
                    Render::Text(),
                    Render::Texture("UI_ResumeGame_spritesheet.png", Vector4f(), true, 0.0f, false, Vector2i(7, 1)));
                NIKE_UI_SERVICE->setButtonInputState(resume, UI::InputStates::PRESSED);
                auto resume_hover_script = Lua::Script();
                resume_hover_script.script_id = "menu_button.lua";
                resume_hover_script.update_function = "HoverButton";
                resume_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
                NIKE_UI_SERVICE->setButtonScript(resume, resume_hover_script, "OnHover");

                auto resume_click_script = Lua::Script();
                resume_click_script.script_id = "ChangeScene.lua";
                resume_click_script.update_function = "Resume";
                NIKE_UI_SERVICE->setButtonScript(resume, resume_click_script, "OnClick");
            }


            if (!NIKE_METADATA_SERVICE->getEntityByName(options)) {
                // Create Option button

                NIKE_UI_SERVICE->createButton(options,
                    Transform::Transform(Vector2f(-10.0f, 45.0f), Vector2f(210.0f, 55.0f), 0.0f, true),
                    Render::Text(),
                    Render::Texture("UI_Options_spritesheet.png", Vector4f(), true, 0.0f, false, Vector2i(7, 1)));
                NIKE_UI_SERVICE->setButtonInputState(options, UI::InputStates::TRIGGERED);

                auto options_hover_script = Lua::Script();
                options_hover_script.script_id = "menu_button.lua";
                options_hover_script.update_function = "HoverButton";
                options_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
                NIKE_UI_SERVICE->setButtonScript(options, options_hover_script, "OnHover");
            }

            if (!NIKE_METADATA_SERVICE->getEntityByName(how_to_play)) {
                // Create How to play button
                NIKE_UI_SERVICE->createButton(how_to_play,
                    Transform::Transform(Vector2f(-10.0f, -25.0f), Vector2f(275.0f, 55.0f), 0.0f, true),
                    Render::Text(),
                    Render::Texture("UI_HowToPlayButton_Spritesheet.png", Vector4f(), true, 0.0f, false, Vector2i(7, 1)));

                NIKE_UI_SERVICE->setButtonInputState(how_to_play, UI::InputStates::TRIGGERED);

                auto how_to_play_hover_script = Lua::Script();
                how_to_play_hover_script.script_id = "menu_button.lua";
                how_to_play_hover_script.update_function = "HoverButton";
                how_to_play_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
                NIKE_UI_SERVICE->setButtonScript(how_to_play, how_to_play_hover_script, "OnHover");

            }
            if (!NIKE_METADATA_SERVICE->getEntityByName(quit)) {
                // Create Quit button
                NIKE_UI_SERVICE->createButton(quit,
                    Transform::Transform(Vector2f(-10.0f, -95.0f), Vector2f(245.0f, 55.0f), 0.0f, true),
                    Render::Text(),
                    Render::Texture("UI_QuitButton_Spritesheet.png", Vector4f(), true, 0.0f, false, Vector2i(7, 1)));

                NIKE_UI_SERVICE->setButtonInputState(quit, UI::InputStates::TRIGGERED);

                auto quit_hover_script = Lua::Script();
                quit_hover_script.script_id = "menu_button.lua";
                quit_hover_script.update_function = "HoverButton";
                quit_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
                auto quit_script = Lua::Script();
                quit_script.script_id = "ChangeScene.lua";
                quit_script.update_function = "Quit";

                NIKE_UI_SERVICE->setButtonScript(quit, quit_hover_script, "OnHover");
                NIKE_UI_SERVICE->setButtonScript(quit, quit_script, "OnClick");


            }

        }

        void togglePauseOverlay(bool show)
        {

            // Boolean of show menu will be toggled here
            show_pause_menu = show;

            const std::vector<std::string> elements = {
                "Paused_UI.png", "Resume", "Settings", "How_To_Play", "Quit"
            };

            for (const auto& element : elements) {
                auto entity = NIKE_METADATA_SERVICE->getEntityByName(element);
                if (!entity.has_value()) continue;

                auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity.value());
                // Change opacity for all buttons here
                if (texture_comp.has_value())
                {
                    texture_comp.value().get().color.a = show ? 1.0f : 0.0f;
                }
                
                if (element != "Paused_UI.png")
                {
                    NIKE_UI_SERVICE->setButtonDisabled(element, !show);
                }
            }
        }

        void gameOverlay(const std::string& background_texture, const std::string& play_again, const std::string& quit_game_text)
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
            auto play_again_hover_script = Lua::Script();
            play_again_hover_script.script_id = "menu_button.lua";
            play_again_hover_script.update_function = "HoverButton";
            play_again_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
            auto play_again_script = Lua::Script();
            play_again_script.script_id = "ChangeScene.lua";
            play_again_script.update_function = "Restart";
            NIKE_UI_SERVICE->setButtonScript(play_again, play_again_hover_script, "OnHover");
            NIKE_UI_SERVICE->setButtonScript(play_again, play_again_script, "OnClick");


            auto quit_hover_script = Lua::Script();
            quit_hover_script.script_id = "menu_button.lua";
            quit_hover_script.update_function = "HoverButton";
            quit_hover_script.named_args["audio"] = std::string("MenuHoverOverSFX.wav");
            auto quit_script = Lua::Script();
            quit_script.script_id = "ChangeScene.lua";
            quit_script.update_function = "Quit";
            NIKE_UI_SERVICE->setButtonScript(quit_game_text, quit_hover_script, "OnHover");
            NIKE_UI_SERVICE->setButtonScript(quit_game_text, quit_script, "OnClick");
        }

        /*****************************************************************//**
         * VFX and SFX Interactions
         *********************************************************************/

        /* TODO, remove or edit call */
        void playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
        {
            auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
            if (e_sfx_comp.has_value()) {
                auto& e_sfx = e_sfx_comp.value().get();

                //Check if group exists
                auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
                if (!group) {
                    e_sfx.audio_id = "EnemyGetHit2.wav"; // Placeholder
                    e_sfx.b_play_sfx = play_or_no;
                    return;
                }
                else {
                    //Play sound
                    if (play_or_no && !group->isPlaying()) {
                        e_sfx.audio_id = "EnemyGetHit2.wav";
                        e_sfx.b_play_sfx = play_or_no;
                        return;
                    }
                }

                //stop sfx
                if (!play_or_no) {
                    group->stop();
                }
            }
        }

        void animationSet(Entity::Type const& entity, int start_x, int start_y, int end_x, int end_y)
        {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
            if (e_animate_comp.has_value() && e_base_comp.has_value()) {
                auto& e_animate = e_animate_comp.value().get();
                auto& e_base = e_base_comp.value().get();

                //Boolean to check for changes
                bool changed = false;

                //Save prev start
                static Vector2i prev_start = e_animate.start_index;
                //Save prev end
                static Vector2i prev_end = e_animate.end_index;

                // Force start the animation
                e_base.animation_mode = Animation::Mode::RESTART;

                //Change animation
                if (prev_start != Vector2i(start_x, start_y) || e_base.animation_mode == Animation::Mode::END) {
                    e_animate.start_index.x = start_x;
                    e_animate.start_index.y = start_y;
                    prev_start = e_animate.start_index;

                    changed = true;
                }



                //Change animation
                if (prev_end != Vector2i(end_x, end_y) || e_base.animation_mode == Animation::Mode::END) {
                    e_animate.end_index.x = end_x;
                    e_animate.end_index.y = end_y;
                    prev_end = e_animate.end_index;

                    changed = true;
                }

                //If variables changed
                if (changed) {

                    //Restart animation
                    e_base.animations_to_complete = 0;
                    e_base.animation_mode = Animation::Mode::RESTART;
                }

                // Reset the start and end index of comp
                e_animate.start_index = prev_start;
                e_animate.end_index = prev_end;
            }
        }

        void flipX(Entity::Type const& entity, bool yes_or_no)
        {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                if (e_texture_comp.value().get().b_flip.x != yes_or_no)
                {
                    e_texture_comp.value().get().b_flip.x = yes_or_no;
                }
            }
        }

        void flipY(Entity::Type const& entity, bool yes_or_no)
        {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                if (e_texture_comp.value().get().b_flip.y != yes_or_no)
                {
                    e_texture_comp.value().get().b_flip.y = yes_or_no;
                }
            }
        }

        void setLastDirection(Entity::Type const& entity, int dir)
        {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                e_physics_comp.value().get().last_direction = dir;
            }
        }

        int getLastDirection(Entity::Type const& entity)
        {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                return e_physics_comp.value().get().last_direction;
            }
            // Return a rand value when cnt retrieve last dir
            return INT_MAX;
        }


        void spawnHealAnimation(const Entity::Type player) {
            // Get player position
            const auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
            if (!player_transform_comp.has_value()) {
                NIKEE_CORE_WARN("spawnEnemy: PLAYER missing Transform Component, heal animation not playing");
                return;
            }

            const Vector2f& player_pos = player_transform_comp.value().get().position;

            // Create heal animation entity
            Entity::Type anim_entity = NIKE_ECS_MANAGER->createEntity();

            // Load from prefab
            NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(anim_entity, "healAnimation.prefab");

            // Add tag to animation
            if (NIKE_METADATA_SERVICE->isTagValid("healAnimation")) {
                NIKE_METADATA_SERVICE->addEntityTag(anim_entity, "healAnimation");
            }

            // Set animation position to player position
            auto anim_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(anim_entity);
            if (anim_transform_comp.has_value()) {
                anim_transform_comp.value().get().position = { player_pos };
            }
        }


        /*****************************************************************//**
         * Player Interactions
         *********************************************************************/

        void restoreHealth(Entity::Type healer, Entity::Type target) {
            const auto healer_heal_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::HealthDrop>(healer);
            const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(target);
            const auto& healer_heal = healer_heal_comp.value().get().heal_amount;

            // Return if target has no health
            if (target_health_comp.has_value() == false) {
                return;
            }

            auto& target_health = target_health_comp.value().get().health;
            const auto& target_max_health = target_health_comp.value().get().max_health;

            // Heal Target
            // (The check might be redundant now as there is another check in sysCollision)
            if (target_health < target_max_health) {
                // Spawn health animation
                // Check no health animation tag exists
                // If no health animation tag
                spawnHealAnimation(target);

                target_health += healer_heal;
                // Temporary hardcoded SFX
                NIKE_AUDIO_SERVICE->playAudio("HealSFX.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
            }
        }

        void handleCollision(Entity::Type entity_a, Entity::Type entity_b) {
            // Collision between damage and health
            const auto a_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity_a);
            const auto b_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity_b);

            if (a_damage_comp.has_value() || b_damage_comp.has_value()) {
                const auto a_faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity_a);
                const auto b_faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity_b);

                // Friendly fire check
                //if (a_faction_comp.has_value() && b_faction_comp.has_value()) {
                //    const auto& a_faction = a_faction_comp.value().get().faction;
                //    const auto& b_faction = b_faction_comp.value().get().faction;

                //    //NIKEE_CORE_WARN("{} and {} == {}", static_cast<int>(a_faction), static_cast<int>(b_faction), a_faction == b_faction);

                //    if (a_faction == b_faction) {
                //        return;
                //    }
                //}

                // Applying Damage
                if (a_damage_comp.has_value()) applyDamage(entity_a, entity_b);
                if (b_damage_comp.has_value()) applyDamage(entity_b, entity_a);
            }

            // Collision between health drop and health
            const auto a_health_drop_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::HealthDrop>(entity_a);
            const auto b_health_drop_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::HealthDrop>(entity_b);

            if (a_health_drop_comp.has_value() || b_health_drop_comp.has_value()) {
                if (a_health_drop_comp.has_value()) restoreHealth(entity_a, entity_b);
                if (b_health_drop_comp.has_value()) restoreHealth(entity_b, entity_a);
            }
        }


        void applyDamage(Entity::Type attacker, Entity::Type target) {
            const auto attacker_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(attacker);
            const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(target);
            const auto attacker_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(attacker);
            const auto target_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(target);

            // Return if no damage comp and health comp
            if (!(attacker_damage_comp.has_value() && target_health_comp.has_value())) {
                return;
            }

            auto& target_health = target_health_comp.value().get();
            auto& attacker_damage = attacker_damage_comp.value().get().damage;

            // Check if target is in HurtState
            //auto state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(target);
            //if (state_comp.has_value()) {
            //    auto& state = state_comp.value().get();
            //    auto current_state = state.current_state.lock();

            //    // If entity is already in HurtState, skip damage application
            //    if (current_state == NIKE_FSM_SERVICE->getStateByID("EnemyHurt")) {
            //        return;  // Skip applying damage if in HurtState
            //    }
            //}

            // Check invulnerability flag
            if (target_health.invulnerable_flag) {
                return; // Skip damage
            }

            // When target minus, boolean here will set to true
            if (!target_health.taken_damage)
            {
                target_health.taken_damage = true;
            }

            // Default dmg multiplier
            float multiplier = 1.f;

            // Apply elemental damage multiplier
            if (attacker_element_comp) {
                const auto attacker_element = attacker_element_comp.value().get().element;

                if (target_element_comp) {
                    const auto target_element = target_element_comp.value().get().element;

                    // Set multiplier
                    multiplier = Element::getElementMultiplier(attacker_element, target_element);
                }

                // If damage dealer has element comp & target has combo comp
                const auto target_combo_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Combo>(target);
                if (target_combo_comp) {
                    // Update combo component
                    target_combo_comp.value().get().registerHit(attacker_element);
                }
            }

            // Apply damage
            target_health.health -= (attacker_damage * multiplier);
            NIKEE_CORE_INFO("Entity {} took {} damage from Entity {}. Remaining health: {}",
                target, attacker_damage, attacker, target_health.health);
            // Check target type (currently for enemy SFX)
            const auto& target_entity_tags = NIKE_METADATA_SERVICE->getEntityTags(target);
            if ( (target_entity_tags.find("enemy") != target_entity_tags.end()) && (target_entity_tags.find("boss") == target_entity_tags.end()) ) // Only identify enemy, not boss
            {
                // Temporary hardcoded SFX
                int randomVariant = NIKE::GameLogic::getRandomNumber<int>(1, 5); // Assuming variations 1-5
                std::string sfxToPlay = "Enemy_Pawn_Hit_0" + std::to_string(randomVariant) + ".wav";
                NIKE_AUDIO_SERVICE->playAudio(sfxToPlay, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), 0.5f * NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
                // Set entity hit boolean to true
                hitEntities[target] = true;

                // Set intensity to max (1.0f) when hit
                auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(target);
                if (!texture_comp) return;

                texture_comp.value().get().intensity = 1.0f;

            }
            else if ((target_entity_tags.find("enemy") != target_entity_tags.end())) { // Entity is a boss
                int randomVariantInteract = NIKE::GameLogic::getRandomNumber<int>(1, 5); // Assuming variations 1-5
                std::string sfxToPlay = "Enemy_Guard_Hit_0" + std::to_string(randomVariantInteract) + ".wav";
                NIKE_AUDIO_SERVICE->playAudio(sfxToPlay, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), 0.6f * NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);

                // Set entity hit boolean to true
                hitEntities[target] = true;

                // Set intensity to max (1.0f) when hit
                auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(target);
                if (!texture_comp) return;

                texture_comp.value().get().intensity = 1.0f;

            }
            else if (target_entity_tags.find("objects") != target_entity_tags.end()) { // Entity is an object
                NIKE_AUDIO_SERVICE->playAudio("MetalHit1.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);


                // Set entity hit boolean to true
                hitEntities[target] = true;

                // Set intensity to max (1.0f) when hit
                auto texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(target);
                if (!texture_comp) return;

                texture_comp.value().get().intensity = 1.0f;

            }

          

            //Apply hurt animation
            auto base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(target);
            auto sprite_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(target);
            auto tags = NIKE_METADATA_SERVICE->getEntityTags(target);
            if (base_comp.has_value() && sprite_comp.has_value()) {

                // If entities are a not boss
                if (tags.find("boss") == tags.end())
                {
                    auto& base = base_comp.value().get();
                    auto& sprite = sprite_comp.value().get();

                    //Set base
                    base.animations_to_complete = 3;
                    base.animation_mode = Animation::Mode::RESTART;
                    base.frame_duration = 0.08f;

                    //Set sprite
                    sprite.start_index = { 0, 12 };
                    sprite.end_index = { 1, 12 };
                    sprite.curr_index = sprite.start_index;
                }
                // If entity is boss
                else if (tags.find("enemy") != tags.end() && tags.find("boss") != tags.end())
                {
                    // Get entity current state
                    auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<NIKE::State::State>(target);
                    auto e_elem_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(target);
                    auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(target);

                    if (!e_state_comp.has_value())
                    {
                        NIKEE_CORE_WARN("error! no state comp");
                        return;
                    }

                    if (!e_elem_comp.has_value())
                    {
                        NIKEE_CORE_WARN("error! no element comp");
                        return;
                    }

                    if (!e_texture_comp.has_value())
                    {
                        NIKEE_CORE_WARN("error! no texture comp");
                        return;
                    }

                    // Get element string 
                    std::string element_string = Element::getElementString(e_elem_comp.value().get().element);
                    // Set texture here
                    std::string spritesheet = State::getSpriteSheet("BossHurt", element_string);
                    if (NIKE_ASSETS_SERVICE->isAssetRegistered(spritesheet))
                    {
                        e_texture_comp.value().get().texture_id = spritesheet;
                    }
                    auto& base = base_comp.value().get();
                    auto& sprite = sprite_comp.value().get();

                    //Set base
                    base.animations_to_complete = 3;
                    base.animation_mode = Animation::Mode::RESTART;
                    base.frame_duration = 0.08f;

                    //Set sprite
                    sprite.start_index = { 0, 0 };
                    sprite.end_index = { 7, 0 };
                    sprite.curr_index = sprite.start_index;
                }

            }

            // Check if target health drops to zero or below
            if (target_health.health <= 0) {
                // Target has more than 1 life
                --target_health.lives;
                target_health.health = target_health.max_health;
                NIKEE_CORE_INFO("Entity {} lost 1 life.", target);
            }
        }

        /*****************************************************************//**
         * Environment Interactions
         *********************************************************************/

        void changeElement(Entity::Type player, Entity::Type source) {
            const auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player);
            const auto source_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Source>(source);
            const auto player_faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(player);

            // Prevent enemies from switching elements
            if (player_faction_comp.has_value() && player_faction_comp.value().get().faction != Combat::Factions::PLAYER) {
                return;
            }

            if (player_element_comp.has_value() && source_element_comp.has_value()) {
                auto& player_element = player_element_comp.value().get().element;
                auto& source_element = source_element_comp.value().get().element;

                // Set player element to source element
                player_element = source_element;

                // Elemental UI removed and added back to update

                // !TODO: Play element change animation here
            }

       
        }


        bool isWithinWorldRange(Entity::Type source, Entity::Type player) {
            // Get player transform
            auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player);
            Vector2f player_pos = player_transform_comp.value().get().position;
            Vector2f player_scale = player_transform_comp.value().get().scale;

            // Get source transform
            auto source_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(source);
            Vector2f source_pos = source_transform_comp.value().get().position;
            Vector2f source_scale = source_transform_comp.value().get().scale;

            // Set source range
            float source_range = 2;

            // Calculations
            float avg_scale_x = (source_scale.x + player_scale.x) / 2;
            float avg_scale_y = (source_scale.y + player_scale.y) / 2;

            float dist_x = (source_pos.x - player_pos.x) / avg_scale_x;
            float dist_y = (source_pos.y - player_pos.y) / avg_scale_y;

            float distance = (dist_x * dist_x) + (dist_y * dist_y);

            //NIKEE_CORE_INFO("Distance = {}, source Range = {}", distance, source_range);

            // It is recommended to use source_range^2, but it's probably easier this way
            return distance < source_range;
        }
    }
}