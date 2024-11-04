/*****************************************************************//**
* \file   sysCollision.cpp
* \brief  Implements the collision manager.
*
* \author Min Khant Ko, 2301320, ko.m@digipen.edu (100%)
* \date   September 2024
* All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysCollision.h"

namespace NIKE {

    void Collision::System::bounceResolution(
        Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        // Calculate relative velocity
        Vector2f vel_rel = dynamics_a.velocity - dynamics_b.velocity;
        float normal_vel = vel_rel.dot(info.collision_normal);

        // Check if entities are already moving apart
        if (normal_vel > 0) return;

        // Calculate impulse magnitude based on collision response
        float impulse_magnitude = -(1 + restitution) * normal_vel;

        // Calculate the reflection vector for angular bounce
        Vector2f impulse = info.collision_normal.operator*(impulse_magnitude);

        if (collider_a.resolution == Physics::Resolution::NONE) {
            // Transform back outside of collision
            transform_b.position += info.mtv;

            // Apply impluse to velocity
            dynamics_b.velocity -= impulse;
        }
        else if (collider_b.resolution == Physics::Resolution::NONE) {
            // Transform back outside of collision
            transform_a.position += info.mtv;

            // Apply impluse to velocity
            dynamics_a.velocity += impulse;
        }
        else {
            // Transform back outside of collision
            transform_a.position += info.mtv.operator*(0.5f);
            transform_b.position -= info.mtv.operator*(0.5f);

            // Apply impulse to velocity based on mass
            dynamics_a.velocity += impulse.operator*(dynamics_b.mass / (dynamics_a.mass + dynamics_b.mass));
            dynamics_b.velocity -= impulse.operator*(dynamics_a.mass / (dynamics_a.mass + dynamics_b.mass));
        }
    }

    void Collision::System::setRestitution(float val) {
        restitution = val;
    }

    float Collision::System::getRestitution() const {
        return restitution;
    }

    bool Collision::System::detectAABBRectRect(
        Transform::Transform const& transform_a, Physics::Dynamics const& dynamics_a,
        Transform::Transform const& transform_b, Physics::Dynamics const& dynamics_b,
        CollisionInfo& info) {
        
        // References to components
        AABB aabb_a({ transform_a.position.x - (transform_a.scale.x * 0.5f), transform_a.position.y - (transform_a.scale.y * 0.5f) }, { transform_a.position.x + (transform_a.scale.x * 0.5f), transform_a.position.y + (transform_a.scale.y * 0.5f) });
        AABB aabb_b({ transform_b.position.x - (transform_b.scale.x * 0.5f), transform_b.position.y - (transform_b.scale.y * 0.5f) }, { transform_b.position.x + (transform_b.scale.x * 0.5f), transform_b.position.y + (transform_b.scale.y * 0.5f) });

        // Get delta time & set epsilon
        const float deltaTime = NIKE_ENGINE.getService<Windows::Service>()->getDeltaTime();
        const float epsilon = 0.0001f;

        // Step 1: Static collision detection
        if (!(aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x ||
            aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y)) {

            // Calculate overlaps on each axis
            float overlapX = Utility::getMin(aabb_a.rect_max.x - aabb_b.rect_min.x, aabb_b.rect_max.x - aabb_a.rect_min.x);
            float overlapY = Utility::getMin(aabb_a.rect_max.y - aabb_b.rect_min.y, aabb_b.rect_max.y - aabb_a.rect_min.y);

            // Determine MTV direction and smallest overlap
            Vector2f mtv_dir;
            if (overlapX < overlapY) {
                mtv_dir = { (aabb_a.rect_min.x < aabb_b.rect_min.x ? -1.0f : 1.0f), 0.0f };
            }
            else {
                mtv_dir = { 0.0f, (aabb_a.rect_min.y < aabb_b.rect_min.y ? -1.0f : 1.0f) };
            }

            // Set MTV and collision normal
            info.mtv = { mtv_dir.x * (overlapX < overlapY ? overlapX : overlapY), mtv_dir.y * (overlapX < overlapY ? overlapX : overlapY) };
            info.collision_normal = mtv_dir;

            info.t_first = 0.0f; //Static collision occurs
            return true;
        }

        // Step 2: Dynamic collision detection
        Vector2 velRel = { dynamics_a.velocity.x - dynamics_b.velocity.x, dynamics_a.velocity.y - dynamics_b.velocity.y };

        // Initialize time of first and last collision along each axis
        Vector2 tFirst = { 0.0f, 0.0f };
        Vector2 tLast = { deltaTime, deltaTime };

        // Step 3: Check dynamic collision on x-axis
        if (abs(velRel.x) > epsilon) {
            if (velRel.x > 0) {
                tFirst.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / velRel.x;
                tLast.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / velRel.x;
            }
            else {
                tFirst.x = (aabb_a.rect_max.x - aabb_b.rect_min.x) / velRel.x;
                tLast.x = (aabb_a.rect_min.x - aabb_b.rect_max.x) / velRel.x;
            }
        }
        else if (aabb_a.rect_max.x < aabb_b.rect_min.x || aabb_a.rect_min.x > aabb_b.rect_max.x) {
            return false; // No collision on the x-axis if there's no relative movement and no static collision
        }

        // Step 4: Check dynamic collision on y-axis
        if (abs(velRel.y) > epsilon) {
            if (velRel.y > 0) {
                tFirst.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / velRel.y;
                tLast.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / velRel.y;
            }
            else {
                tFirst.y = (aabb_a.rect_min.y - aabb_b.rect_max.y) / velRel.y;
                tLast.y = (aabb_a.rect_max.y - aabb_b.rect_min.y) / velRel.y;
            }
        }
        else if (aabb_a.rect_max.y < aabb_b.rect_min.y || aabb_a.rect_min.y > aabb_b.rect_max.y) {
            return false; // No collision on the y-axis if there's no relative movement and no static collision
        }

        // Step 5: Check if collisions occur within the time frame
        float tFirstOverall = Utility::getMax(tFirst.x, tFirst.y);
        float tLastOverall = Utility::getMin(tLast.x, tLast.y);

        if (tFirstOverall > tLastOverall || tFirstOverall > deltaTime) {
            return false; // No collision detected
        }

        // Update the time of first collision
        info.t_first = tFirstOverall;

        // Determine MTV and collision normal based on earliest axis of collision
        float smallestOverlap;
        Vector2f mtv_dir;
        if (abs(aabb_a.rect_min.y - aabb_b.rect_max.y)) {
            mtv_dir = { 1.0f, 0.0f };
            smallestOverlap = abs(aabb_a.rect_min.x - aabb_b.rect_max.x);
        }
        else {
            mtv_dir = { 0.0f, 1.0f };
            smallestOverlap = abs(aabb_a.rect_min.y - aabb_b.rect_max.y);
        }

        info.mtv = { mtv_dir.x * smallestOverlap, mtv_dir.y * smallestOverlap };
        info.collision_normal = mtv_dir;

        return true; // Collision detected
    }

    void Collision::System::collisionResolution(
        Transform::Transform& transform_a, Physics::Dynamics& dynamics_a, Physics::Collider& collider_a,
        Transform::Transform& transform_b, Physics::Dynamics& dynamics_b, Physics::Collider& collider_b,
        CollisionInfo const& info) {

        // Bounce Resolution
        if (collider_a.resolution == Physics::Resolution::BOUNCE || collider_b.resolution == Physics::Resolution::BOUNCE) {
            bounceResolution(transform_a, dynamics_a, collider_a, transform_b, dynamics_b, collider_b, info);
        }

        switch (collider_a.resolution) {
        case Physics::Resolution::NONE:
            break;
        case Physics::Resolution::SLIDE:
            transform_a.position += info.mtv;
            break;
        default:
            break;
        }

        switch (collider_b.resolution) {
        case Physics::Resolution::NONE:
            break;
        case Physics::Resolution::SLIDE:
            transform_b.position += info.mtv;
            break;
        default:
            break;
        }
    }

}
