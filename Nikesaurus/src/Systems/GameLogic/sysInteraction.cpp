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

            // Get entity's health and damage components
            auto a_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Health::Health>(entity_a);
            auto b_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Health::Health>(entity_b);

            auto a_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Damage::Damage>(entity_a);
            auto b_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Damage::Damage>(entity_b);

            // Apply damage if applicable
            applyDamage(entity_a, entity_b);
            applyDamage(entity_b, entity_a);
        }

        void Manager::applyDamage(Entity::Type attacker, Entity::Type target) {
            auto attacker_damage_comp = NIKE_ECS_MANAGER->getEntityComponent<Damage::Damage>(attacker);
            auto target_health_comp = NIKE_ECS_MANAGER->getEntityComponent<Health::Health>(target);

            if (attacker_damage_comp && target_health_comp) {
                auto& target_health = target_health_comp.value().get().health;
                auto& attacker_damage = attacker_damage_comp.value().get().damage;

                // Apply damage
                target_health -= attacker_damage;
                NIKEE_CORE_INFO("Entity {} took {} damage from Entity {}. Remaining health: {}",
                    target, attacker_damage, attacker, target_health);

                // Check if target health drops to zero or below
                if (target_health <= 0) {
                    //!TODO: DESTROY ENTITY
                    // NIKE_ECS_MANAGER->markEntityForDeletion(target);
                    NIKEE_CORE_INFO("Entity {} has been destroyed due to zero health.", target);
                }
            }
        }
    }
}