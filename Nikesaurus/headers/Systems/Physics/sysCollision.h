#pragma once
#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include "Components/cTransform.h"
#include "Components/cPhysics.h"

namespace NIKE {
    namespace Collision {

        // Collision info for collision resolution
        struct CollisionInfo {
            Vector2f mtv;             // Minimum translation vector for resolving collision
            Vector2f collision_normal; // Collision normal vector to determine direction of collision (?)
            float t_first;            // Time of first collision

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
                    : rect_min{ rect_min }, rect_max{ rect_max } {
                }
            };

            // Collision world restitution for bounce effect (0 = inelastic, 1 = perfectly elastic)
            float restitution;

            // Bounce collision resolution
            void bounceResolution(Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
                Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
                CollisionInfo const& info);

            // SAT helper functions
            std::vector<Vector2f> getRotatedVertices(const Physics::Collider& collider, const std::string& model_id);
            std::vector<Vector2f> getSeparatingAxes(const std::vector<Vector2f>& verticesA, const std::vector<Vector2f>& verticesB);
            void projectVerticesOnAxis(const std::vector<Vector2f>& vertices, const Vector2f& axis, float& min, float& max);

            // Health drop collision check
            bool healthDropCollisionCheck(Entity::Type entity_a, Entity::Type entity_b);

            // Faction collision check
            bool factionCollisionCheck(Entity::Type entity_a, Entity::Type entity_b);

        public:
            // Move the EntityPairHash definition to public so that it is accessible.
            struct EntityPairHash {
                std::size_t operator()(const std::pair<Entity::Type, Entity::Type>& pair) const {
                    return std::hash<Entity::Type>()(pair.first) ^ std::hash<Entity::Type>()(pair.second);
                }
            };

        private:
            // Processed collisions (to prevent one collision from being registered as multiple)
            std::unordered_set<std::pair<Entity::Type, Entity::Type>, EntityPairHash> processed_collisions;

        public:
            System() : restitution{ 1.0f } {}
            ~System() = default;

            // Set collision world restitution
            void setRestitution(float val);

            // Get collision world restitution
            float getRestitution() const;

            // AABB collision detection
            bool detectAABBRectRect(Physics::Dynamics const& dynamics_a, Physics::Collider const& collider_a,
                Physics::Dynamics const& dynamics_b, Physics::Collider const& collider_b,
                CollisionInfo& info);

            // SAT collision detection
            bool detectSATCollision(const Physics::Collider& colliderA, const Physics::Collider& colliderB,
                const std::string& model_idA, const std::string& model_idB, CollisionInfo& info);

            //Apply slide correction
            void applySlideCorrection(Physics::Dynamics& dynamics, const Vector2f& normal);

            //Resolve slide collision
            void resolveSlideCollision(
                Transform::Transform& a, Physics::Dynamics& dynamics_a,
                Transform::Transform& b, Physics::Dynamics& dynamics_b,
                const CollisionInfo& info);

            // Collision resolution
            void collisionResolution(Entity::Type entity_a, Transform::Transform& transform_a,
                Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
                Entity::Type entity_b, Transform::Transform& transform_b,
                Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
                CollisionInfo const& info);

            // Clear processed collisions
            void clearProcessedCollisions();
        };
    }
}

#endif // COLLISION_MANAGER_HPP
