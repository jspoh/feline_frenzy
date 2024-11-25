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
                AABB,
                Circle,
                Polygon
            };

            // Collider properties
            ShapeType shape_type;          // Type of collider shape
            Vector2f position;             // Collider's position in world space
            Vector2f size;                 // Dimensions: width and height for AABB, or radius for Circle
            std::vector<Vector2f> vertices; // Vertices for polygon shapes
            Vector2f offset;               // Offset relative to the Transform
            float rotation;                // Collider's independent rotation (in degrees)

            // Collision flags
            bool is_static;                // Whether the Collider is static (non-moving)
            bool is_trigger;               // Whether the Collider acts as a trigger (no physical resolution)
            bool b_collided;               // Collision flag
            Resolution resolution;         // Collision resolution type (NONE, SLIDE, BOUNCE)

            // Initialization flag
            bool isInitialized;            // True if Collider has been initialized from Transform

            // Default constructor
            Collider()
                : shape_type{ ShapeType::AABB },
                position{ 0.0f, 0.0f },
                size{ 0.0f, 0.0f },
                offset{ 0.0f, 0.0f },
                rotation{ 0.0f },
                is_static{ false },
                is_trigger{ false },
                b_collided{ false },
                resolution{ Resolution::NONE },
                isInitialized{ false } {}

            // Constructor with parameters
            Collider(ShapeType type, Vector2f size, Vector2f offset, float rotation,
                bool is_static, bool is_trigger, Resolution resolution)
                : shape_type{ type },
                position{ 0.0f, 0.0f },
                size{ size },
                offset{ offset },
                rotation{ rotation },
                is_static{ is_static },
                is_trigger{ is_trigger },
                b_collided{ false },
                resolution{ resolution },
                isInitialized{ true } {}

            // Initialize Collider from a Transform
            void initializeFromTransform(const Transform::Transform& transform) {
                position = transform.position + offset;
                size = transform.scale;
                rotation = transform.rotation;
            }

            // Sync Transform to Collider state
            void syncTransform(Transform::Transform& transform) const {
                if (!is_static) {
                    transform.position = position - offset;
                    transform.scale = size;
                    transform.rotation = rotation;
                }
            }
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