/*****************************************************************//**
 * \file   sysPhysics.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 *         Min Khant Ko, 2301320, ko.m@digipen.edu (60%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysPhysics.h"
#include "Components/cPhysics.h"
#include "Components/cTransform.h"

namespace NIKE {

    //------------------------------------------------------------------------
    // Physics::Manager::init()
    //------------------------------------------------------------------------
    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    //------------------------------------------------------------------------
    // calculateOccupiedCells()
    //
    // Determine which grid cells an entity occupies. Expand by 1 cell in each direction as a margin, to ensure proper checking.
    //------------------------------------------------------------------------
    std::vector<Vector2i> Physics::Manager::calculateOccupiedCells(const Transform::Transform& transform) {
        std::vector<Vector2i> occupied_cells;

        // Compute entity's AABB
        Vector2f half_size = transform.scale * 0.5f;
        Vector2f world_min = transform.position - half_size;
        Vector2f world_max = transform.position + half_size;

        // Convert corners to cell indices
        auto min_cell_opt = NIKE_MAP_SERVICE->getCellIndexFromCords(world_min);
        auto max_cell_opt = NIKE_MAP_SERVICE->getCellIndexFromCords(world_max);

        // If out-of-bounds, or can't get cell indices, just return empty
        if (!min_cell_opt || !max_cell_opt) {
            return occupied_cells;
        }

        Vector2i min_cell = min_cell_opt.value();
        Vector2i max_cell = max_cell_opt.value();

        // Expand by 1 cell each side
        min_cell.x = Utility::getMax(0, min_cell.x - 1);
        min_cell.y = Utility::getMax(0, min_cell.y - 1);
        max_cell.x = Utility::getMin(NIKE_MAP_SERVICE->getGridSize().x - 1, max_cell.x + 1);
        max_cell.y = Utility::getMin(NIKE_MAP_SERVICE->getGridSize().y - 1, max_cell.y + 1);

        // Fill all cells in range
        for (int y = min_cell.y; y <= max_cell.y; ++y) {
            for (int x = min_cell.x; x <= max_cell.x; ++x) {
                occupied_cells.emplace_back(x, y);
            }
        }

        return occupied_cells;
    }

    //------------------------------------------------------------------------
    // updateEntityPhysics()
    //
    // Updates forces, velocity, transform, and the set of occupied cells.
    //------------------------------------------------------------------------
    void Physics::Manager::updateEntityPhysics(Entity::Type entity, float dt) {
        // Get transform
        auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
        if (!e_transform_comp) return;
        auto& e_transform = e_transform_comp.value().get();

        // Optionally get dynamics
        auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
        if (e_dynamics_comp) {
            auto& e_dynamics = e_dynamics_comp.value().get();

            // Ensure mass not negative
            e_dynamics.mass = (e_dynamics.mass == 0.0f) ? EPSILON : e_dynamics.mass;

            // Acceleration from force
            Vector2f acceleration = e_dynamics.force / e_dynamics.mass;

            // Update velocity
            e_dynamics.velocity += acceleration * dt;

            // Add drag
            e_dynamics.velocity -= e_dynamics.velocity * e_dynamics.drag * dt;

            // Cap velocity
            if (e_dynamics.velocity.length() > e_dynamics.max_speed) {
                e_dynamics.velocity = e_dynamics.velocity.normalized() * e_dynamics.max_speed;
            }

            // Zero if below EPSILON
            if (e_dynamics.velocity.length() < EPSILON) {
                e_dynamics.velocity = { 0.0f, 0.0f };
            }

            // Update position
            e_transform.position.x += e_dynamics.velocity.x * dt;
            e_transform.position.y += e_dynamics.velocity.y * dt;
        }

        // Update collider transform and occupied cells
        auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
        if (e_collider_comp) {
            auto& e_collider = e_collider_comp.value().get();

            // If bound to entity, copy transform
            if (e_collider.b_bind_to_entity) {
                e_collider.transform = e_transform;
            }
            else {
                e_collider.transform.position = e_transform.position + e_collider.pos_offset;
            }

            //Update vertices function
            auto updateVertices = [&e_collider](std::vector<Assets::Vertex> const& vertices) {

                //Clear all vertices
                e_collider.vertices.clear();

                //Get World size
                auto world_size = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();

                //Get window size
                auto window_size = NIKE_WINDOWS_SERVICE->getWindow()->getViewportSize();

                //Calculate scale factor
                Vector2f scale_factor = { static_cast<float>(window_size.x) / static_cast<float>(world_size.x), static_cast<float>(window_size.y) / static_cast<float>(world_size.y) };

                //Update vertices
                for (const Assets::Vertex& v : vertices) {
                    e_collider.vertices.push_back(v.pos);
                }

                for (auto& point : e_collider.vertices) {
                    point.x *= (e_collider.transform.scale.x * scale_factor.x);
                    point.y *= (e_collider.transform.scale.y * scale_factor.y);
                    point.x += (e_collider.transform.position.x * scale_factor.x);
                    point.y -= (e_collider.transform.position.y * scale_factor.y);

                    //Translate model to world coordinates
                    point.x += (window_size.x / 2.0f);
                    point.y += (window_size.y / 2.0f);
                }
                };

            // Update collider vertices
            auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
            if (e_shape_comp.has_value()) {
                auto const& e_shape = e_shape_comp.value().get();

                //Check if model is registered
                if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {

                    //Get vertices
                    std::vector<Assets::Vertex>& vertices = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(e_shape.model_id)->vertices;

                    //Update vertices
                    updateVertices(vertices);
                }
            }
            else {
                if (NIKE_ASSETS_SERVICE->isAssetRegistered("square-texture.model")) {

                    //Get vertices
                    std::vector<Assets::Vertex>& vertices = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model")->vertices;

                    //Update vertices
                    updateVertices(vertices);
                }
            }

            // Reset collision flag
            e_collider.b_collided = false;

            // Finally, record occupied cells
            entity_occupied_cells[entity] = calculateOccupiedCells(e_collider.transform);
        }
    }

    //------------------------------------------------------------------------
    // getPotentialCollisions()
    //
    // 1) Build collision pairs from shared cells
    // 2) Hard-coded hack: if entity name is "left_wall", "right_wall", etc.,
    //    forcibly collide with *all* other colliders. (TODO, optimise)
    //------------------------------------------------------------------------
    std::vector<std::pair<Entity::Type, Entity::Type>> Physics::Manager::getPotentialCollisions() {
        std::vector<std::pair<Entity::Type, Entity::Type>> pairs;
        std::set<std::pair<Entity::Type, Entity::Type>> processed_pairs;

        // (A) Normal broad-phase from overlapping cells
        for (const auto& [entity, cells] : entity_occupied_cells) {
            // For each cell occupied by this entity, gather any other entity that shares it
            std::unordered_map<Vector2i, std::vector<Entity::Type>, Vector2iHasher> cell_map;

            for (auto& cell_pos : cells) {
                for (auto& [other_entity, other_cells] : entity_occupied_cells) {
                    if (entity == other_entity) continue;
                    // If other occupies the same cell
                    if (std::find(other_cells.begin(), other_cells.end(), cell_pos) != other_cells.end()) {
                        cell_map[cell_pos].push_back(other_entity);
                    }
                }
            }

            // Generate unique pairs from those cell matches
            for (auto& [cell_pos, occupant_list] : cell_map) {
                for (Entity::Type other_entity : occupant_list) {
                    // Sort pair so smaller ID is first
                    auto pair = Utility::getMinMax(entity, other_entity);
                    if (processed_pairs.insert(pair).second) {
                        pairs.push_back(pair);
                    }
                }
            }
        }

        // (B) Hard-coded "wall" collisions: forcibly pair any entity that is very large.
        auto isVeryLarge = [&](Entity::Type e) -> bool {
            // Attempt to get the Transform::Transform component
            auto transform_opt = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(e);
            if (!transform_opt) {
                return false; // No transform => definitely not "large"
            }

            // Check scale
            auto& transform = transform_opt.value().get();
            float width = transform.scale.x;
            float height = transform.scale.y;

            // If bigger than 1500 in either dimension, treat as a "wall"
            return (width >= 1500.0f || height >= 1500.0f);
            };

        // Collect "large" entities and all other colliders
        std::vector<Entity::Type> large_entities;
        std::vector<Entity::Type> all_colliders;

        for (auto& [ent, occupiedCells] : entity_occupied_cells) {
            auto col_opt = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(ent);
            if (!col_opt.has_value()) {
                continue; // skip if no collider
            }

            if (isVeryLarge(ent)) {
                large_entities.push_back(ent);
            }
            else {
                all_colliders.push_back(ent);
            }
        }

        // For each large entity, forcibly pair it with all other colliders
        for (auto large_ent : large_entities) {
            for (auto c : all_colliders) {
                if (large_ent == c) continue;
                auto pair = Utility::getMinMax(large_ent, c);
                if (processed_pairs.insert(pair).second) {
                    pairs.push_back(pair);
                }
            }
        }

        return pairs;
    }

    //------------------------------------------------------------------------
    // processCollisions()
    //
    // For each potential pair, do final collision detection (AABB, SAT).
    //------------------------------------------------------------------------
    void Physics::Manager::processCollisions(const std::vector<std::pair<Entity::Type, Entity::Type>>& collision_pairs) {
        for (auto& [entity_a, entity_b] : collision_pairs) {
            // Grab components
            auto a_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_a);
            auto a_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_a);
            auto a_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_a);

            auto b_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_b);
            auto b_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_b);
            auto b_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_b);

            if (!a_transform_comp.has_value() || !a_collider_comp.has_value() ||
                !b_transform_comp.has_value() || !b_collider_comp.has_value()) {
                continue;
            }

            // Refs
            auto& a_transform = a_transform_comp.value().get();
            auto& a_collider = a_collider_comp.value().get();

            Physics::Dynamics def_dynA;
            auto& a_dynamics = a_dynamics_comp.has_value() ? a_dynamics_comp.value().get() : def_dynA;

            auto& b_transform = b_transform_comp.value().get();
            auto& b_collider = b_collider_comp.value().get();

            Physics::Dynamics def_dynB;
            auto& b_dynamics = b_dynamics_comp.has_value() ? b_dynamics_comp.value().get() : def_dynB;

            // For shape-based collisions (SAT)
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

            // Record how many collision checks we do, just for logging
            collision_checks_count[entity_a]++;
            collision_checks_count[entity_b]++;

            // AABB vs. SAT
            Collision::CollisionInfo info;
            bool collision_detected = false;

            // If both rotations are multiples of 90, do simpler AABB
            if (!(static_cast<int>(a_collider.transform.rotation) % 90) &&
                !(static_cast<int>(b_collider.transform.rotation) % 90)) {
                collision_detected = collision_system->detectAABBRectRect(
                    a_dynamics, a_collider, b_dynamics, b_collider, info);
            }
            // Otherwise, check AABB or fallback to SAT
            else {
                collision_detected = collision_system->detectAABBRectRect(
                    a_dynamics, a_collider, b_dynamics, b_collider, info)
                    || collision_system->detectSATCollision(
                        a_collider, b_collider, a_model_id, b_model_id, info);
            }

            // If collided, do resolution
            if (collision_detected) {
                a_collider.b_collided = true;
                b_collider.b_collided = true;

                collision_system->collisionResolution(
                    entity_a, a_transform, a_dynamics, a_collider,
                    entity_b, b_transform, b_dynamics, b_collider,
                    info
                );
            }
        }
    }

    //------------------------------------------------------------------------
    // Physics::Manager::update()
    //
    // 1) Clear collisions
    // 2) Per-step: update physics, build potential pairs, process collisions
    //------------------------------------------------------------------------
    void Physics::Manager::update() {
        // Clear any collisions from last frame
        collision_system->clearProcessedCollisions();

        // Clear old mappings
        entity_occupied_cells.clear();
        collision_checks_count.clear();

        // Get dt
        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

        // Grab layers
        auto const& layers = NIKE_SCENES_SERVICE->getLayers();

        // Up to 5 steps
        constexpr int MAX_PHYSICS_STEPS = 5;
        int steps = Utility::getMin(NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(), MAX_PHYSICS_STEPS);

        // For each step
        for (int s = 0; s < steps; ++s) {

            // (A) Update physics on all relevant entities
            for (auto& layer : layers) {
                if (!layer->getLayerState()) continue; // skip inactive

                for (auto& entity : layer->getEntitites()) {
                    if (entities.find(entity) == entities.end()) continue;
                    updateEntityPhysics(entity, dt);
                }
            }

            // (B) Build potential collision pairs
            auto collision_pairs = getPotentialCollisions();

            // (C) Process collisions
            processCollisions(collision_pairs);

            // (D) Logging how many checks each entity did
            /*for (auto& [ent, checks] : collision_checks_count) {
                NIKEE_CORE_WARN("Entity {} underwent {} collision checks this frame.", ent, checks);
            }*/
        }
    }

    //------------------------------------------------------------------------
    // Physics::Manager::onEvent(ChangePhysicsEvent)
    //------------------------------------------------------------------------
    void Physics::Manager::onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) {
        collision_system->setRestitution(event->restitution);
        event->setEventProcessed(true);
    }
} // namespace NIKE
