/*****************************************************************//**
 * \file   sysInteraction.cpp
 * \brief  Interaction manager
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
            auto e_audio_comp = NIKE_ECS_MANAGER->getEntityComponent<Audio::SFX>(target);
            if (e_audio_comp.has_value())
            {
                auto& audio_comp = e_audio_comp.value().get();
                audio_comp.audio_id = "EnemyGetHit.wav";
                audio_comp.b_play_sfx = true;
            }

			// Check if target health drops to zero or below
			if (target_health.health <= 0) {
				// Target has more than 1 life
				--target_health.lives;
				target_health.health = ENEMY_HEALTH;
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

        bool withinRange(const Entity::Type& source, const Entity::Type& player) {
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