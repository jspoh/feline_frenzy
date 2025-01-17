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
            std::vector<Force> forces; //Future implementation!!!
            int last_direction; // COMPONENT TO BE MOVED ELSE WHERE

            Dynamics() : max_speed{ 0.0f }, drag{ 0.0f }, mass{ EPSILON }, velocity(), force(), last_direction() {}
            Dynamics(float max_speed, float drag, float mass)
                : max_speed{ max_speed }, drag{ drag }, mass{ mass }, velocity(), force(), last_direction() {}
        };

        enum class Resolution {
            NONE = 0,
            SLIDE,
            BOUNCE,
            DESTROY
        };

        struct Collider {
            enum class ShapeType {
                AABB,
                Circle,
                Polygon
            };

            ShapeType shape_type;
            std::vector<Vector2f> vertices;
            bool b_bind_to_entity;
            Transform::Transform transform;
            Vector2f pos_offset;
            bool b_collided;
            Resolution resolution;
            float restitution; // Entity-specific restitution (elasticity)

            Collider()
                : shape_type{ ShapeType::AABB },
                transform(),
                pos_offset{ 0.0f, 0.0f },
                b_bind_to_entity{ true },
                b_collided{ false },
                resolution{ Resolution::NONE },
                restitution{ 1.0f } {} // Default to perfectly elastic
        };


        //Change physics world variables
        struct ChangePhysicsEvent : public Events::IEvent {
            float restitution;

            ChangePhysicsEvent(float restitution)
                : restitution{ restitution } {}
        };

        //For damage and health interaction
        struct CollisionEvent : public Events::IEvent {
            Entity::Type entity_a; 
            Entity::Type entity_b;
            
            CollisionEvent(Entity::Type a, Entity::Type b)
                : entity_a{ a }, entity_b{ b } {}
        };

        void registerComponents();
    }
}

#endif // C_COLLISION_HPP