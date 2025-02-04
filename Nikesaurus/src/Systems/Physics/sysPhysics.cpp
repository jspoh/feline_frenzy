/*****************************************************************//**
 * \file   sysPhysics.cpp
 * \brief  Physics system for engine with added broad-phase collision detection.
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (45%)
 * \co-author Min Khant Ko, 2301320, ko.m@digipen.edu (45%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (10%)
 *
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysPhysics.h"
#include "Components/cPhysics.h"
#include "Components/cTransform.h"
#include "Components/cRender.h" // For Render::Shape component (if available)

namespace NIKE {

    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    void Physics::Manager::update() {

        // Clear processed collisions
        collision_system->clearProcessedCollisions();

        // Delta time
        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

        //-------------------------------------------------------------------------
        // Phase 1: Update Dynamics and Transforms
        //-------------------------------------------------------------------------
        for (int step = 0; step < NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(); ++step) {

            // Iterate through layers
            for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

                // Skip inactive layer
                if (!layer->getLayerState())
                    continue;

                // Iterate through all entities in the layer
                for (auto& entity : layer->getEntitites()) {

                    // Skip entities with no transform
                    auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                    if (!e_transform_comp) continue;
                    auto& e_transform = e_transform_comp.value().get();

                    // Update entities with dynamics
                    auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                    if (e_dynamics_comp) {

                        auto& e_dynamics = e_dynamics_comp.value().get();

                        // Ensure that mass is not zero (avoid division by zero)
                        e_dynamics.mass = (e_dynamics.mass == 0.0f) ? EPSILON : e_dynamics.mass;

                        // Calculate net force from active forces and remove expired forces
                        Vector2f net_force;
                        for (auto it = e_dynamics.forces.begin(); it != e_dynamics.forces.end();) {
                            it->life_time -= dt;
                            if (it->life_time > 0.0f) {
                                net_force += it->direction;
                                ++it;
                            }
                            else {
                                it = e_dynamics.forces.erase(it);
                            }
                        }

                        // Apply force and mass to calculate acceleration
                        Vector2f acceleration = e_dynamics.force / e_dynamics.mass;

                        // Update velocity based on acceleration
                        e_dynamics.velocity += acceleration * dt;

                        // Apply drag/friction
                        e_dynamics.velocity -= e_dynamics.velocity * e_dynamics.drag * dt;

                        // Cap velocity
                        if (e_dynamics.velocity.length() > e_dynamics.max_speed) {
                            e_dynamics.velocity = e_dynamics.velocity.normalize() * e_dynamics.max_speed;
                        }

                        // Set velocity to zero if too small
                        if (e_dynamics.velocity.length() < EPSILON) {
                            e_dynamics.velocity = { 0.0f, 0.0f };
                        }

                        // Update Transform position based on velocity
                        e_transform.position.x += e_dynamics.velocity.x * dt;
                        e_transform.position.y += e_dynamics.velocity.y * dt;
                    }

                    // Update collider transform (bound to entity or with offset)
                    auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                    if (e_collider_comp.has_value()) {
                        auto& e_collider = e_collider_comp.value().get();
                        if (e_collider.b_bind_to_entity) {
                            e_collider.transform = e_transform;
                        }
                        else {
                            e_collider.transform.position = e_transform.position + e_collider.pos_offset;
                        }
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        // Phase 2: Build Spatial Hash for Broad-Phase Collision Detection
        //-------------------------------------------------------------------------
        // Create a spatial hash that maps grid cell indices to a list of entity IDs.
        std::unordered_map<Vector2i, std::vector<Entity::Type>, Vector2iHasher> spatialHash;

        // Iterate through active layers and bucketize entities that have colliders.
        for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
            if (!layer->getLayerState())
                continue;

            for (auto& entity : layer->getEntitites()) {
                auto collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                if (!collider_comp.has_value())
                    continue;

                auto transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                if (!transform_comp.has_value())
                    continue;
                auto& transform = transform_comp.value().get();

                // Use the map service to get the grid cell index for the entity's position.
                auto cellIndexOpt = NIKE_MAP_SERVICE->getCellIndexFromCords(transform.position);
                if (!cellIndexOpt.has_value())
                    continue;
                Vector2i cellIndex = cellIndexOpt.value();

                spatialHash[cellIndex].push_back(entity);
            }
        }

        //-------------------------------------------------------------------------
        // Phase 3: Narrow-Phase Collision Detection among Entities in Neighboring Cells
        //-------------------------------------------------------------------------
        // For each cell, check collisions against entities in that cell and the 8 surrounding cells.
        for (auto& cellPair : spatialHash) {
            Vector2i cellIndex = cellPair.first;
            // Loop through neighboring cell offsets (including current cell: dx=0, dy=0)
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    Vector2i neighborIndex = { cellIndex.x + dx, cellIndex.y + dy };
                    if (spatialHash.find(neighborIndex) == spatialHash.end())
                        continue;

                    // For each pair of entities between the current cell and this neighbor cell
                    for (Entity::Type entityA : cellPair.second) {
                        for (Entity::Type entityB : spatialHash[neighborIndex]) {
                            if (entityA == entityB)
                                continue;
                            // Avoid duplicate checks: in the same cell, only process if entityA < entityB.
                            if (cellIndex == neighborIndex && entityA >= entityB)
                                continue;

                            // Retrieve components for both entities.
                            auto transformA_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entityA);
                            auto colliderA_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entityA);
                            auto dynamicsA_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entityA);

                            auto transformB_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entityB);
                            auto colliderB_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entityB);
                            auto dynamicsB_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entityB);

                            if (!transformA_comp.has_value() || !colliderA_comp.has_value() ||
                                !transformB_comp.has_value() || !colliderB_comp.has_value())
                                continue;

                            auto& transformA = transformA_comp.value().get();
                            auto& colliderA = colliderA_comp.value().get();
                            auto& transformB = transformB_comp.value().get();
                            auto& colliderB = colliderB_comp.value().get();

                            // Use default dynamics if an entity does not have one.
                            Physics::Dynamics def_dynamics;
                            auto& dynamicsA = dynamicsA_comp.has_value() ? dynamicsA_comp.value().get() : def_dynamics;
                            auto& dynamicsB = dynamicsB_comp.has_value() ? dynamicsB_comp.value().get() : def_dynamics;

                            // Update collider transforms (if not bound to entity)
                            if (colliderA.b_bind_to_entity)
                                colliderA.transform = transformA;
                            else
                                colliderA.transform.position = transformA.position + colliderA.pos_offset;

                            if (colliderB.b_bind_to_entity)
                                colliderB.transform = transformB;
                            else
                                colliderB.transform.position = transformB.position + colliderB.pos_offset;

                            // Retrieve model IDs for SAT collision if available (default is "square.model")
                            std::string modelA = "square.model";
                            std::string modelB = "square.model";
                            auto shapeA_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entityA);
                            if (shapeA_comp.has_value()) {
                                modelA = shapeA_comp.value().get().model_id;
                            }
                            auto shapeB_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entityB);
                            if (shapeB_comp.has_value()) {
                                modelB = shapeB_comp.value().get().model_id;
                            }

                            Collision::CollisionInfo info;
                            bool collisionDetected = false;
                            // Use AABB collision detection if rotations are multiples of 180; otherwise, use SAT.
                            if (!(static_cast<int>(colliderA.transform.rotation) % 180) &&
                                !(static_cast<int>(colliderB.transform.rotation) % 180)) {

                                if (collision_system->detectAABBRectRect(dynamicsA, colliderA, dynamicsB, colliderB, info))
                                    collisionDetected = true;
                            }
                            else if (collision_system->detectSATCollision(colliderA, colliderB, modelA, modelB, info)) {
                                collisionDetected = true;
                            }

                            if (collisionDetected) {
                                // Mark colliders as colliding
                                colliderA.b_collided = true;
                                colliderB.b_collided = true;

                                // Perform collision resolution
                                collision_system->collisionResolution(
                                    entityA, transformA, dynamicsA, colliderA,
                                    entityB, transformB, dynamicsB, colliderB,
                                    info
                                );
                            }
                            else {
                                colliderA.b_collided = false;
                                colliderB.b_collided = false;
                            }
                        }
                    }
                }
            }
        }
    }

    void Physics::Manager::onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) {
        collision_system->setRestitution(event->restitution);
        event->setEventProcessed(true);
    }
}
