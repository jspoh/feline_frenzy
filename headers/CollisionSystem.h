/*****************************************************************//**
 * \file   CollisionManager.h
 * \brief  Basic collision system
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP

#include "stdafx.h"
#include "System.h"
#include "Vector2.h"
#include "CollisionUtils.h"

namespace Collision {

    class Manager : public System::Base {
    private:
        // Private constructor for singleton pattern
        Manager() = default;

        // Delete Copy Constructor & Copy Assignment
        Manager(Manager const& copy) = delete;
        void operator=(Manager const& copy) = delete;

        struct Collider {
            Vector2 position;  // Position of the object (X, Y) world coordinates...
            Vector2 size;      // Size (width, height)
            float radius = 0.0f; // Optional for circle colliders

            // Test...
            Vector2 velocity;
            Vector2 rect_min;
            Vector2 rect_max;

            // For bounding box and bounding sphere
            Vector2 min;
            Vector2 max;
        };

        std::vector<Collider> colliders;  // List of colliders in the system (CURRENTLY STORES ALL COLLIDERS)

        // Function to detect collisions between two colliders!
        bool detectCollision(const Collider& a, const Collider& b) const;

        // Test function to check for collision between 2 positions...
        bool checkCollisionBetween(const Vector2& pos_a, const Vector2& size_a, const Vector2& vel_a, float radius_a, const Vector2& pos_b, const Vector2& size_b, const Vector2& vel_b, float radius_b);


    public:
        // Singleton instance
        static Manager& getInstance() {
            static Manager instance;
            return instance;
        }

        // Initialization
        void init() override;

        // Update - where collisions will be checked every frame
        void update() override;

        // Register a new collider
        void registerCollider(const Vector2& position, const Vector2& size, float radius = 0.0f);

        // Unregister a collider
        void unregisterCollider(const Vector2& position);

        // Destructor
        ~Manager() override = default;
    };
}

#endif // COLLISION_SYSTEM_HPP
