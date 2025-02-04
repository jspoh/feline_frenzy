/*****************************************************************//**
 * \file    sysPhysics.cpp
 * \brief  Physics system for engine
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
#include "Components/cRender.h"  // For Render::Shape (if available)

#include <unordered_map>
#include <vector>

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
        // Phase 1: Update Dynamics, Transforms and Colliders
        //-------------------------------------------------------------------------
        {
            // Get layers
            auto const& layers = NIKE_SCENES_SERVICE->getLayers();

            // Iterate through layers
            for (auto& layer : layers) {

                // Skip inactive layer
                if (!layer->getLayerState())
                    continue;

                // Iterate through all entities
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

                        // Calculate net force and remove expired forces
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

                        // Apply forces to calculate acceleration
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

                    // Update collider transform: if bound to entity, copy transform; else add offset.
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
        // Phase 2: Broad-phase Collision Detection using Spatial Grid
        //-------------------------------------------------------------------------
        {
            // Use an unordered_map keyed by Vector2i (cell coordinates) mapping to a list of entity IDs.
            std::unordered_map<Vector2i, std::vector<Entity::Type>, Vector2iHasher> spatial_grid;
            // Assume NIKE_MAP_SERVICE->getCellSize() returns the size of one cell.
            Vector2f cell_size = NIKE_MAP_SERVICE->getCellSize();
            Physics::Dynamics def_dynamics;  // Default dynamics if an entity does not have one.

            // For simplicity, we use the same layers that were updated.
            auto const& layers = NIKE_SCENES_SERVICE->getLayers();
            for (auto& layer : layers) {
                // Consider only active layers.
                if (!layer->getLayerState())
                    continue;

                // Bucketize colliding entities in this layer.
                for (auto& colliding_entity : layer->getEntitites()) {
                    auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(colliding_entity);
                    if (!e_transform_comp) continue;
                    auto& e_transform = e_transform_comp.value().get();

                    // Only consider entities that have a collider.
                    auto collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(colliding_entity);
                    if (!collider_comp.has_value()) continue;

                    // Compute cell indices based on entity position.
                    int cell_x = static_cast<int>(e_transform.position.x / cell_size.x);
                    int cell_y = static_cast<int>(e_transform.position.y / cell_size.y);
                    Vector2i cell = { cell_x, cell_y };

                    spatial_grid[cell].push_back(colliding_entity);
                }
            }

            //-------------------------------------------------------------------------
            // Phase 3: Narrow-phase Collision Detection within Cells and Neighbors
            //-------------------------------------------------------------------------
            // For each cell in the spatial grid, check collisions with entities in the same cell and adjacent cells.
            for (auto& [cell, cell_entities] : spatial_grid) {
                // Loop over neighboring cell offsets (including current cell)
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        Vector2i neighbor = { cell.x + dx, cell.y + dy };
                        if (spatial_grid.find(neighbor) == spatial_grid.end())
                            continue;
                        auto& neighbor_entities = spatial_grid[neighbor];

                        // For each pair: if same cell, avoid duplicates by starting at index i+1.
                        for (size_t i = 0; i < cell_entities.size(); ++i) {
                            Entity::Type entity_a = cell_entities[i];

                            auto a_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_a);
                            auto a_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_a);
                            auto a_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_a);
                            if (!a_transform_comp.has_value() || !a_collider_comp.has_value())
                                continue;
                            auto& a_transform = a_transform_comp.value().get();
                            auto& a_collider = a_collider_comp.value().get();
                            auto& a_dynamics = a_dynamics_comp.has_value() ? a_dynamics_comp.value().get() : def_dynamics;

                            // For neighbor entities, if in the same cell, start j from i+1; if different, from 0.
                            size_t j_start = (cell == neighbor) ? i + 1 : 0;
                            for (size_t j = j_start; j < neighbor_entities.size(); ++j) {
                                Entity::Type entity_b = neighbor_entities[j];
                                if (entity_a == entity_b)
                                    continue;

                                auto b_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_b);
                                auto b_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_b);
                                auto b_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_b);
                                if (!b_transform_comp.has_value() || !b_collider_comp.has_value())
                                    continue;
                                auto& b_transform = b_transform_comp.value().get();
                                auto& b_collider = b_collider_comp.value().get();
                                auto& b_dynamics = b_dynamics_comp.has_value() ? b_dynamics_comp.value().get() : def_dynamics;

                                // Retrieve model IDs for SAT collision (default "square.model")
                                std::string a_model_id = "square.model";
                                std::string b_model_id = "square.model";
                                auto a_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity_a);
                                if (a_shape_comp.has_value()) {
                                    a_model_id = a_shape_comp.value().get().model_id;
                                }
                                auto b_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity_b);
                                if (b_shape_comp.has_value()) {
                                    b_model_id = b_shape_comp.value().get().model_id;
                                }

                                Collision::CollisionInfo info;
                                bool collisionDetected = false;
                                // Use AABB collision detection if rotations are multiples of 90
                                if (!(static_cast<int>(a_collider.transform.rotation) % 90) &&
                                    !(static_cast<int>(b_collider.transform.rotation) % 90)) {
                                    if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info))
                                        collisionDetected = true;
                                }
                                // Otherwise, try AABB then SAT
                                else if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info) ||
                                    collision_system->detectSATCollision(a_collider, b_collider, a_model_id, b_model_id, info)) {
                                    collisionDetected = true;
                                }

                                if (collisionDetected) {
                                    a_collider.b_collided = true;
                                    b_collider.b_collided = true;

                                    collision_system->collisionResolution(
                                        entity_a, a_transform, a_dynamics, a_collider,
                                        entity_b, b_transform, b_dynamics, b_collider,
                                        info
                                    );
                                }
                                else {
                                    a_collider.b_collided = false;
                                    b_collider.b_collided = false;
                                }
                            } // end for j
                        } // end for i
                    } // end for dy
                } // end for dx
            } // end for each cell in spatial_grid
        }
    }

    void Physics::Manager::onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) {
        collision_system->setRestitution(event->restitution);
        event->setEventProcessed(true);
    }
}
