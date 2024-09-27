/*****************************************************************//**
 * \file   CollisionUtils.cpp
 * \brief  Functions and utility functions for collision detection.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/


#include "../headers/Core/stdafx.h"

/*
#include "CollisionUtils.h"
#include "InputSystem.h"
#include <iostream>

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

        return (distance < (radius_a + radius_b));
    }

    bool aabbRectRectCheck(const Vector2& pos_a, const Vector2& size_a, const Vector2& vel_a, const Vector2& pos_b, const Vector2& size_b, const Vector2& vel_b) {
        // Step 1: Static collision detection
        if (pos_a.x < pos_b.x + size_b.x &&
            pos_a.x + size_a.x > pos_b.x &&
            pos_a.y < pos_b.y + size_b.y &&
            pos_a.y + size_a.y > pos_b.y) {
            return true;  // Static collision detected
        }

        // Step 2: Dynamic collision detection using relative velocity
        Vector2 relativeVel = vel_a - vel_b;

        // Calculate time of collision along X-axis
        float tFirstX = (pos_b.x - (pos_a.x + size_a.x)) / relativeVel.x;
        float tLastX = ((pos_b.x + size_b.x) - pos_a.x) / relativeVel.x;

        // Calculate time of collision along Y-axis
        float tFirstY = (pos_b.y - (pos_a.y + size_a.y)) / relativeVel.y;
        float tLastY = ((pos_b.y + size_b.y) - pos_a.y) / relativeVel.y;

        // Ensure the collision happens within the valid time frame (tFirst <= tLast)
        if (tFirstX > tLastY || tFirstY > tLastX) {
            return false; // No dynamic collision
        }

        return true; // Dynamic collision detected
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

    // Detect if the mouse is inside a rectangular area
    bool detectMClickRect(const Vector2& center, float width, float height) {
        // Get the mouse position from Input::Manager
        float mouseX = Input::Manager::getInstance().mouse.x;
        float mouseY = Input::Manager::getInstance().mouse.y;

        // Calculate the boundaries of the rectangle
        float left = center.x - (width / 2.0f);
        float right = center.x + (width / 2.0f);
        float top = center.y - (height / 2.0f);
        float bottom = center.y + (height / 2.0f);

        // Check if the mouse is inside the rectangle
        if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom) {

            // Check if the mouse is exactly at the center
            if (mouseX == center.x && mouseY == center.y) {
                cout << "Mouse is exactly at the center of the rectangle." << endl;
            }
            else {
                // Print where the mouse is relative to the center of the rectangle
                if (mouseX < center.x)
                    cout << "Mouse is on the left side of the center." << endl;
                else
                    cout << "Mouse is on the right side of the center." << endl;

                if (mouseY < center.y)
                    cout << "Mouse is above the center." << endl;
                else
                    cout << "Mouse is below the center." << endl;
            }

            return true;
        }

        return false;
    }


    // Detect if the mouse is inside a circular area
    bool detectMClickCircle(const Vector2& center, float radius) {
        // Get the mouse position from Input::Manager
        float mouseX = Input::Manager::getInstance().mouse.x;
        float mouseY = Input::Manager::getInstance().mouse.y;

        // Calculate the distance from the mouse to the center of the circle
        float distX = mouseX - center.x;
        float distY = mouseY - center.y;
        float distance = std::sqrt(distX * distX + distY * distY);

        // Check if the mouse is inside the circle
        if (distance <= radius) {
            cout << "Mouse is inside the circle." << endl;
            return true;
        }

        return false;
    }
}
*/