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
            const auto attacker_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Damage>(attacker);
            const auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Combat::Health>(target);
            const auto attacker_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(attacker);
            const auto target_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(target);

            // Return if no damage comp and health comp
            if (!(attacker_damage_comp && target_health_comp)) {
                return;
            }

            auto& target_health = target_health_comp.value().get();
            auto& attacker_damage = attacker_damage_comp.value().get().damage;

            // Check invulnerability flag
            if (target_health.invulnerableFlag) {
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

            // Check if target health drops to zero or below
            if (target_health.health <= 0) {
                if (target_health.lives <= 1) {
                    // Target only has 1 life left
                    NIKE_ECS_MANAGER->markEntityForDeletion(target);
                    NIKEE_CORE_INFO("Entity {} has been destroyed due to zero health.", target);
                }
                else {
                    // Target has more than 1 life
                    --target_health.lives;
                    target_health.health = 100;
                    NIKEE_CORE_INFO("Entity {} lost 1 life.", target);
                }
            }
        }

        void Manager::changeElement(Entity::Type player, Entity::Type source) {

            // Checking if it is the player that collided with element source
            if (!NIKE_ECS_MANAGER->getEntityComponent<GameLogic::ILogic>(player)) {
                return;
            }

            auto player_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Entity>(player);
            auto source_element_comp = NIKE_ECS_MANAGER->getEntityComponent<Element::Source>(source);

            if (player_element_comp && source_element_comp) {
                auto& player_element = player_element_comp.value().get().element;
                auto& source_element = source_element_comp.value().get().element;

                // Set player element to source element
                player_element = source_element;
            }
        }

        float Manager::getElementMultiplier(Element::Elements attacker, Element::Elements defender) {
            return Element::elemental_multiplier_table[static_cast<int>(attacker)][static_cast<int>(defender)];
        }

    }
}