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
            if (b_health_comp && a_damage_comp) {
                auto& b_health = b_health_comp.value().get().health;
                auto& a_damage = a_damage_comp.value().get().damage;
                b_health -= a_damage;
                NIKEE_CORE_INFO("Entity b health: " + std::to_string(b_health));
                //NIKEE_CORE_INFO("Entity {} dealt {} damage to Entity {}. Remaining health: {}", entity_a, damage_a->damage, entity_b, health_b->health);
            }

            if (a_health_comp && b_damage_comp) {
                auto& a_health = a_health_comp.value().get().health;
                auto& b_damage = b_damage_comp.value().get().damage;
                a_health -= b_damage;
                NIKEE_CORE_INFO("Entity a health: " + std::to_string(a_health));
                //NIKEE_CORE_INFO("Entity {} dealt {} damage to Entity {}. Remaining health: {}", entity_b, damage_b->damage, entity_a, health_a->health);
            }

            // Handle destruction if health drops to zero
            if (a_health_comp) {
                auto& a_health = a_health_comp.value().get().health;
                if (a_health <= 0) {
                    //NIKE_ECS_MANAGER->markEntityForDeletion(entity_a);
                    //!TODO: DESTROY ENTITY 
                    NIKEE_CORE_INFO("Entity {} has been destroyed due to zero health.", entity_a);
                }
            }

            if (b_health_comp) {
                auto& b_health = b_health_comp.value().get().health;
                if (b_health <= 0) {
                    //NIKE_ECS_MANAGER->markEntityForDeletion(entity_b);
                    //!TODO: DESTROY ENTITY 
                    NIKEE_CORE_INFO("Entity {} has been destroyed due to zero health.", entity_b);
                }
            }
        }
    }
}