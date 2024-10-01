/*****************************************************************//**
 * \file   mCollision.h
 * \brief  Collision manager header for the physics system.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include "../headers/Components/cCollision.h"

namespace Collision {

    class Manager {
    private:
        std::vector<Collider> colliders;  // List of all colliders

        // Utility functions
        template <typename T>
        T getMax(const T a, const T b) const;

        template <typename T>
        T getMin(const T a, const T b) const;

        /* Raw collision functions */

        // Circle collision check
        bool circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b) const;

        // AABB collision check for rectangle to rectangle
        bool aabbRectRectCheck(const Vector2& vel_a, const Vector2& vel_b,
            const Vector2& min_a, const Vector2& max_a,
            const Vector2& min_b, const Vector2& max_b,
            float& firstTimeOfCollision) const;

        // Separating Axis Theorem (SAT) check
        bool satCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b) const;

        // Function to detect if the mouse is inside a rectangle area
        bool detectMClickRect(const Vector2& center, float width, float height);

        // Function to detect if the mouse is inside a circle area
        bool detectMClickCircle(const Vector2& center, float radius);

    public:
        Manager();
        ~Manager();

        // Register a new collider
        void registerCollider(const Vector2& position, const Vector2& size, float radius, const Vector2& velocity);
        void registerCollider(const Transform::Transform& transformT, const Transform::Velocity& transformV);

        // Unregister a collider // Currently pass in just position
        void unregisterCollider(const Vector2& position);

        // Detect any collision between 2 colliders
        bool detectCollision(const Collider& a, const Collider& b);
        // Detect any collision between 2 cTransform
        bool detectCollision(const Transform::Transform& aT, const Transform::Velocity& aV, const Transform::Transform& bT, const Transform::Velocity& bV);
    };
}

#endif // COLLISION_MANAGER_HPP
