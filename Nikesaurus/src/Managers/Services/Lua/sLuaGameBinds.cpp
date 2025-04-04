/*****************************************************************//**
 * \file   sLuaGameBinds.cpp
 * \brief  Lua Game bindings
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (100%)
 * \date   March 2025
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Lua/sLuaGameBinds.h"

#include "Systems/GameLogic/sysInteraction.h"
#include "Systems/GameLogic/sysGameLogic.h"

namespace NIKE {
    const float Lua::player_shot_cooldown = 0.1f;
    float Lua::player_last_shot_time = player_shot_cooldown;

    void Lua::luaGameBinds(sol::state& lua_state) {
        ////Register destroy entity
        //lua_state.set_function("KillEntity", [&](Entity::Type entity) {
        //    NIKE_METADATA_SERVICE->destroyEntity(entity);
        //    });


        /*****************************************************************//**
         * Physics
         *********************************************************************/

         //Apply force to entities
        lua_state.set_function("ApplyForce", [&](Entity::Type entity, float x, float y) {
            auto e_trans_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_trans_comp.has_value()) {
                e_trans_comp.value().get().force.x = x;
                e_trans_comp.value().get().force.y = y;
            }
            });

        // Bind delta time getter (if needed)
        lua_state.set_function("GetDeltaTime", []() {
            return NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
            });

        ////Get fixed delta time
        //lua_state.set_function("GetFixedDeltaTime", [&]()-> float {
        //    return NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
        //    }
        //);

        //Get Velocity
        lua_state.set_function("Velocity", [&](Entity::Type entity) -> sol::optional<Vector2f> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().velocity;
            }
            else {
                return sol::optional<Vector2f>();
            }
            });

        //Get Net Velocity
        lua_state.set_function("NetVelocity", [&](Entity::Type entity) -> sol::optional<float> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().velocity.length();
            }
            else {
                return sol::optional<float>();
            }
            });


        /*****************************************************************//**
         * Visuals
         *********************************************************************/

         //Change animation start & end
        lua_state.set_function("AnimationSet", [&](Entity::Type entity, int start_x, int start_y, int end_x, int end_y) {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
            if (e_animate_comp.has_value() && e_base_comp.has_value()) {
                auto& e_animate = e_animate_comp.value().get();
                auto& e_base = e_base_comp.value().get();

                //Boolean to check for changes
                bool changed = false;

                //Save prev start
                static Vector2i prev_start = e_animate.start_index;

                //Change animation
                if (prev_start != Vector2i(start_x, start_y) || e_base.animation_mode == Animation::Mode::END) {
                    e_animate.start_index.x = start_x;
                    e_animate.start_index.y = start_y;
                    prev_start = e_animate.start_index;

                    changed = true;
                }

                //Save prev end
                static Vector2i prev_end = e_animate.end_index;

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
            }
            });

        //Change animation variables
        lua_state.set_function("SetAnimationVariables", [&](Entity::Type entity, int start_x, int start_y, int end_x, int end_y, int curr_x, int curr_y) {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
            if (e_animate_comp.has_value() && e_base_comp.has_value()) {
                auto& e_animate = e_animate_comp.value().get();

                e_animate.start_index.x = start_x;
                e_animate.start_index.y = start_y;
                e_animate.end_index.x = end_x;
                e_animate.end_index.y = end_y;
                e_animate.curr_index.x = curr_x;
                e_animate.curr_index.y = curr_y;
            }
            });

        //Get Last Direction (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("AnimationCompleted", [&](Entity::Type entity) -> sol::optional<int> {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
            if (e_animate_comp.has_value()) {
                return e_animate_comp.value().get().completed_animations;
            }
            else {
                return sol::optional<int>();
            }
            });

        //Flip texture
        lua_state.set_function("FlipX", [&](Entity::Type entity, bool yes_or_no) {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                e_texture_comp.value().get().b_flip.x = yes_or_no;
            }
            });

        lua_state.set_function("FlipY", [&](Entity::Type entity, bool yes_or_no) {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                e_texture_comp.value().get().b_flip.y = yes_or_no;
            }
            });

        //Apply force to entities (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("SetLastDirection", [&](Entity::Type entity, int dir) {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                e_physics_comp.value().get().last_direction = dir;
            }
            });

        //Get Last Direction (COMPONENT TO BE MOVED ELSE WHERE)
        lua_state.set_function("LastDirection", [&](Entity::Type entity) -> sol::optional<int> {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                return e_physics_comp.value().get().last_direction;
            }
            else {
                return sol::optional<int>();
            }
            });

        /*****************************************************************//**
         * Game Logic
         *********************************************************************/

        lua_state.set_function("SetState", [&](Entity::Type entity, std::string state) {
            auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
            if (e_state_comp.has_value()) {
                e_state_comp.value().get().state_id = state;
            }
            });

        lua_state.set_function("State", [&](Entity::Type entity) -> sol::optional<std::string> {
            auto e_state_comp = NIKE_ECS_MANAGER->getEntityComponent<State::State>(entity);
            if (e_state_comp.has_value()) {
                return e_state_comp.value().get().state_id;
            }
            else {
                return sol::optional<std::string>();
            }
            });

        // Play SFX (To edit)
        lua_state.set_function("PlaySFX", [&](Entity::Type entity, bool play_or_stop) {
            auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
            if (e_sfx_comp.has_value()) {
                auto& e_sfx = e_sfx_comp.value().get();

                //Check if group exists
                auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
                if (!group) {
                    e_sfx.b_play_sfx = play_or_stop;
                    return;
                }
                else {
                    //Play sound
                    if (play_or_stop && !group->isPlaying()) {
                        e_sfx.b_play_sfx = play_or_stop;
                    }
                }

                //stop sfx
                if (!play_or_stop) {
                    group->stop();
                }
            }
            });

        //Fire Bullet
        lua_state.set_function("FireBullet", [&](Entity::Type entity) {

            Entity::Type bullet_entity = NIKE_ECS_MANAGER->createEntity();

            // Player Element Type
            auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(entity);
            if (player_element_comp.has_value()) {
                // Shoot elemental bullet
                NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, Element::playerBullet[static_cast<int>(player_element_comp.value().get().element)]);
            }
            else {
                // Missing Element Comp
                NIKEE_CORE_WARN("PLAYER missing Elemental Component");
                NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(bullet_entity, "bullet.prefab");
            }

            //Player position
            auto player_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (!player_transform_comp.has_value()) {
                return;
            }

            //Get player transform
            auto const& player_transform = player_transform_comp.value().get();

            //Calculate direction vector (mouse - player)
            float direction = atan2((-(NIKE_INPUT_SERVICE->getMouseWorldPos().y) + player_transform.position.y), (NIKE_INPUT_SERVICE->getMouseWorldPos().x - player_transform.position.x));
            Vector2f bull_direction = { cosf(direction), -sinf(direction) };

            //Normalize direction
            if (bull_direction.length() != 0) {
                bull_direction.x /= bull_direction.length();
                bull_direction.y /= bull_direction.length();
            }

            //Bullet Speed
            int constexpr BULLET_SPEED = 100;

            //Calculate bullet force vector
            Vector2f bullet_force = bull_direction * BULLET_SPEED;

            // Apply force to the bullet
            auto bullet_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(bullet_entity);
            if (bullet_dynamics.has_value()) {
                bullet_dynamics.value().get().force = bullet_force;
            }

            //Set initial bullet position to player's position
            auto bullet_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(bullet_entity);
            if (bullet_transform.has_value()) {
                bullet_transform.value().get().position = player_transform.position + (bull_direction.normalize() * (player_transform.scale.length() * 0.25f));
            }

            //Set player bullet SFX with randomized variant
            auto bullet_sfx_opt = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(bullet_entity);
            if (bullet_sfx_opt.has_value()) {
                auto& sfx_comp = bullet_sfx_opt.value().get();
                std::string original_sfxID = sfx_comp.audio_id; // Get ID set by prefab

                std::string baseFilename = "";
                int maxVariant = 1; // Default to 1 if type unknown

                // Determine type and randomization range based on original ID
                if (original_sfxID.find("Shoot_Fire") != std::string::npos) { // Check if "Shoot_Fire" is present
                    baseFilename = "Shoot_Fire_";
                    maxVariant = 3;
                }
                else if (original_sfxID.find("Shoot_Water") != std::string::npos) {
                    baseFilename = "Shoot_Water_";
                    maxVariant = 9;
                }
                else if (original_sfxID.find("Shoot_Wind") != std::string::npos) { // Assuming "Wind" based on your list
                    baseFilename = "Shoot_Wind_";
                    maxVariant = 15;
                }
                else if (original_sfxID.find("Laser") != std::string::npos) {
                    baseFilename = "Laser";
                    maxVariant = 3;
                }
                else if (original_sfxID.find("Pop") != std::string::npos) {
                    baseFilename = "Pop";
                    maxVariant = 5;
                }
                // Add more else if checks for other bullet base names if needed

                std::string randomized_sfxID = original_sfxID; // Default to original if type unknown

                if (!baseFilename.empty()) { // If a known type was found
                    int randomVariant = NIKE::GameLogic::getRandomNumber<int>(1, maxVariant);
                    if (randomVariant < 10) {
						baseFilename = baseFilename + "0"; // Add leading zero for 2-digit numbers
                    }
                    randomized_sfxID = baseFilename + std::to_string(randomVariant) + ".wav";
                }
                else {
                    NIKEE_CORE_WARN("FireBullet: Unknown bullet SFX base type for '{}', using original ID for playback.", original_sfxID);
                }

                // Update the component's audio_id with the randomized one
                //sfx_comp.audio_id = randomized_sfxID;
                // Set the flag for sysAudio to play this sound
                //sfx_comp.b_play_sfx = true;

                // Play the determined sound directly, consistent with other SFX calls
                NIKE_AUDIO_SERVICE->playAudio(randomized_sfxID, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), sfx_comp.volume * NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), sfx_comp.pitch, false, false);
            }
            });

        //Spawn enemy function
        lua_state.set_function("Spawn Enemy", [&](float x, float y) {
            Entity::Type entity = NIKE_ECS_MANAGER->createEntity();
            NIKE_SERIALIZE_SERVICE->loadEntityFromPrefab(entity, "enemy.prefab");
            auto e_trans_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (e_trans_comp.has_value()) {
                e_trans_comp.value().get().position.x = x;
                e_trans_comp.value().get().position.y = y;
            }
            // Temporary hardcoded SFX
            NIKE_AUDIO_SERVICE->playAudio("EnemySpawn1.wav", "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
            });

        // Player EnemyDeathState annimation function
        lua_state.set_function("CheckDeath", [&](Entity::Type entity) -> bool {
            auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
            if (health_comp.has_value()) {

                // Temporary method to store static map for health (TODO, remove after optimising another way)
                auto& health = health_comp.value().get();

                // Note static
                static float prevHealth = health.health;

                // If health has decreased, play the damage-taken SFX and update prevHealth.
                if (health.health < prevHealth) {

                    // Temporary hardcoded SFX
                    int randomVariantDamage = NIKE::GameLogic::getRandomNumber<int>(1, 12); // Assuming variations 1-12
                    std::string sfxToPlay = "";
					if (randomVariantDamage < 10) {
						// Add leading zero for 2-digit numbers
						sfxToPlay = "Cat3High_Hurt_0" + std::to_string(randomVariantDamage) + ".wav";
					}
					else {
						sfxToPlay = "Cat3High_Hurt_" + std::to_string(randomVariantDamage) + ".wav";
					}
                    NIKE_AUDIO_SERVICE->playAudio(sfxToPlay, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
                    prevHealth = health.health;
                }
                else {
                    // If health has increased (e.g., healing) update prevHealth.
                    prevHealth = health.health;
                }

                // When player do not have any health
                if (health_comp.value().get().lives <= 0)
                {
                    // Temporary hardcoded SFX
                    int randomVariantDeath = NIKE::GameLogic::getRandomNumber<int>(1, 7); // Assuming variations 1-7
                    std::string sfxToPlay = "Cat3High_Death_0" + std::to_string(randomVariantDeath) + ".wav";
                    NIKE_AUDIO_SERVICE->playAudio(sfxToPlay, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);

                    Interaction::gameOverlay("Defeat_screen_bg.png", "Play Again", "Quit");
                    // Delay for 0.5 seconds using engine's delta time (careful busy-wait loop)
                    float secondsToDelay = 0.5f;
                    float currentDelay = 0.0f;
                    while (currentDelay < secondsToDelay) {
                        currentDelay += NIKE_WINDOWS_SERVICE->getDeltaTime();
                    }

                    NIKE_METADATA_SERVICE->destroyEntity(entity);
                    return true;
                }
            }
            return false;
            });

        // Bind player shot cooldown
        lua_state.set_function("GetShotCooldown", []() {
            return player_shot_cooldown;
            });

        // Bind last shot time getter
        lua_state.set_function("GetLastShotTime", []() {
            return player_last_shot_time;
            });

        // Bind last shot time setter
        lua_state.set_function("SetLastShotTime", [](float time) {
            player_last_shot_time = time;
            });

        /*****************************************************************//**
         * Audio
         *********************************************************************/

         // Bind slider arrow functions for adjusting global BGM volume.
        lua_state.set_function("SliderArrowLeft", [&]() {
            float currentVolume = NIKE_AUDIO_SERVICE->getGlobalBGMVolume();
            float newVolume = Utility::getMax(0.0f, currentVolume - 0.05f);
            NIKE_AUDIO_SERVICE->setGlobalBGMVolume(newVolume);
            NIKE_UI_SERVICE->updateVolumeSliderPositions();
            });

        lua_state.set_function("SliderArrowRight", [&]() {
            float currentVolume = NIKE_AUDIO_SERVICE->getGlobalBGMVolume();
            float newVolume = Utility::getMin(1.0f, currentVolume + 0.05f);
            NIKE_AUDIO_SERVICE->setGlobalBGMVolume(newVolume);
            NIKE_UI_SERVICE->updateVolumeSliderPositions();
            });

        // Bind slider arrow functions for adjusting global SFX volume.
        lua_state.set_function("SfxSliderArrowLeft", [&]() {
            float currentVolume = NIKE_AUDIO_SERVICE->getGlobalSFXVolume();
            float newVolume = Utility::getMax(0.0f, currentVolume - 0.05f);
            NIKE_AUDIO_SERVICE->setGlobalSFXVolume(newVolume);
            NIKE_UI_SERVICE->updateVolumeSliderPositions();
            });

        lua_state.set_function("SfxSliderArrowRight", [&]() {
            float currentVolume = NIKE_AUDIO_SERVICE->getGlobalSFXVolume();
            float newVolume = Utility::getMin(1.0f, currentVolume + 0.05f);
            NIKE_AUDIO_SERVICE->setGlobalSFXVolume(newVolume);
            NIKE_UI_SERVICE->updateVolumeSliderPositions();
            });

        //Play audio
        lua_state.set_function("PlayAudio", [&](std::string const& audio_id) {
            NIKE_AUDIO_SERVICE->playAudio(audio_id, "", NIKE_AUDIO_SERVICE->getSFXChannelGroupID(), NIKE_AUDIO_SERVICE->getGlobalSFXVolume(), 1.f, false, false);
            });

        // Set SFX list from script
        lua_state.set_function("SetAdditionalSFX", [&](Entity::Type entity, sol::table sfxTable) {
            auto compOpt = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
            if (compOpt.has_value()) {
                auto& comp = compOpt.value().get();
                comp.sfx_list.clear();
                for (auto& kv : sfxTable) {
                    // Each element in sfxTable should be a string.
                    std::string sfxName = kv.second.as<std::string>();
                    // Only add if sfxName is not already present.
                    if (std::find(comp.sfx_list.begin(), comp.sfx_list.end(), sfxName) == comp.sfx_list.end()) {
                        comp.sfx_list.push_back(sfxName);
                    }
                }
            }
            });

        /*****************************************************************//**
         * Misc
         *********************************************************************/

         // Set/Teleport entity
        lua_state.set_function("SetPosition", [&](Entity::Type entity, float x, float y) {
            auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (transform.has_value()) {
                transform.value().get().position = { x, y };
            }
            });

        // God mode toggle
        lua_state.set_function("SetGodMode", [&](Entity::Type entity, bool enable) {
            auto health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(entity);
            if (health_comp) {
                health_comp.value().get().invulnerable_flag = enable;
                if (health_comp.value().get().invulnerable_flag) {
                    cout << "Player god mode enabled" << endl;
                }
                else {
                    cout << "Player god mode disabled" << endl;
                }
            }
            });

        // High Damage toggle
        lua_state.set_function("SetHighDamage", [&](Entity::Type entity, bool enable) {
            auto damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity);
            if (damage_comp) {
                if (enable) {
                    damage_comp.value().get().damage = 9999.0f; // High damage
                    cout << "Damage set max" << endl;
                }
                else {
                    damage_comp.value().get().damage = 1.0f; // Default damage
                    cout << "Damage set default 1" << endl;
                }
            }
            });

        //Path finding
        lua_state.set_function("PathFind", [&](Entity::Type entity, int x_index, int y_index, float speed) {

            //Acceptable offset per cell
            const float cell_offset = 10.0f;

            //Get transform of entity for position mapping
            auto transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            if (transform.has_value()) {

                //Entity transform
                auto& e_transform = transform.value().get();

                //Get index of entity as the starting position
                auto start = NIKE_MAP_SERVICE->getCellIndexFromCords(e_transform.position);

                //Get cell to travel to
                if (start.has_value()) {

                    //Get start index
                    auto start_index = start.value();

                    //Check if path has been generated or if destination cell has changed
                    if (!NIKE_MAP_SERVICE->checkPath(entity) ||
                        NIKE_MAP_SERVICE->checkGridChanged() ||
                        (NIKE_MAP_SERVICE->getPath(entity).goal.index != Vector2i(x_index, y_index)) ||
                        (!NIKE_MAP_SERVICE->getPath(entity).path.empty() &&
                            (std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.x - start_index.x) > 1 ||
                                std::abs(NIKE_MAP_SERVICE->getPath(entity).path.front().index.y - start_index.y) > 1)) ||
                        (NIKE_MAP_SERVICE->getPath(entity).b_finished && start_index != NIKE_MAP_SERVICE->getPath(entity).end.index)
                        ) {

                        //Search for path
                        NIKE_MAP_SERVICE->findPath(entity, start_index, Vector2i(x_index, y_index));
                    }

                    //Get path 
                    auto path = NIKE_MAP_SERVICE->getPath(entity);

                    //Check if there are cells left in path
                    if (!path.path.empty()) {

                        //Get next cell
                        auto const& next_cell = path.path.front();

                        //Check if entity has arrived near destination
                        if ((next_cell.position - e_transform.position).length() > cell_offset) {

                            //Direction of next cell
                            float dir = atan2((next_cell.position.y - e_transform.position.y), (next_cell.position.x - e_transform.position.x));

                            //Apply force to entity
                            auto dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                            if (dynamics.has_value()) {
                                dynamics.value().get().force = { cos(dir) * speed, sin(dir) * speed };
                            }
                        }
                        else {
                            path.path.pop_front();
                        }
                    }
                    else {
                        //Marked path as finished
                        path.b_finished = true;
                    }
                }
            }
            });
    }
}
