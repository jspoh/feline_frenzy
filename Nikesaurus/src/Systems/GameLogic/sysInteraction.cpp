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
            // Register the Manager as a listener for collision events
            std::shared_ptr<Manager> interaction_sys_wrapped(this, [](Manager*) {});
            NIKE_EVENTS_SERVICE->addEventListeners<Physics::CollisionEvent>(interaction_sys_wrapped);
        }

        void Manager::update() {
            // Not used
        }

        void Manager::onEvent(std::shared_ptr<Physics::CollisionEvent> event) {
            // Ensure entities exist and handle the collision
            if (NIKE_ECS_MANAGER->checkEntity(event->entity_a) && NIKE_ECS_MANAGER->checkEntity(event->entity_b)) {
                handleCollision(event->entity_a, event->entity_b);
            }
        }

        void Manager::handleCollision(Entity::Type entity_a, Entity::Type entity_b) {
            // Player Element Swapping
            // Check for E key pressed
            if (NIKE_INPUT_SERVICE->isKeyPressed(NIKE_KEY_E)) {
                changeElement(entity_a, entity_b);
            }

            // Collision between damage and health
            applyDamage(entity_a, entity_b);
            applyDamage(entity_b, entity_a);
        }

        void Manager::applyDamage(Entity::Type attacker, Entity::Type target) {
            auto attacker_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Damage::Damage>(attacker);
            auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Health::Health>(target);

            if (attacker_damage_comp && target_health_comp) {
                auto& target_health = target_health_comp.value().get();
                auto& attacker_damage = attacker_damage_comp.value().get().damage;

                // Check invulnerability flag
                if (target_health.invulnerableFlag) {
                    return; // Skip damage
                }

                // Apply damage
                target_health.health -= attacker_damage;
                NIKEE_CORE_INFO("Entity {} took {} damage from Entity {}. Remaining health: {}",
                    target, attacker_damage, attacker, target_health.health);

                // Check if target health drops to zero or below
                if (target_health.health <= 0) {
                    NIKE_ECS_MANAGER->markEntityForDeletion(target);
                    NIKEE_CORE_INFO("Entity {} has been destroyed due to zero health.", target);
                }
            }
        }

        void Manager::changeElement(Entity::Type player, Entity::Type source) {
            auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Player>(player);
            auto source_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Source>(source);

            if (player_element_comp && source_element_comp) {
                auto& player_element = player_element_comp.value().get().current_element;
                auto& source_element = source_element_comp.value().get().element;

                // Set player element to source element
                player_element = source_element;
            }
        }
    }
}