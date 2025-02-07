/*****************************************************************//**
 * \file   sysPhysics.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (95%)
 * \co-authors: Min Khant Ko, 2301320, ko.m@digipen.edu (5%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Systems/Physics/sysCollision.h"
#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"
#include "Managers/Services/sMap.h"

namespace NIKE {
    namespace Physics {
        class Manager :
            public System::ISystem,
            public Events::IEventListener<Physics::ChangePhysicsEvent> {
        private:
            // Delete Copy Constructor & Copy Assignment
            Manager(Manager const& copy) = delete;
            void operator=(Manager const& copy) = delete;

            // Collision sub system
            std::unique_ptr<Collision::System> collision_system;

            // Entity to grid cells mapping
            std::unordered_map<Entity::Type, std::vector<Vector2i>> entity_occupied_cells;

            // Collision check counter for logging
            std::unordered_map<Entity::Type, int> collision_checks_count;

            // Calculate which cells an entity occupies based on its transform
            std::vector<Vector2i> calculateOccupiedCells(const Transform::Transform& transform);

            // Update entity physics (movement, forces, etc)
            void updateEntityPhysics(Entity::Type entity, float dt);

            // Process collisions between potential pairs
            void processCollisions(const std::vector<std::pair<Entity::Type, Entity::Type>>& collision_pairs);

            // Get potential collision pairs from occupied cells
            std::vector<std::pair<Entity::Type, Entity::Type>> getPotentialCollisions();

            //// Old force application functions - preserved but commented out
            //void applyXForce(Entity::Type entity, float force);
            //void applyYForce(Entity::Type entity, float force);

        public:
            // Default Constructor
            Manager() = default;

            // Default Destructor
            ~Manager() = default;

            // Init
            void init() override;

            // System name
            std::string getSysName() override {
                return "Physics System";
            }

            // Update
            void update() override;

            // On change physics event
            void onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) override;
        };
    }
}

#endif //!PHYSICS_HPP