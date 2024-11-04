/*****************************************************************//**
 * \file   sysCollision.h
 * \brief  Collision manager header for the physics system.
 *
 * \author Min Khant Ko, 2301320, ko.m@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

namespace NIKE {
    namespace Collision {

        // Collision info for collision resolution
        struct CollisionInfo {
            Vector2f mtv;
            Vector2f collision_normal;
            float t_first;

            CollisionInfo() : mtv(), collision_normal(), t_first{ 0.0f } {}
        };

        // Collision System
        class System {
        private:
            // Internal AABB
            struct AABB {
                Vector2f rect_min;
                Vector2f rect_max;

                AABB(Vector2f const& rect_min, Vector2f const& rect_max)
                    : rect_min{ rect_min }, rect_max{ rect_max } {}
            };

            // Collision world restitution for bounce effect (0 = inelastic, 1 = perfectly elastic)
            float restitution;

            // Bounce collision resolution
            void bounceResolution(Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a, Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b, CollisionInfo const& info);

        public:
            System() : restitution{ 1.0f } {}
            ~System() = default;

            // Set collision world restitution
            void setRestitution(float val);

            // Get collision world restitution
            float getRestitution() const;

            // AABB Collision detection
            bool detectAABBRectRect(Transform::Transform const& transform_a, Physics::Dynamics const& dynamics_a, Transform::Transform const& transform_b, Physics::Dynamics const& dynamics_b, CollisionInfo& info);

            // Collision resolution
            void collisionResolution(Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a, Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b, CollisionInfo const& info);
        };
    }
}

#endif // COLLISION_MANAGER_HPP
