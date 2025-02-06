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
#include "Components/cPhysics.h"
#include "Components/cTransform.h"


namespace NIKE {
    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    void Physics::Manager::update() {

        // Clear processed collisions
        collision_system->clearProcessedCollisions();

        //Delta time
        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

        //Get layers
        auto const& layers = NIKE_SCENES_SERVICE->getLayers();

        //Hashing function
        auto hash = [](int x, int y) -> int {
            return static_cast<int>(std::hash<int>()(x) ^ (std::hash<int>()(y) << 1));
            };

        //Maximum physics steps
        constexpr int MAX_PHYSICS_STEPS = 5;

        //Iteration every fixed step for fixed delta time
        for (int step = 0; step < min(NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(), MAX_PHYSICS_STEPS); ++step) {

            //Broad-phase collision detection
            std::unordered_map<int, std::vector<Entity::Type>> spatial_grid;
            const Vector2f grid_scale = NIKE_MAP_SERVICE->getGridScale();

            //Default dynamics place holder
            Physics::Dynamics def_dynamics;

            //Iterate through layers
            for (auto& layer : layers) {

                //Skip inactive layer
                if (!layer->getLayerState())
                    continue;

                //Iterate through all entities
                for (auto& entity : layer->getEntitites()) {

                    //Skip entity not registered to this system
                    if (entities.find(entity) == entities.end()) continue;

                    //Skip entities with no transform
                    auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                    if (!e_transform_comp) continue;
                    auto& e_transform = e_transform_comp.value().get();

                    //Update entities with dynamics
                    auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                    if (e_dynamics_comp) {

                        auto& e_dynamics = e_dynamics_comp.value().get();

                        //Ensure that mass is not negative
                        e_dynamics.mass = e_dynamics.mass == 0.0f ? EPSILON : e_dynamics.mass;

                        //!!!Multiple forces to be implemented soon

                        ////Calculate netforce
                        //Vector2f net_force;

                        ////iterate through forces to get net force and remove inactive forces
                        //for (auto it = e_dynamics.forces.begin(); it != e_dynamics.forces.end();) {
                        //    it->life_time -= dt;
                        //    if (it->life_time > 0.0f) {
                        //        net_force += it->direction;
                        //        it++;
                        //    }
                        //    else {
                        //        it = e_dynamics.forces.erase(it);
                        //    }
                        //}

                        //Apply forces & mass to calculate direction
                        Vector2f acceleration = e_dynamics.force / e_dynamics.mass;

                        //Update velocity based on acceleration
                        e_dynamics.velocity += acceleration * dt;

                        //Add drag/friction
                        e_dynamics.velocity -= e_dynamics.velocity * e_dynamics.drag * dt;

                        //Cap Velocity
                        if (e_dynamics.velocity.length() > e_dynamics.max_speed) {
                            e_dynamics.velocity = e_dynamics.velocity.normalize() * e_dynamics.max_speed;
                        }

                        //Set velocity to zero if net velo < 0.01
                        if (e_dynamics.velocity.length() < EPSILON) {
                            e_dynamics.velocity = { 0.0f, 0.0f };
                        }

                        //Update Transform position based on velocity
                        e_transform.position.x += e_dynamics.velocity.x * dt;
                        e_transform.position.y += e_dynamics.velocity.y * dt;
                    }

                    // Collision detection
                    auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                    if (e_collider_comp.has_value()) {
                        auto& e_collider = e_collider_comp.value().get();

                        //Update collision transform
                        if (e_collider.b_bind_to_entity) {
                            e_collider.transform = e_transform;
                        }
                        else {
                            e_collider.transform.position = e_transform.position + e_collider.pos_offset;
                        }

                        //Hashing cell ID
                        int cell_x = static_cast<int>(e_collider.transform.position.x / grid_scale.x);
                        int cell_y = static_cast<int>(e_collider.transform.position.y / grid_scale.y);
                        int cell_id = hash(cell_x, cell_y);

                        //Insert into spatial grid for collision check
                        spatial_grid[cell_id].push_back(entity);
                    }
                }
            }

            //Narrow-phase collision detection
            for (auto& [cell_id, cell_entities] : spatial_grid) {
                for (size_t i = 0; i < cell_entities.size(); ++i) {
                    auto entity_a = cell_entities[i];
                    auto a_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_a);
                    auto a_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_a);
                    auto a_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_a);

                    if (!a_collider_comp.has_value() || !a_transform_comp.has_value()) continue;

                    //Get entity's layer
                    auto& e_layer = layers.at(NIKE_METADATA_SERVICE->getEntityLayerID(entity_a));

                    //Skip if layer is inactive
                    if (!e_layer->getLayerState()) continue;

                    //Check for collision with entities within cell range
                    for (size_t j = i + 1; j < cell_entities.size(); ++j) {
                        auto entity_b = cell_entities[j];
                        auto b_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity_b);
                        auto b_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity_b);
                        auto b_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity_b);

                        if (!b_collider_comp.has_value() || !b_transform_comp.has_value()) continue;

                        //Get all components
                        auto& a_transform = a_transform_comp.value().get();
                        auto& a_collider = a_collider_comp.value().get();
                        auto& a_dynamics = a_dynamics_comp.has_value() ? a_dynamics_comp.value().get() : def_dynamics;
                        auto& b_transform = b_transform_comp.value().get();
                        auto& b_collider = b_collider_comp.value().get();
                        auto& b_dynamics = b_dynamics_comp.has_value() ? b_dynamics_comp.value().get() : def_dynamics;

                        //Skip collision between entities from unmasked layer
                        if (!e_layer->getLayerMask().test(NIKE_METADATA_SERVICE->getEntityLayerID(entity_b))) continue;

                        //Skip collision check if objects are definitely not colliding
                        if ((a_collider.transform.position - b_collider.transform.position).lengthSq() > (a_collider.transform.scale + b_collider.transform.scale).lengthSq()) continue;

                        // Temporary code to get model_id for SAT collision, current SAT uses model_id to determine vertices.
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

                        //Collision info
                        Collision::CollisionInfo info;

                        // Perform AABB collision detection first
                        if (!(static_cast<int>(a_collider.transform.rotation) % 90) &&
                            !(static_cast<int>(b_collider.transform.rotation) % 90)) {

                            //If AABB collision
                            if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info)) {

                                // Set the collision flags
                                a_collider.b_collided = true;
                                b_collider.b_collided = true;

                                // Perform collision resolution
                                collision_system->collisionResolution(
                                    entity_a, a_transform, a_dynamics, a_collider,
                                    entity_b, b_transform, b_dynamics, b_collider,
                                    info
                                );

                                continue;
                            }
                        }
                        // Perform SAT collision detection if AABB fails
                        else if (collision_system->detectAABBRectRect(a_dynamics, a_collider, b_dynamics, b_collider, info) || collision_system->detectSATCollision(a_collider, b_collider, a_model_id, b_model_id, info)) {

                            // Set the collision flags
                            a_collider.b_collided = true;
                            b_collider.b_collided = true;

                            // Perform collision resolution
                            collision_system->collisionResolution(
                                entity_a, a_transform, a_dynamics, a_collider,
                                entity_b, b_transform, b_dynamics, b_collider,
                                info
                            );

                            continue;
                        }

                        // Reset collision flags if no collision
                        a_collider.b_collided = false;
                        b_collider.b_collided = false;
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
