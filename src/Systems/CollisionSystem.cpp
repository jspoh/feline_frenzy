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
    colliders.resize(300);  // Pre-allocate 300 colliders
    for (auto& collider : colliders) {
        collider.active = false;  // Mark all colliders as unused at start
    }
}


void Collision::Manager::update() { // Currently checks for collision between ALL used colliders
    for (size_t i = 0; i < colliders.size(); ++i) {
        if (!colliders[i].active) {
            continue;  // Skip inactive colliders
        }
        for (size_t j = i + 1; j < colliders.size(); ++j) {
            if (!colliders[j].active) {
                continue;  // Skip inactive colliders
            }
            // Apply detect collision between every collider
            if (detectCollision(colliders[i], colliders[j])) {
                cout << "Collision detected between objects " << i << " and " << j << endl;
            }
        }
    }
}


bool Collision::Manager::detectCollision(const Collider& a, const Collider& b) const {
    // Placeholder for collision time
    float timeOfCollision = 0.0f;

    // Check for Circle, then AABB, then SAT collision
    if (a.radius > 0.0f && b.radius > 0.0f) {
        return CollisionUtils::circleCollisionCheck(a.position, a.radius, b.position, b.radius);
    }
    else if (CollisionUtils::aabbRectRectCheck(a.velocity, b.velocity, a.rect_min, a.rect_max, b.rect_min, b.rect_max, timeOfCollision)) {
        return true;
    }
    else {
        return CollisionUtils::satCheck(a.position, a.size, b.position, b.size);
    }
}
// Test function
bool Collision::Manager::checkCollisionBetween( const Vector2& vel_a, const Vector2& rect_min_a, const Vector2& rect_max_a,
                                                const Vector2& vel_b, const Vector2& rect_min_b, const Vector2& rect_max_b,
                                                float radius_a, float radius_b) {
    // Placeholder for collision time
    float timeOfCollision = 0.0f;

    // Check for Circle, then AABB, then SAT collision
    if (radius_a > 0.0f && radius_b > 0.0f) {
        return CollisionUtils::circleCollisionCheck(rect_min_a, radius_a, rect_min_b, radius_b);
    }
    else {
        if (CollisionUtils::aabbRectRectCheck(vel_a, vel_b, rect_min_a, rect_max_a, rect_min_b, rect_max_b, timeOfCollision)) {
            return true;
        }
        else {
            return CollisionUtils::satCheck(rect_min_a, rect_max_a - rect_min_a, rect_min_b, rect_max_b - rect_min_b);
        }
    }
}


// Register a collider...
void Collision::Manager::registerCollider(const Vector2& position, const Vector2& size, float radius, const Vector2& velocity) {
    for (auto& collider : colliders) {
        if (!collider.active) {  // Find the first unused collider
            collider.active = true;
            collider.position = position;
            collider.size = size;
            collider.radius = radius;
            collider.velocity = velocity;
            collider.rect_min = { position.x - (size.x / 2), position.y - (size.y / 2) }; // Bounding box min
            collider.rect_max = { position.x + (size.x / 2), position.y + (size.y / 2) }; // Bounding box max
            return;  // Successfully registered
        }
    }
    // If no available colliders, log an error
    cout << "Error: No available colliders to register!" << endl;
}


// Unregister a collider (Removes collider currently based on position...)
void Collision::Manager::unregisterCollider(const Vector2& position) {
    for (auto& collider : colliders) {
        //if (collider.active && collider.position == position) {  // Find the collider based on position // Note Vector2 does not have defined/overloaded == or != operators currently
        if (collider.active && (collider.position.x == position.x && collider.position.y == position.y)) {
            collider.active = false;  // Mark the collider as unused
            return;
        }
    }
}

