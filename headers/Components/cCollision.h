#pragma once

#ifndef C_COLLISION_HPP
#define C_COLLISION_HPP

#include "../headers/Core/stdafx.h"

namespace Collision {

    struct Collider {
        bool active = false; // Tracks if collider is in use (collision enabled or not)
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

}

#endif // C_COLLISION_HPP