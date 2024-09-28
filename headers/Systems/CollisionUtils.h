#pragma once
/*****************************************************************//**
 * \file   CollisionUtils.h
 * \brief  Functions and utility functions for collision detection.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/
/*
#pragma once

#ifndef COLLISION_UTILS_HPP
#define COLLISION_UTILS_HPP

#include "Vector2.h"

namespace CollisionUtils {

    // Utility functions
    template <typename T>
    T getMax(const T a, const T b);

    template <typename T>
    T getMin(const T a, const T b);

    // Circle collision check
    bool circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b);

    // AABB collision check for rectangle to rectangle
    bool aabbRectRectCheck( const Vector2& vel_a, const Vector2& vel_b,
                            const Vector2& min_a, const Vector2& max_a,
                            const Vector2& min_b, const Vector2& max_b,
                            float& firstTimeOfCollision);


    // Separating Axis Theorem (SAT) check
    bool satCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b);

    // Function to detect if the mouse is inside a rectangle area
    bool detectMClickRect(const Vector2& center, float width, float height);

    // Function to detect if the mouse is inside a circle area
    bool detectMClickCircle(const Vector2& center, float radius);
}

#endif // COLLISION_UTILS_HPP
*/