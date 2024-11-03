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

#include "Core/stdafx.h"
#include "Managers/Services/sEvents.h"

namespace NIKE {
    namespace Physics {

        struct Dynamics {
            const float max_speed;
            const float drag;
            const float mass;
            Vector2f velocity;
            Vector2f force;

            Dynamics() : max_speed{ 0.0f }, drag{ 0.0f }, mass{ 0.0f }, velocity(), force() {}
            Dynamics(float max_speed, float drag, float mass)
                : max_speed{ max_speed }, drag{ drag }, mass{ mass }, velocity(), force() {}
        };

        enum class Resolution {
            NONE = 0,
            SLIDE,
            BOUNCE
        };

        struct Collider {
            bool b_collided;
            Resolution resolution;
            
            Collider() : b_collided{ false }, resolution{ Resolution::NONE }{}
            Collider(Resolution resolution) : b_collided{ false }, resolution{ resolution } {}
        };

        //Change physics world variables
        struct ChangePhysicsEvent : public Events::IEvent {
            float restitution;

            ChangePhysicsEvent(float restitution)
                : restitution{ restitution } {}
        };

        //Temporary Disable DLL Export Warning
        #pragma warning(disable: 4251)

        //struct NIKE_API Collider {
        //    bool active = false; // Tracks if collider is in use (collision enabled or not)
        //    //Vector2 position;  // Position of the object (X, Y) world coordinates
        //    //Vector2 size;      // Size (width, height)
        //    float radius = 0.0f; // Optional for circle colliders
        //    //Vector2 velocity; // Current velocity

        //    // For bounding box
        //    Vector2f rect_min;
        //    Vector2f rect_max;

        //    // Which sides of bounding box are colliding/"blocked"
        //    bool left = false;
        //    bool right = false;
        //    bool top = false;
        //    bool bottom = false;

        //    bool bounceFlag = false;

        //    // For bounding sphere...unused for now
        //    Vector2f cir_min;
        //    Vector2f cir_max;
        //};

        //Re-enable DLL Export warning
        #pragma warning(default: 4251)

    }
}

#endif // C_COLLISION_HPP