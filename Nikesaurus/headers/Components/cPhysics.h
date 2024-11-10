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
            float max_speed;
            float drag;
            float mass;
            Vector2f velocity;
            Vector2f force;

            Dynamics() : max_speed{ 0.0f }, drag{ 0.0f }, mass{ 0.0001f }, velocity(), force() {}
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

        void registerComponents();
    }
}

#endif // C_COLLISION_HPP