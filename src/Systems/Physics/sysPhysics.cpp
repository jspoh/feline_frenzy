/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (45%)
 * \co-author Min Khant Ko, 2301320, ko.m@digipen.edu (45%)
 *\ co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (10%)
 * 
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Physics/sysPhysics.h"
#include "../headers/Components/cMove.h"

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

bool Physics::Manager::update() {
    float dt = NIKEEngine.accessWindow()->getDeltaTime();

    // Loop through all entities to reset collision flags
    for (Entity::Type entity : entities) {
        if (NIKEEngine.checkEntityComponent<Collision::Collider>(entity)) {
            Collision::Collider& collider = NIKEEngine.getEntityComponent<Collision::Collider>(entity);
            collider.left = collider.right = collider.top = collider.bottom = false; // Must reset in physics loop

            //cout << "E bounding box min: " << NIKEEngine.getEntityComponent<Collision::Collider>(entity).rect_min.x << ", " << NIKEEngine.getEntityComponent<Collision::Collider>(entity).rect_min.y << endl;
            //cout << "E bounding box max: " << NIKEEngine.getEntityComponent<Collision::Collider>(entity).rect_max.x << ", " << NIKEEngine.getEntityComponent<Collision::Collider>(entity).rect_max.y << endl;
        }
    }
    // Loop through all entities to perform collision checks
    for (Entity::Type entityA : entities) {
        if (NIKEEngine.checkEntityComponent<Transform::Transform>(entityA) &&
            NIKEEngine.checkEntityComponent<Transform::Velocity>(entityA) &&
            NIKEEngine.checkEntityComponent<Collision::Collider>(entityA)) {

            // Loop through all other entities for collision detection
            for (Entity::Type entityB : entities) {
                if (entityA == entityB) {
                    continue; // Skip self-collision
                }

                if (NIKEEngine.checkEntityComponent<Transform::Transform>(entityB) &&
                    NIKEEngine.checkEntityComponent<Collision::Collider>(entityB)) {

                    float firstTimeOfCollision = 0.0f;

                    // Perform AABB collision detection between entityA and entityB
                    Physics::Manager::getInstance()->collision_manager.detectAABBRectRect(entityA, entityB, firstTimeOfCollision);
                }
            }
        }
    }

    // Loop through all entities to perform physics and movement updates
    for (const auto& entity : entities) {
        // Check if entity contains Transform component and Velocity component
        if (NIKEEngine.checkEntityComponent<Transform::Transform>(entity) &&
            NIKEEngine.checkEntityComponent<Transform::Velocity>(entity)) {

            // Reference to transform component
            Transform::Transform& transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

            // Reference to velocity component
            Transform::Velocity& velocity = NIKEEngine.getEntityComponent<Transform::Velocity>(entity);

            const float speed = 100.0f;

            // For entities without move but still have movement
            if (NIKEEngine.checkEntityComponent<Collision::Collider>(entity)) {
                Collision::Collider& collider = NIKEEngine.getEntityComponent<Collision::Collider>(entity);

                // For entities that bounce
                if (collider.bounceFlag == true) {
                    // Adjust velocity based collision flags
                    if (collider.top && (velocity.velocity.y > 0.0f)) {
                        velocity.velocity.y *= -1;
                    }

                    if (collider.bottom && (velocity.velocity.y < 0.0f)) {
                        velocity.velocity.y *= -1;
                    }

                    if (collider.right && (velocity.velocity.x > 0.0f)) {
                        velocity.velocity.x *= -1;
                    }

                    if (collider.left && (velocity.velocity.x < 0.0f)) {
                        velocity.velocity.x *= -1;
                    }
                }
                else {
                    // Adjust velocity based collision flags
                    if (collider.top && (velocity.velocity.y > 0.0f)) {
                        velocity.velocity.y = 0;
                    }

                    if (collider.bottom && (velocity.velocity.y < 0.0f)) {
                        velocity.velocity.y = 0;
                    }

                    if (collider.right && (velocity.velocity.x > 0.0f)) {
                        velocity.velocity.x = 0;
                    }

                    if (collider.left && (velocity.velocity.x < 0.0f)) {
                        velocity.velocity.x = 0;
                    }
                }  
            }

            // Normalize Movement
            if (velocity.velocity.lengthSq() > 0.0f) {
                velocity.velocity = velocity.velocity.normalize();
            }

            // Apply velocity to transform component if no collision in that direction
            transform.position += velocity.velocity * speed * dt;

            // Update collider bounding box
            if (NIKEEngine.checkEntityComponent<Collision::Collider>(entity)) {
                Collision::Collider& collider = NIKEEngine.getEntityComponent<Collision::Collider>(entity);
                collider.rect_min = transform.position - (transform.scale * 0.5f);
                collider.rect_max = transform.position + (transform.scale * 0.5f);
            }
        }
        if (NIKEEngine.checkEntityComponent<Transform::Transform>(entity) &&
            NIKEEngine.checkEntityComponent<Transform::Runtime_Transform>(entity)) {

            // Reference to transform component
            Transform::Transform& c_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

            // Reference to runtime component
            Transform::Runtime_Transform& c_runtime = NIKEEngine.getEntityComponent<Transform::Runtime_Transform>(entity);
            runtimeScaleOrRotate(c_runtime, c_transform);
        }
    }

    return false;
}

