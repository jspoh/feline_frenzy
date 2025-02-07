/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Original authors preserved
 * \date   February 2025
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysPhysics.h"
#include "Components/cPhysics.h"
#include "Components/cTransform.h"

namespace NIKE {
    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    std::vector<Vector2i> Physics::Manager::calculateOccupiedCells(const Transform::Transform& transform) {
        std::vector<Vector2i> occupied_cells;

        // Calculate entity's AABB in world space
        Vector2f half_size = transform.scale * 0.5f;
        Vector2f world_min = transform.position - half_size;
        Vector2f world_max = transform.position + half_size;

        // Get cell indices for corners
        auto min_cell_opt = NIKE_MAP_SERVICE->getCellIndexFromCords(world_min);
        auto max_cell_opt = NIKE_MAP_SERVICE->getCellIndexFromCords(world_max);

        if (!min_cell_opt || !max_cell_opt) return occupied_cells;

        Vector2i min_cell = min_cell_opt.value();
        Vector2i max_cell = max_cell_opt.value();

        // Add one cell margin in each direction
        min_cell.x = Utility::getMax(0, min_cell.x - 1);
        min_cell.y = Utility::getMax(0, min_cell.y - 1);
        max_cell.x = Utility::getMin(NIKE_MAP_SERVICE->getGridSize().x - 1, max_cell.x + 1);
        max_cell.y = Utility::getMin(NIKE_MAP_SERVICE->getGridSize().y - 1, max_cell.y + 1);

        // Collect all cells in the range
        for (int y = min_cell.y; y <= max_cell.y; ++y) {
            for (int x = min_cell.x; x <= max_cell.x; ++x) {
                occupied_cells.emplace_back(x, y);
            }
        }

        return occupied_cells;
    }

    void Physics::Manager::updateEntityPhysics(Entity::Type entity, float dt) {
        auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
        if (!e_transform_comp) return;

        auto& e_transform = e_transform_comp.value().get();
        auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);

        if (e_dynamics_comp) {
            auto& e_dynamics = e_dynamics_comp.value().get();

            // Ensure mass is not negative
            e_dynamics.mass = e_dynamics.mass == 0.0f ? EPSILON : e_dynamics.mass;

            // Apply forces & mass to calculate acceleration
            Vector2f acceleration = e_dynamics.force / e_dynamics.mass;

            // Update velocity based on acceleration
            e_dynamics.velocity += acceleration * dt;

            // Add drag/friction
            e_dynamics.velocity -= e_dynamics.velocity * e_dynamics.drag * dt;

            // Cap Velocity
            if (e_dynamics.velocity.length() > e_dynamics.max_speed) {
                e_dynamics.velocity = e_dynamics.velocity.normalized() * e_dynamics.max_speed;
            }

            // Set velocity to zero if net velocity < EPSILON
            if (e_dynamics.velocity.length() < EPSILON) {
                e_dynamics.velocity = { 0.0f, 0.0f };
            }

            // Update Transform position based on velocity
            e_transform.position.x += e_dynamics.velocity.x * dt;
            e_transform.position.y += e_dynamics.velocity.y * dt;
        }

