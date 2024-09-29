/*****************************************************************//**
 * \file   CollisionUtils.cpp
 * \brief  Functions and utility functions for collision detection.
 *
 * \author Min Khant Ko
 * \date   September 2024
 *********************************************************************/


#include "../headers/Systems/CollisionUtils.h"
#include "../headers/Systems/sysInput.h"
#include "../headers/Core/stdafx.h"

namespace CollisionUtils {

    template <typename T>
    T getMax(const T a, const T b) {
        return (a > b) ? a : b;
    }

    template <typename T>
    T getMin(const T a, const T b) {
        return (a < b) ? a : b;
    }

    bool circleCollisionCheck(const Vector2& pos_a, float radius_a, const Vector2& pos_b, float radius_b) {
        float distX = pos_a.x - pos_b.x;
        float distY = pos_a.y - pos_b.y;
        float distance = std::sqrt(distX * distX + distY * distY);

        return (distance < (radius_a + radius_b));
    }

    bool aabbRectRectCheck(const Vector2& vel_a, const Vector2& vel_b,
        const Vector2& min_a, const Vector2& max_a,
        const Vector2& min_b, const Vector2& max_b,
        float& firstTimeOfCollision) {
        // Step 1: Static collision detection
        // Check if the two AABBs are overlapping statically
        if (!(max_a.x < min_b.x || min_a.x > max_b.x ||
            max_a.y < min_b.y || min_a.y > max_b.y)) {
            // If there is an overlap in both x and y axes, a static collision occurs
            return true;
        }

        // Step 2: Calculate relative velocity
        Vector2 velRel = { vel_a.x - vel_b.x, vel_a.y - vel_b.y };

        // Initialize time of first and last collision along each axis
        Vector2 tFirst = { 0.0f, 0.0f };
        //Vector2 tLast = { (float)AEFrameRateControllerGetFrameTime(), (float)AEFrameRateControllerGetFrameTime() }; // Need a way to get time for previous frame
        Vector2 tLast = { 0.0f, 0.0f }; // Placeholder

        // Step 3: Check dynamic collision on x-axis
        if (velRel.x != 0) {
            if (velRel.x > 0) {
                tFirst.x = (min_a.x - max_b.x) / velRel.x;
                tLast.x = (max_a.x - min_b.x) / velRel.x;
            }
            else {
                tFirst.x = (max_a.x - min_b.x) / velRel.x;
                tLast.x = (min_a.x - max_b.x) / velRel.x;
            }
        }
        else if (max_a.x < min_b.x || min_a.x > max_b.x) {
            return false; // No collision on the x-axis if there's no relative movement and no static collision
        }

        // Step 4: Check dynamic collision on y-axis
        if (velRel.y != 0) {
            if (velRel.y > 0) {
                tFirst.y = (min_a.y - max_b.y) / velRel.y;
                tLast.y = (max_a.y - min_b.y) / velRel.y;
            }
            else {
                tFirst.y = (max_a.y - min_b.y) / velRel.y;
                tLast.y = (min_a.y - max_b.y) / velRel.y;
            }
        }
        else if (max_a.y < min_b.y || min_a.y > max_b.y) {
            return false; // No collision on the y-axis if there's no relative movement and no static collision
        }

        // Step 5: Check if collisions occur within the time frame
        float tFirstOverall = getMax(tFirst.x, tFirst.y);
        float tLastOverall = getMin(tLast.x, tLast.y);

        // Ensure that the time of first collision occurs before the time of last collision
        if (tFirstOverall > tLastOverall /* || tFirstOverall > AEFrameRateControllerGetFrameTime() */) { // Need a way to get time for previous frame
            return false; // No collision detected
        }

        // If we have a valid time of collision, update the output parameter
        firstTimeOfCollision = tFirstOverall;

        return true; // Collision detected
    }
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

    /*
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
