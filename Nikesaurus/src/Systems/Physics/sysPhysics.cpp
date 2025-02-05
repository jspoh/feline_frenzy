/*****************************************************************//**
 * \file   sysPhysics.cpp
 * \brief  Physics system for engine with broad-phase collision detection
 *         that properly handles large entities spanning many cells.
 *
 * This version uses the map grid to determine which cells an entity's
 * collider occupies. Instead of processing each cell independently,
 * for each entity we compute the union of all grid cells its AABB covers,
 * expand that region by one cell in all directions, and then gather all
 * candidate entities from that region for narrow-phase collision checking.
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
 // For accessing map service (grid size, cell size, etc.)
#include "Managers/Services/sMap.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>

namespace NIKE {

    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    void Physics::Manager::update() {

        // Clear processed collisions at the start.
        collision_system->clearProcessedCollisions();

        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();
        auto const& layers = NIKE_SCENES_SERVICE->getLayers();

        //-------------------------------------------------------------------------
        // Phase 1: Update Dynamics, Transforms and Collider Transforms.
        //-------------------------------------------------------------------------
        for (int step = 0; step < NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(); ++step) {
            for (auto& layer : layers) {
                if (!layer->getLayerState())
                    continue;

                for (auto& entity : layer->getEntitites()) {
                    auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                    if (!e_transform_comp) continue;
                    auto& e_transform = e_transform_comp.value().get();

                    auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                    if (e_dynamics_comp) {
                        auto& e_dynamics = e_dynamics_comp.value().get();
                        e_dynamics.mass = (e_dynamics.mass == 0.0f) ? EPSILON : e_dynamics.mass;
                        Vector2f acceleration = e_dynamics.force / e_dynamics.mass;
                        e_dynamics.velocity += acceleration * dt;
                        e_dynamics.velocity -= e_dynamics.velocity * e_dynamics.drag * dt;
                        if (e_dynamics.velocity.length() > e_dynamics.max_speed)
                            e_dynamics.velocity = e_dynamics.velocity.normalize() * e_dynamics.max_speed;
                        if (e_dynamics.velocity.length() < EPSILON)
                            e_dynamics.velocity = { 0.0f, 0.0f };
                        e_transform.position.x += e_dynamics.velocity.x * dt;
                        e_transform.position.y += e_dynamics.velocity.y * dt;
                    }

                    auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                    if (e_collider_comp.has_value()) {
                        auto& e_collider = e_collider_comp.value().get();
                        if (e_collider.b_bind_to_entity)
                            e_collider.transform = e_transform;
                        else
                            e_collider.transform.position = e_transform.position + e_collider.pos_offset;
                    }
                }
            }
        }

       //-------------------------------------------------------------------------
       // Phase 2: Build Spatial Grid and Entity-to-Cells Mapping.
       //-------------------------------------------------------------------------
       // We compute each entity’s AABB and determine all grid cells it overlaps.
        std::unordered_map<Vector2i, std::vector<Entity::Type>, Vector2iHasher> spatial_grid;
        // Also, map each entity to its set of occupied grid cells.
        std::unordered_map<Entity::Type, std::vector<Vector2i>> entityCells;
        Vector2f cell_size = NIKE_MAP_SERVICE->getCellSize();

        for (auto& layer : layers) {
            if (!layer->getLayerState())
                continue;

            for (auto& entity : layer->getEntitites()) {
                auto collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                if (!collider_comp.has_value()) continue;
                auto& collider = collider_comp.value().get();

                Vector2f aabb_min, aabb_max;
                // If the collider defines vertices, use them to compute a more accurate AABB.
                if (!collider.vertices.empty()) {
                    std::vector<Vector2f> worldVertices;
                    float angleRad = collider.transform.rotation * static_cast<float>(M_PI) / 180.0f;
                    float cosAngle = cos(angleRad);
                    float sinAngle = sin(angleRad);
                    for (auto& v : collider.vertices) {
                        // Apply scale.
                        Vector2f scaled = { v.x * collider.transform.scale.x, v.y * collider.transform.scale.y };
                        // Rotate.
                        Vector2f rotated = { scaled.x * cosAngle - scaled.y * sinAngle, scaled.x * sinAngle + scaled.y * cosAngle };
                        // Translate.
                        Vector2f worldPos = collider.transform.position + rotated;
                        worldVertices.push_back(worldPos);
                    }
                    aabb_min = worldVertices[0];
                    aabb_max = worldVertices[0];
                    for (auto& wv : worldVertices) {
                        if (wv.x < aabb_min.x) aabb_min.x = wv.x;
                        if (wv.y < aabb_min.y) aabb_min.y = wv.y;
                        if (wv.x > aabb_max.x) aabb_max.x = wv.x;
                        if (wv.y > aabb_max.y) aabb_max.y = wv.y;
                    }
                }
                else {
                    // Fallback: assume the collider is centered at transform.position with half-extents = scale/2.
                    Vector2f halfExtents = collider.transform.scale * 0.5f;
                    aabb_min = collider.transform.position - halfExtents;
                    aabb_max = collider.transform.position + halfExtents;
                }

                int min_cell_x = static_cast<int>(std::floor(aabb_min.x / cell_size.x));
                int max_cell_x = static_cast<int>(std::floor(aabb_max.x / cell_size.x));
                int min_cell_y = static_cast<int>(std::floor(aabb_min.y / cell_size.y));
                int max_cell_y = static_cast<int>(std::floor(aabb_max.y / cell_size.y));

                std::vector<Vector2i> cells;
                for (int cx = min_cell_x; cx <= max_cell_x; ++cx) {
                    for (int cy = min_cell_y; cy <= max_cell_y; ++cy) {
                        Vector2i cell = { cx, cy };
                        spatial_grid[cell].push_back(entity);
                        cells.push_back(cell);
                    }
                }
                entityCells[entity] = cells;
            }
        }

        //-------------------------------------------------------------------------
        // Phase 3: For Each Entity, Gather Nearby Candidates from the Union
        // of its occupied cells (expanded by one cell margin) and perform narrow-phase checks.
        //-------------------------------------------------------------------------
        // We'll use a set to avoid duplicate collision checks.
        std::unordered_set<std::pair<Entity::Type, Entity::Type>, Collision::System::EntityPairHash> checkedPairs;

        // Get the grid size (number of cells in each direction) from the map service.
        Vector2i gridSize = NIKE_MAP_SERVICE->getGridSize();

        // Default dynamics placeholder.
        Physics::Dynamics def_dynamics;

        for (auto& [entity, cells] : entityCells) {
            // Compute the union of cells occupied by the entity.
            int minX = cells[0].x, minY = cells[0].y, maxX = cells[0].x, maxY = cells[0].y;
            for (const auto& cell : cells) {
                if (cell.x < minX) minX = cell.x;
                if (cell.y < minY) minY = cell.y;
                if (cell.x > maxX) maxX = cell.x;
                if (cell.y > maxY) maxY = cell.y;
            }
            // Expand the union by one cell in every direction.
            minX -= 1; minY -= 1; maxX += 1; maxY += 1;
            // Clamp to valid grid cells.
            minX = Utility::getMin(minX, 0);
            minY = Utility::getMax(minY, 0);
            maxX = Utility::getMin(maxX, gridSize.x - 1);
            maxY = Utility::getMax(maxY, gridSize.y - 1);

            // Gather candidate entities from all cells in the expanded region.
            std::unordered_set<Entity::Type> candidates;
            for (int cx = minX; cx <= maxX; ++cx) {
                for (int cy = minY; cy <= maxY; ++cy) {
                    Vector2i cell = { cx, cy };
                    if (spatial_grid.find(cell) != spatial_grid.end()) {
                        for (auto cand : spatial_grid[cell]) {
                            // Avoid adding self.
                            if (cand != entity)
                                candidates.insert(cand);
                        }
                    }
                }
            }

            // Retrieve components for the primary entity.
            auto a_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
            auto a_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
            auto a_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
            if (!a_transform_comp.has_value() || !a_collider_comp.has_value())
                continue;
            auto& a_transform = a_transform_comp.value().get();
            auto& a_collider = a_collider_comp.value().get();
            auto& a_dynamics = a_dynamics_comp.has_value() ? a_dynamics_comp.value().get() : def_dynamics;

            // For each candidate, perform narrow-phase collision detection.
            for (auto candidate : candidates) {
                // Avoid duplicate checks (order the pair by id).
                auto pair = (entity < candidate) ? std::make_pair(entity, candidate)
                    : std::make_pair(candidate, entity);
                if (checkedPairs.find(pair) != checkedPairs.end())
                    continue;
                checkedPairs.insert(pair);

                auto b_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(candidate);
                auto b_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(candidate);
                auto b_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(candidate);
                if (!b_transform_comp.has_value() || !b_collider_comp.has_value())
                    continue;
                auto& b_transform = b_transform_comp.value().get();
                auto& b_collider = b_collider_comp.value().get();
                auto& b_dynamics = b_dynamics_comp.has_value() ? b_dynamics_comp.value().get() : def_dynamics;

                // (Optional) Quick distance check.
                if ((a_collider.transform.position - b_collider.transform.position).lengthSq() >
                    (a_collider.transform.scale + b_collider.transform.scale).lengthSq())
                    continue;

                // Retrieve model IDs for SAT collision (default "square.model").
                std::string a_model_id = "square.model";
                std::string b_model_id = "square.model";
                auto a_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
                if (a_shape_comp.has_value())
                    a_model_id = a_shape_comp.value().get().model_id;
                auto b_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(candidate);
                if (b_shape_comp.has_value())
                    b_model_id = b_shape_comp.value().get().model_id;

                Collision::CollisionInfo info;
                bool collisionDetected = false;
                // Use AABB if both colliders are axis-aligned (rotation multiple of 90).
                if (!(static_cast<int>(a_collider.transform.rotation) % 90) &&
                    !(static_cast<int>(b_collider.transform.rotation) % 90)) {
                    if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info))
                        collisionDetected = true;
                }
                // Otherwise, try AABB then SAT.
                else if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info) ||
                    collision_system->detectSATCollision(a_collider, b_collider, a_model_id, b_model_id, info)) {
                    collisionDetected = true;
                }

                if (collisionDetected) {
                    a_collider.b_collided = true;
                    b_collider.b_collided = true;
                    collision_system->collisionResolution(
                        entity, a_transform, a_dynamics, a_collider,
                        candidate, b_transform, b_dynamics, b_collider,
                        info
                    );
                }
                else {
                    a_collider.b_collided = false;
                    b_collider.b_collided = false;
                }
            }
        }

        // End of update.
    }

    void Physics::Manager::onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) {
        collision_system->setRestitution(event->restitution);
        event->setEventProcessed(true);
    }
}
