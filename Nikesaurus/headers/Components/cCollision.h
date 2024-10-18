/*****************************************************************//**
 * \file   cCollision.h
 * \brief  Defines the components used for Collision
 *
 * \author Min Khant Ko, 2301320, ko.m@digipen.edu (100%)
 * \date   September 2024
 *  All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef C_COLLISION_HPP
#define C_COLLISION_HPP

#include "../headers/Core/stdafx.h"

namespace NIKESAURUS {
    namespace Collision {

        //Temporary Disable DLL Export Warning
        #pragma warning(disable: 4251)

        struct NIKESAURUS_API Collider {
            bool active = false; // Tracks if collider is in use (collision enabled or not)
            //Vector2 position;  // Position of the object (X, Y) world coordinates
            //Vector2 size;      // Size (width, height)
            float radius = 0.0f; // Optional for circle colliders
            //Vector2 velocity; // Current velocity

            // For bounding box
            Vector2 rect_min;
            Vector2 rect_max;

            // Which sides of bounding box are colliding/"blocked"
            bool left = false;
            bool right = false;
            bool top = false;
            bool bottom = false;

            bool bounceFlag = false;

            // For bounding sphere...unused for now
            Vector2 cir_min;
            Vector2 cir_max;
        };

        //Re-enable DLL Export warning
        #pragma warning(default: 4251)

    }
}

#endif // C_COLLISION_HPP