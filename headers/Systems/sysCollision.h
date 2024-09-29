/*****************************************************************//**
 * \file   sysCollision.h
 * \brief  Collision manager header for the physics system.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

namespace Collision {

    class Manager {
    private:

        struct Collider {
            bool active = false; // Tracks if collider is in use
            Vector2 position;  // Position of the object (X, Y) world coordinates
            Vector2 size;      // Size (width, height)
            float radius = 0.0f; // Optional for circle colliders
            Vector2 velocity; // Current velocity

            // For bounding box
            Vector2 rect_min;
            Vector2 rect_max;

            // For bounding sphere...unused for now
            Vector2 cir_min;
            Vector2 cir_max;
        };

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

        // Unregister a collider
        void unregisterCollider(const Vector2& position);

        // Detect ANY collision between 2 colliders
        bool detectCollision(const Collider& a, const Collider& b) const;

    };
}

#endif // COLLISION_MANAGER_HPP
