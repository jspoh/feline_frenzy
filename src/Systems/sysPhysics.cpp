/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author b.soh
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysPhysics.h"

// Forward declaration for Collision
//namespace Collision {
//    class Manager;
//}


void Physics::Manager::init() {

}

void Physics::Manager::runtimeScaleOrRotate(Transform::Runtime_Transform& runtime_comp, Transform::Transform& transform_comp)
{

	if (runtime_comp.runtime_scale_up)
	{
		transform_comp.scale.x *= 1.05f;
		transform_comp.scale.y *= 1.05f;
	}

    if (runtime_comp.runtime_scale_down)
    {
        transform_comp.scale.x /= 1.05f;
        transform_comp.scale.y /= 1.05f;
    }

	if (runtime_comp.runtime_rotate)
	{
		transform_comp.rotation += 10.f;
	}

}

void Physics::Manager::update() {
    float dt = NIKEEngine.getDeltaTime();

    for (const auto& entity : entities) {
        // Check if entity contains Transform component and Velocity component
        if (NIKEEngine.checkEntityComponent<Transform::Transform>(entity) && NIKEEngine.checkEntityComponent<Transform::Velocity>(entity)) {

            // Reference to transform component
            Transform::Transform& transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

            // Ref to velocity component
            Transform::Velocity& velocity = NIKEEngine.getEntityComponent<Transform::Velocity>(entity);

            const float speed = 100.0f;

            // Check if entity contains Move component

            if (NIKEEngine.checkEntityComponent<Move::Movement>(entity)) {
                // Ref to Move component
                Move::Movement& move = NIKEEngine.getEntityComponent<Move::Movement>(entity);

                const float movespeed = 300.0f;

                // Reset Velocity
                velocity.velocity.y = 0.0f;
                velocity.velocity.x = 0.0f;

                if (move.Up) {
                    velocity.velocity.y += movespeed;
                }

                if (move.Down) {
                    velocity.velocity.y -= movespeed;
                }

                if (move.Left) {
                    velocity.velocity.x -= movespeed;
                }

                if (move.Right) {
                    velocity.velocity.x += movespeed;
                }
            }

            // Normalize Movement
            if (velocity.velocity.lengthSq() > 0.0f) {
                velocity.velocity = velocity.velocity.normalize();
            }

            // Apply velocity
            transform.position += velocity.velocity * speed * dt;

        }
        else if (NIKEEngine.checkEntityComponent<Transform::Transform>(entity) && NIKEEngine.checkEntityComponent<Transform::Runtime_Transform>(entity))
        {
            // Reference to transform component
            Transform::Transform& c_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

            // Ref to runtime component
            Transform::Runtime_Transform& c_runtime = NIKEEngine.getEntityComponent<Transform::Runtime_Transform>(entity);
            runtimeScaleOrRotate(c_runtime, c_transform);
        }
    }
}