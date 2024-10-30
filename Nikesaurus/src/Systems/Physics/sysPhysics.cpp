/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (45%)
 * \co-author Min Khant Ko, 2301320, ko.m@digipen.edu (45%)
 *\ co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (10%)
 * 
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysPhysics.h"
#include "Components/cMove.h"
#include "Components/cTransform.h"


namespace NIKESAURUS {
    void Physics::Manager::init() {

    }

    void Physics::Manager::update() {
        float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

        for (auto& layer : NIKE_ENGINE.getService<Scenes::Service>()->getCurrScene()->getLayers()) {
            for (auto& entity : entities) {
                if (!layer->checkEntity(entity))
                    continue;

                if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Velocity>(entity)) {
                    auto& e_transform = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Transform>(entity);
                    auto& e_velo = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Velocity>(entity);

                    e_transform.position.x += e_velo.velocity.x * dt;
                    e_transform.position.y += e_velo.velocity.y * dt;
                }
            }
        }

        //// Loop through all entities to reset collision flags
        //for (Entity::Type entity : entities) {
        //    if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Collision::Collider>(entity)) {
        //        Collision::Collider& collider = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity);
        //        collider.left = collider.right = collider.top = collider.bottom = false; // Must reset in physics loop

        //        //cout << "E bounding box min: " << NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity).rect_min.x << ", " << NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity).rect_min.y << endl;
        //        //cout << "E bounding box max: " << NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity).rect_max.x << ", " << NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity).rect_max.y << endl;
        //    }
        //}
        //// Loop through all entities to perform collision checks
        //for (Entity::Type entityA : entities) {
        //    if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Transform>(entityA) &&
        //        NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Velocity>(entityA) &&
        //        NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Collision::Collider>(entityA)) {

        //        // Loop through all other entities for collision detection
        //        for (Entity::Type entityB : entities) {
        //            if (entityA == entityB) {
        //                continue; // Skip self-collision
        //            }

        //            if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Transform>(entityB) &&
        //                NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Collision::Collider>(entityB)) {

        //                float firstTimeOfCollision = 0.0f;

        //                // Perform AABB collision detection between entityA and entityB
        //                Physics::Manager::getInstance()->collision_manager.detectAABBRectRect(entityA, entityB, firstTimeOfCollision);
        //            }
        //        }
        //    }
        //}

        //// Loop through all entities to perform physics and movement updates
        //for (const auto& entity : entities) {
        //    // Check if entity contains Transform component and Velocity component
        //    if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Transform>(entity) &&
        //        NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Velocity>(entity)) {

        //        // Reference to transform component
        //        Transform::Transform& transform = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Transform>(entity);

        //        // Reference to velocity component
        //        Transform::Velocity& velocity = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Velocity>(entity);

        //        const float speed = 100.0f;

        //        // For entities without move but still have movement
        //        if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Collision::Collider>(entity)) {
        //            Collision::Collider& collider = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity);

        //            // For entities that bounce
        //            if (collider.bounceFlag == true) {
        //                // Adjust velocity based collision flags
        //                if (collider.top && (velocity.velocity.y > 0.0f)) {
        //                    velocity.velocity.y *= -1;
        //                }

        //                if (collider.bottom && (velocity.velocity.y < 0.0f)) {
        //                    velocity.velocity.y *= -1;
        //                }

        //                if (collider.right && (velocity.velocity.x > 0.0f)) {
        //                    velocity.velocity.x *= -1;
        //                }

        //                if (collider.left && (velocity.velocity.x < 0.0f)) {
        //                    velocity.velocity.x *= -1;
        //                }
        //            }
        //            else {
        //                // Adjust velocity based collision flags
        //                if (collider.top && (velocity.velocity.y > 0.0f)) {
        //                    velocity.velocity.y = 0;
        //                }

        //                if (collider.bottom && (velocity.velocity.y < 0.0f)) {
        //                    velocity.velocity.y = 0;
        //                }

        //                if (collider.right && (velocity.velocity.x > 0.0f)) {
        //                    velocity.velocity.x = 0;
        //                }

        //                if (collider.left && (velocity.velocity.x < 0.0f)) {
        //                    velocity.velocity.x = 0;
        //                }
        //            }
        //        }

        //        // Normalize Movement
        //        if (velocity.velocity.lengthSq() > 0.0f) {
        //            velocity.velocity = velocity.velocity.normalize();
        //        }

        //        // Apply velocity to transform component if no collision in that direction
        //        transform.position += velocity.velocity * speed * dt;

        //        // Update collider bounding box
        //        if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Collision::Collider>(entity)) {
        //            Collision::Collider& collider = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Collision::Collider>(entity);
        //            collider.rect_min = transform.position - (transform.scale * 0.5f);
        //            collider.rect_max = transform.position + (transform.scale * 0.5f);
        //        }
        //    }
        //    if (NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Transform>(entity) &&
        //        NIKE_ENGINE.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Runtime_Transform>(entity)) {

        //        // Reference to transform component
        //        Transform::Transform& c_transform = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Transform>(entity);

        //        // Reference to runtime component
        //        Transform::Runtime_Transform& c_runtime = NIKE_ENGINE.getService<Coordinator::Manager>()->getEntityComponent<Transform::Runtime_Transform>(entity);
        //        runtimeScaleOrRotate(c_runtime, c_transform);
        //    }
        //}
    }
}
