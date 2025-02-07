/*****************************************************************//**
 * \file   sysPhysics.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
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

        //--------------------------------------------------------------------------
        // NEW Multi-Cell Broad Phase with Hard-coded "Wall" collision hack
        //
        // Instead of the old single-cell approach, we store the set of grid cells
        // occupied by each entity. Then build potential collision pairs from any
        // overlap in those cell sets. We add an extra hack: if an entity is named
        // "left_wall","right_wall","top_wall","bottom_wall", we forcibly pair it
        // with all other colliders.
        //
        // The old single-cell approach is commented out at the bottom for reference.
        //--------------------------------------------------------------------------
        class Manager :
            public System::ISystem,
            public Events::IEventListener<Physics::ChangePhysicsEvent>
        {
        private:
            // Delete Copy Constructor & Copy Assignment
            Manager(Manager const& copy) = delete;
            void operator=(Manager const& copy) = delete;

            // Collision sub system
            std::unique_ptr<Collision::System> collision_system;

            // Map each entity to the set of grid cells it occupies
            std::unordered_map<Entity::Type, std::vector<Vector2i>> entity_occupied_cells;

            // Keep track of how many collision checks each entity does, for logging
            std::unordered_map<Entity::Type, int> collision_checks_count;

            // Calculate which cells an entity occupies (plus a margin of 1 cell)
            std::vector<Vector2i> calculateOccupiedCells(const Transform::Transform& transform);

            // Update one entity's physics (forces, velocity) and occupied cells
            void updateEntityPhysics(Entity::Type entity, float dt);

            // Return potential collision pairs by checking overlap in cell sets
            std::vector<std::pair<Entity::Type, Entity::Type>> getPotentialCollisions();

            // Actually do collision detection/resolution for each pair
            void processCollisions(const std::vector<std::pair<Entity::Type, Entity::Type>>& collision_pairs);

            //// OLD CODE: Single-cell approach - commented out
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

    } // namespace Physics
} // namespace NIKE

#endif //!PHYSICS_HPP
