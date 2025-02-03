/*****************************************************************//**
 * \file   sysInteraction.h
 * \brief  Interaction system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "Managers/Services/sEvents.h"
#include "Managers/ECS/mEntity.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cElement.h"

namespace NIKE {
    namespace Interaction {

        // Handles interaction logic between two entities
        void handleCollision(Entity::Type entity_a, Entity::Type entity_b);

        // Apply damage
        void applyDamage(Entity::Type attacker, Entity::Type target);

        // Change element
        void changeElement(Entity::Type player, Entity::Type source);

        // Get Elemental Multiplier for Elements
        float getElementMultiplier(Element::Elements attacker, Element::Elements target);

        // Check if player is within range
        bool withinRange(const Entity::Type& source, const Entity::Type& player);

        class Manager : 
            public System::ISystem,
            public Events::IEventListener<Physics::CollisionEvent> {
        public:
            // Default constructor
            Manager() = default;

            // Default destructor
            ~Manager() = default;

            // Initialization
            void init() override;

            // Update
            void update() override;

            // System name
            std::string getSysName() override {
                return "Interaction System";
            }

            // Event handling for collisions
            void onEvent(std::shared_ptr<Physics::CollisionEvent> event) override;

        private:
            Manager(Manager const& copy) = delete;
            void operator=(Manager const& copy) = delete;

            //// Handles interaction logic between two entities
            //void handleCollision(Entity::Type entity_a, Entity::Type entity_b);

            //// Apply damage
            //void applyDamage(Entity::Type attacker, Entity::Type target);

            //// Change element
            //void changeElement(Entity::Type player, Entity::Type source);

            //// Get Elemental Multiplier for Elements
            //float getElementMultiplier(Element::Elements attacker, Element::Elements target);

            //// Check if player is within range
            //bool withinRange(const Entity::Type& source, const Entity::Type& player);

        };
    }
}


#endif //!INTERACTION_HPP