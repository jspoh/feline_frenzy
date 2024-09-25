/*****************************************************************//**
 * \file   CollisionSystem.cpp
 * \brief  Basic collision system
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "CollisionSystem.h"
#include "CollisionUtils.h"

void Collision::Manager::init() {
    // Initialization logic for collision system
    colliders.clear();
}

void Collision::Manager::update() {
    // Check for collisions between all registered colliders
    for (size_t i = 0; i < colliders.size(); ++i) {
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            if (detectCollision(colliders[i], colliders[j])) {

                // Handle collision response
                cout << "Collision detected between objects " << i << " and " << j << endl;

            }
        }
    }
}

bool Collision::Manager::detectCollision(const Collider& a, const Collider& b) const {

    // Check for Circle, then AABB, then SAT collision
    if (a.radius > 0.0f && b.radius > 0.0f) {
        return CollisionUtils::circleCollisionCheck(a.position, a.radius, b.position, b.radius);
    }
    else if (CollisionUtils::aabbRectRectCheck(a.position, a.size, a.velocity, b.position, b.size, b.velocity)) {
        return true;
    }
    else {
        return CollisionUtils::satCheck(a.position, a.size, b.position, b.size);
    }
}

// Test function to detect collision between 2 positions...
bool Collision::Manager::checkCollisionBetween(const Vector2& pos_a, const Vector2& size_a, const Vector2& vel_a, float radius_a, const Vector2& pos_b, const Vector2& size_b, const Vector2& vel_b, float radius_b) {
    // Check circle, then AABB, then SAT
    if (radius_a > 0.0f && radius_b > 0.0f) {
        return CollisionUtils::circleCollisionCheck(pos_a, radius_a, pos_b, radius_b);
    }
    else {
        // Check for AABB or SAT collision
        if (CollisionUtils::aabbRectRectCheck(pos_a, size_a, vel_a, pos_b, size_b, vel_b)) {
            return true;
        }
        else {
            return CollisionUtils::satCheck(pos_a, size_a, pos_b, size_b);
        }
    }
}

// Register a new collider
void Collision::Manager::registerCollider(const Vector2& position, const Vector2& size, float radius) {
    colliders.push_back({ position, size, radius });
}

// Unregister a collider (Removes collider based on POSITION)
void Collision::Manager::unregisterCollider(const Vector2& position) {

    colliders.erase(std::remove_if(colliders.begin(), colliders.end(),
       
        //[&position](const Collider& col) { return col.position == position; }), // Vector2 does not have defined/overloaded == or != operators currently
        [&position](const Collider& col) { return (col.position.x == position.x && col.position.y == position.y);
        }),

        colliders.end());
}
