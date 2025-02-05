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

namespace NIKE {
    namespace Interaction {

        void Manager::init() {
          
        }

        void Manager::update() {
            // Get layers
            auto& layers = NIKE_SCENES_SERVICE->getLayers();

            // Reverse iterate through layers
            for (auto layer = layers.rbegin(); layer != layers.rend(); layer++) {

                //Skip inactive layers
                if (!(*layer)->getLayerState())
                    continue;

                // Iterate through all entities
                for (auto& entity : (*layer)->getEntitites()) {

                    //Skip entity not registered to this system
                    if (entities.find(entity) == entities.end()) continue;

                    if (NIKE_ECS_MANAGER->checkEntity(entity)) {

                        // Check for Elemental Source component
                        const auto e_source_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Source>(entity);
                        if (e_source_comp.has_value()) {

                            // Look for entity with player component
                            for (auto& other_entity : entities) {
                                auto e_player_comp = NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(other_entity);

                                if (e_player_comp.has_value()) { // Player entity exists

                                    // Get Render Component
                                    const auto source_render_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
                                    auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
                                    //float& source_intensity = source_render_comp.value().get().intensity;
                                    Vector4f& source_alpha = source_render_comp.value().get().color;

                                    float target_alpha = withinRange(entity, other_entity) ? 1.0f : 0.0f; // Set target alpha
                                    float alpha_speed = 10.0f * NIKE_WINDOWS_SERVICE->getDeltaTime(); // Adjust based on deltaTime

                                    // Smoothly interpolate alpha
                                    source_alpha.a += (target_alpha - source_alpha.a) * alpha_speed;

                                    // Clamp alpha between 0 and 1.0
                                    source_alpha.a = std::clamp(source_alpha.a, 0.0f, 1.0f);

                                    // Player Element Swapping
                                    if (withinRange(entity, other_entity) && NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_E)) {
                                        changeElement(other_entity, entity);
                                        if (e_sfx_comp.has_value()) {
                                            e_sfx_comp.value().get().b_play_sfx = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        void playSFX([[maybe_unused]] Entity::Type& entity, [[maybe_unused]] bool play_or_no)
        {
            auto e_sfx_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(entity);
            if (e_sfx_comp.has_value()) {
                auto& e_sfx = e_sfx_comp.value().get();

                //Check if group exists
                auto group = NIKE_AUDIO_SERVICE->getChannelGroup(e_sfx.channel_group_id);
                if (!group) {
                    e_sfx.audio_id = "EnemyGetHit.wav";
                    e_sfx.b_play_sfx = play_or_no;
                    return;
                }
                else {
                    //Play sound
                    if (play_or_no && !group->isPlaying()) {
                        e_sfx.audio_id = "EnemyGetHit.wav";
                        e_sfx.b_play_sfx = play_or_no;
                    }
                }

                //stop sfx
                if (!play_or_no) {
                    group->stop();
                }
            }
        }

        void animationHurtStart(Entity::Type& entity, int start_x, int start_y)
        {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            if (e_animate_comp.has_value()) {
                //static Vector2i prev_start = e_animate_comp.value().get().start_index;

                if (e_animate_comp.value().get().start_index != Vector2i(start_x, start_y)) {
                    e_animate_comp.value().get().start_index.x = start_x;
                    e_animate_comp.value().get().start_index.y = start_y;
                    //prev_start = e_animate_comp.value().get().start_index;

                    //Restart animation
                    auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
                    if (e_base_comp.has_value()) {
                        e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
                    }
                }
            }
        }

        void animationHurtEnd(Entity::Type& entity, int end_x, int end_y)
        {
            auto e_animate_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Sprite>(entity);
            if (e_animate_comp.has_value()) {
                //static Vector2i prev_end = e_animate_comp.value().get().end_index;

                if (e_animate_comp.value().get().end_index != Vector2i(end_x, end_y)) {
                    e_animate_comp.value().get().end_index.x = end_x;
                    e_animate_comp.value().get().end_index.y = end_y;
                    //prev_end = e_animate_comp.value().get().end_index;

                    //Restart animation
                    auto e_base_comp = NIKE_ECS_MANAGER->getEntityComponent<Animation::Base>(entity);
                    if (e_base_comp.has_value()) {
                        e_base_comp.value().get().animation_mode = Animation::Mode::RESTART;
                    }
                }
            }
        }

        void flipX(Entity::Type& entity, bool yes_or_no)
        {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                if (e_texture_comp.value().get().b_flip.x != yes_or_no)
                {
                    e_texture_comp.value().get().b_flip.x = yes_or_no;
                }
            }
        }

        void flipY(Entity::Type& entity, bool yes_or_no)
        {
            auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);
            if (e_texture_comp.has_value()) {
                if (e_texture_comp.value().get().b_flip.y != yes_or_no)
                {
                    e_texture_comp.value().get().b_flip.y = yes_or_no;
                }
            }
        }

        void setLastDirection(Entity::Type& entity, int dir)
        {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                e_physics_comp.value().get().last_direction = dir;
            }
        }

        int getLastDirection(Entity::Type& entity)
        {
            auto e_physics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (e_physics_comp.has_value()) {
                return e_physics_comp.value().get().last_direction;
            }
            // Return a rand value when cnt retrieve last dir
            return INT_MAX;
        }


        void handleCollision(Entity::Type entity_a, Entity::Type entity_b) {
            // Collision between damage and health
            const auto a_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity_a);
            const auto b_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(entity_b);

            if (a_damage_comp.has_value() || b_damage_comp.has_value()) {
                const auto a_faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity_a);
                const auto b_faction_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(entity_b);

                // Friendly fire check
                if (a_faction_comp.has_value() && b_faction_comp.has_value()) {
                    const auto& a_faction = a_faction_comp.value().get().faction;
                    const auto& b_faction = b_faction_comp.value().get().faction;

                    //NIKEE_CORE_WARN("{} and {} == {}", static_cast<int>(a_faction), static_cast<int>(b_faction), a_faction == b_faction);

                    if (a_faction == b_faction) {
                        return;
                    }
                }

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

        void restoreHealth(Entity::Type healer, Entity::Type target) {
            const auto healer_heal_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::HealthDrop>(healer);
            const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(target);
            const auto& healer_heal = healer_heal_comp.value().get().heal_amount;

            // Return if target has no health
            if (target_health_comp.has_value() == false) {
                return;
            }

            //const auto target_player_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Faction>(target);
            //auto& player_faction = target_player_comp.value().get().faction;
            
            // !TODO: Return if faction not player

            auto& target_health = target_health_comp.value().get().health;
            const auto& target_max_health = target_health_comp.value().get().max_health;

            // Heal Target
            if (target_health < target_max_health) {
                target_health += healer_heal;
            }
        }

        void applyDamage(Entity::Type attacker, Entity::Type target) {
            const auto attacker_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(attacker);
            const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(target);
            const auto attacker_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(attacker);
            const auto target_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(target);

            // Return if no damage comp and health comp
            if ((attacker_damage_comp.has_value() && target_health_comp.has_value()) == false) {
                return;
            }

            auto& target_health = target_health_comp.value().get();
            auto& attacker_damage = attacker_damage_comp.value().get().damage;

            // Check invulnerability flag
            if (target_health.invulnerable_flag) {
                return; // Skip damage
            }

            // Default dmg multiplier
            float multiplier = 1.f;

            // Apply elemental damage multiplier
            if (attacker_element_comp && target_element_comp) {
                const auto attacker_element = attacker_element_comp.value().get().element;
                const auto target_element = target_element_comp.value().get().element;

                multiplier = getElementMultiplier(attacker_element, target_element);
            }

            // Apply damage
            target_health.health -= (attacker_damage * multiplier);
            NIKEE_CORE_INFO("Entity {} took {} damage from Entity {}. Remaining health: {}",
                target, attacker_damage, attacker, target_health.health);
            // Play SFX when apply damage
            playSFX(attacker, true);
            // Play animation when taken damage
            static float deathAnimationTimer = 0.0f;
            static const float deathAnimationDuration = 0.5f; 
            // Handle death animation
            animationHurtStart(target, 0, 12);
            flipX(target, false);

            // Slow down transition from frame 0 to frame 1
            deathAnimationTimer += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();  

            // If timer reaches the desired duration, proceed to frame 1
            if (deathAnimationTimer >= deathAnimationDuration) {
                animationHurtEnd(target, 1, 12);
                // Reset timer
                deathAnimationTimer = 0.0f;  
            }

			// Check if target health drops to zero or below
			if (target_health.health <= 0) {
				// Target has more than 1 life
				--target_health.lives;
				target_health.health = target_health.max_health;
				NIKEE_CORE_INFO("Entity {} lost 1 life.", target);
			}
        }

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

                // !TODO: Play element change animation here
            }
        }

        float getElementMultiplier(Element::Elements attacker, Element::Elements defender) {
            return Element::elemental_multiplier_table[static_cast<int>(attacker)][static_cast<int>(defender)];
        }

        bool withinRange(Entity::Type source, Entity::Type player) {
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