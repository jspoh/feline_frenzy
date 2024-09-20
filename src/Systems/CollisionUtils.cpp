/*****************************************************************//**
 * \file   CollisionUtils.cpp
 * \brief  Functions and utility functions for collision detection.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/

#include "CollisionUtils.h"
#include <cmath>

namespace CollisionUtils {

    template <typename T>
    T get_max(const T a, const T b) {
        return (a > b) ? a : b;
    }

    template <typename T>
    T get_min(const T a, const T b) {
        return (a < b) ? a : b;
    }

    bool circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b) {
        float distX = pos_a.x - pos_b.x;
        float distY = pos_a.y - pos_b.y;
        float distance = std::sqrt(distX * distX + distY * distY);
        return distance < (radius_a + radius_b);
    }

    bool aabbCollisionCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b) {
        return (pos_a.x < pos_b.x + size_b.x &&
            pos_a.x + size_a.x > pos_b.x &&
            pos_a.y < pos_b.y + size_b.y &&
            pos_a.y + size_a.y > pos_b.y);
    }

    bool satCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& pos_b, const Vector2& size_b) {
        // Calculate the half extents for each box
        Vector2 half_extent_a(size_a.x / 2.0f, size_a.y / 2.0f);
        Vector2 half_extent_b(size_b.x / 2.0f, size_b.y / 2.0f);

        // Calculate the center points of both boxes
        Vector2 center_a = pos_a + half_extent_a;
        Vector2 center_b = pos_b + half_extent_b;

        // Calculate the distance between the centers
        Vector2 dist = center_b - center_a;

        // Calculate overlap on the x-axis
        float overlap_x = half_extent_a.x + half_extent_b.x - std::abs(dist.x);
        if (overlap_x <= 0) return false;  // No overlap on x-axis, so no collision

        // Calculate overlap on the y-axis
        float overlap_y = half_extent_a.y + half_extent_b.y - std::abs(dist.y);
        if (overlap_y <= 0) return false;  // No overlap on y-axis, so no collision

        // If we get here, both axes overlap, so a collision exists
        return true;
    }
}
