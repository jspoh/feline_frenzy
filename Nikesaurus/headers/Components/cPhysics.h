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

        struct Force {
            Vector2f direction;
            float life_time;

            Force(Vector2f const& direction, float life_time)
                : direction{ direction }, life_time{ life_time } {}
        };

        struct Dynamics {
            float max_speed;
            float drag;
            float mass;
            Vector2f velocity;
            Vector2f force; //Depricated to be removed
            std::vector<Force> forces;

            Dynamics() : max_speed{ 0.0f }, drag{ 0.0f }, mass{ EPSILON }, velocity(), force() {}
            Dynamics(float max_speed, float drag, float mass)
                : max_speed{ max_speed }, drag{ drag }, mass{ mass }, velocity(), force() {}
        };

        enum class Resolution {
            NONE = 0,
            SLIDE,
            BOUNCE
        };

        struct Collider {
            enum class ShapeType {
                AABB,        // Axis-Aligned Bounding Box
                Circle,      // Circle collider
                Polygon      // Convex Polygon collider
            };

            ShapeType shape_type;                // Type of collider shape
            Vector2f size;                       // Dimensions: width and height for AABB, or radius for Circle
            std::vector<Vector2f> vertices;      // Vertices for polygon shapes
            Vector2f offset{ 0.0f, 0.0f };       // Relative to entity's Transform
            Vector2f position{ 0.0f, 0.0f };     // World position (calculated using offset and Transform position)
            float rotation{ 0.0f };              // Independent rotation
            bool is_static = false;              // If true, collider does not move (no need update from Transform component)
            bool is_trigger = false;             // Trigger for non-physical collision zones

            bool b_collided = false;             // Collision flag
            Resolution resolution = Resolution::NONE; // Collision resolution type (NONE, SLIDE, BOUNCE)

            // Default constructor
            Collider()
                : shape_type{ ShapeType::AABB }, size{ 20.0f, 20.0f }, offset{ 0.0f, 0.0f }, position{ 0.0f, 0.0f },
                rotation{ 0.0f }, is_static{ false }, is_trigger{ true }, b_collided{ false },
                resolution{ Resolution::NONE } {}

            // Constructor taking all members
            Collider(ShapeType type, Vector2f size, Vector2f offset, Vector2f position, float rotation, bool is_static,
                bool is_trigger, Resolution resolution, bool b_collided = false)
                : shape_type{ type }, size{ size }, offset{ offset }, position{ position }, rotation{ rotation },
                is_static{ is_static }, is_trigger{ is_trigger }, b_collided{ b_collided },
                resolution{ resolution } {}
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