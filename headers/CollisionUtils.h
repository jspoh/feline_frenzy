#pragma once
/*****************************************************************//**
 * \file   CollisionUtils.h
 * \brief  Functions and utility functions for collision detection.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef COLLISION_UTILS_HPP
#define COLLISION_UTILS_HPP

#include "Vector2.h"

namespace CollisionUtils {

    // Utility functions for max and min
    template <typename T>
    T get_max(const T a, const T b);

    template <typename T>
    T get_min(const T a, const T b);

    // Circle collision check
    bool circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b);

    // AABB collision check
    bool aabbCollisionCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b);

    // Separating Axis Theorem (SAT) check
    bool satCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b);
}

#endif // COLLISION_UTILS_HPP