        // Update collider transform
        auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
        if (e_collider_comp) {
            auto& e_collider = e_collider_comp.value().get();
            if (e_collider.b_bind_to_entity) {
                e_collider.transform = e_transform;
            }
            else {
                e_collider.transform.position = e_transform.position + e_collider.pos_offset;
            }

            // Reset collision flag for this frame
            e_collider.b_collided = false;

            // Update occupied cells
            entity_occupied_cells[entity] = calculateOccupiedCells(e_collider.transform);
        }
    }

    std::vector<std::pair<Entity::Type, Entity::Type>> Physics::Manager::getPotentialCollisions() {
        std::vector<std::pair<Entity::Type, Entity::Type>> pairs;
        std::set<std::pair<Entity::Type, Entity::Type>> processed_pairs;

        // For each entity
        for (const auto& [entity, cells] : entity_occupied_cells) {
            // Count unique colliders in each cell this entity occupies
            std::unordered_map<Vector2i, std::vector<Entity::Type>, Vector2iHasher> entities_in_cell;

            // Collect all entities in each cell
            for (const auto& cell_pos : cells) {
                for (const auto& [other_entity, other_cells] : entity_occupied_cells) {
                    if (entity != other_entity &&
                        std::find(other_cells.begin(), other_cells.end(), cell_pos) != other_cells.end()) {
                        entities_in_cell[cell_pos].push_back(other_entity);
                    }
                }
            }

            // Log cell occupancy and generate pairs
            for (const auto& [cell_pos, cell_entities] : entities_in_cell) {
                // NIKEE_CORE_WARN("Broad-phase warning: Entity {} is in grid cell ({}, {}) containing {} collider entities.", entity, cell_pos.x, cell_pos.y, cell_entities.size());

                // Generate collision pairs
                for (Entity::Type other_entity : cell_entities) {
                    if (entity < other_entity) { // Avoid duplicate pairs
                        auto pair = std::make_pair(entity, other_entity);
                        if (processed_pairs.insert(pair).second) {
                            pairs.push_back(pair);
                        }
                    }
                }
            }
        }

        return pairs;
    }

    void Physics::Manager::processCollisions(
        const std::vector<std::pair<Entity::Type, Entity::Type>>& collision_pairs) {

        for (const auto& [entity_a, entity_b] : collision_pairs) {
            auto a_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_a);
            auto a_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_a);
            auto a_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_a);

            auto b_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_b);
            auto b_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_b);
            auto b_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_b);

            if (!a_collider_comp.has_value() || !a_transform_comp.has_value() ||
                !b_collider_comp.has_value() || !b_transform_comp.has_value()) {
                continue;
            }

            auto& a_transform = a_transform_comp.value().get();
            auto& a_collider = a_collider_comp.value().get();

            Physics::Dynamics default_dynamics_a;
            Physics::Dynamics& a_dynamics = a_dynamics_comp.has_value() ?
                a_dynamics_comp.value().get() : default_dynamics_a;

            auto& b_transform = b_transform_comp.value().get();
            auto& b_collider = b_collider_comp.value().get();

            Physics::Dynamics default_dynamics_b;
            Physics::Dynamics& b_dynamics = b_dynamics_comp.has_value() ?
                b_dynamics_comp.value().get() : default_dynamics_b;

            // Get model IDs for collision detection
            std::string a_model_id = "square.model"; // Default model
            std::string b_model_id = "square.model"; // Default model

            auto a_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity_a);
            if (a_shape_comp.has_value()) {
                a_model_id = a_shape_comp.value().get().model_id;
            }

            auto b_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity_b);
            if (b_shape_comp.has_value()) {
                b_model_id = b_shape_comp.value().get().model_id;
            }

            // Track collision checks
            collision_checks_count[entity_a]++;
            collision_checks_count[entity_b]++;

            // Collision info
            Collision::CollisionInfo info;

            // Perform AABB collision detection first
            bool collision_detected = false;
            if (!(static_cast<int>(a_collider.transform.rotation) % 90) &&
                !(static_cast<int>(b_collider.transform.rotation) % 90)) {
                collision_detected = collision_system->detectAABBRectRect(
                    a_dynamics, a_collider, b_dynamics, b_collider, info);
            }
            // Perform SAT collision detection if needed
            else {
                collision_detected = collision_system->detectAABBRectRect(
                    a_dynamics, a_collider, b_dynamics, b_collider, info) ||
                    collision_system->detectSATCollision(
                        a_collider, b_collider, a_model_id, b_model_id, info);
            }

            if (collision_detected) {
                // Set collision flags
                a_collider.b_collided = true;
                b_collider.b_collided = true;

                // Perform collision resolution
                collision_system->collisionResolution(
                    entity_a, a_transform, a_dynamics, a_collider,
                    entity_b, b_transform, b_dynamics, b_collider,
                    info
                );
            }
        }
    }

    void Physics::Manager::update() {
        // Clear processed collisions from previous frame
        collision_system->clearProcessedCollisions();

        // Clear entity cell mappings and collision check counts
        entity_occupied_cells.clear();
        collision_checks_count.clear();

        // Get delta time
        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

        // Get layers
        auto const& layers = NIKE_SCENES_SERVICE->getLayers();

        // Maximum physics steps
        constexpr int MAX_PHYSICS_STEPS = 5;
        int num_steps = Utility::getMin(
            NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(), MAX_PHYSICS_STEPS);

        // Iteration every fixed step for fixed delta time
        for (int step = 0; step < num_steps; ++step) {
            // Update physics and track occupied cells
            for (auto& layer : layers) {
                if (!layer->getLayerState()) continue;  // Skip inactive layers

                for (auto& entity : layer->getEntitites()) {
                    if (entities.find(entity) == entities.end()) continue;

                    // Update entity physics and its occupied cells
                    updateEntityPhysics(entity, dt);
                }
            }

            // Get potential collision pairs
            auto collision_pairs = getPotentialCollisions();

            // Process collisions
            processCollisions(collision_pairs);

            // Log collision check counts for this frame
            for (const auto& [entity, checks] : collision_checks_count) {
                NIKEE_CORE_WARN("Entity {} underwent {} collision checks this frame.", entity, checks);
            }
        }
    }

    void Physics::Manager::onEvent(std::shared_ptr<Physics::ChangePhysicsEvent> event) {
        collision_system->setRestitution(event->restitution);
        event->setEventProcessed(true);
    }
}