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

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"
#include "../headers/Math/Vector2.h"
#include "../headers/Systems/CollisionUtils.h"

/*
namespace Collision {

    class Manager : public System::ISystem {
    private:
        // Private constructor for singleton pattern
        Manager() = default;

        // Delete Copy Constructor & Copy Assignment
        Manager(Manager const& copy) = delete;
        void operator=(Manager const& copy) = delete;

        struct Collider {
            // Note Vector2 default initialized to zero vector already

            bool active = false; // Tracks if collider is in use

            int objType = 7; // Placeholder for object type

            Vector2 position;  // Position of the object (X, Y) world coordinates...
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

        std::vector<Collider> colliders;  // List of colliders in the system (CURRENTLY STORES ALL COLLIDERS)

        // Function to set bounding box for collider
        void setBoundingBox(Collider);

        // Function to detect collisions between two colliders!
        bool detectCollision(const Collider& a, const Collider& b) const;

        // Test function to check for collision between 2 positions...
        bool checkCollisionBetween( const Vector2& vel_a, const Vector2& rect_min_a, const Vector2& rect_max_a,
                                    const Vector2& vel_b, const Vector2& rect_min_b, const Vector2& rect_max_b,
                                    float radius_a, float radius_b);


    public:
        //Singleton Of Manager Class
        static std::shared_ptr<Manager> getInstance() {
            static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
            return instance;
        }

        // Initialization
        void init() override;

        // Update - where collisions will be checked every frame
        void update() override;

        // Register a new collider
        void registerCollider(const Vector2& position, const Vector2& size, float radius, const Vector2& velocity);

        // Unregister a collider
        void unregisterCollider(const Vector2& position);

        // Destructor
        ~Manager() override = default;
    };
}

*/

#endif // COLLISION_SYSTEM_HPP
