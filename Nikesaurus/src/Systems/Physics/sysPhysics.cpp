/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (45%)
 * \co-author Min Khant Ko, 2301320, ko.m@digipen.edu (45%)
 *\ co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (10%)
 * 
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Physics/sysPhysics.h"
#include "Components/cPhysics.h"
#include "Components/cTransform.h"
#include "Components/cPathfinding.h"


namespace NIKE {
    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    void Physics::Manager::update() {

        //Delta time
        float dt = NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

        //Iteration every fixed step for fixed delta time
        for (int step = 0; step < NIKE_WINDOWS_SERVICE->getCurrentNumOfSteps(); ++step) {

            //Iterate through layers
            for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
                //SKip inactive layer
                if (!layer->getLayerState())
                    continue;

                //Iterate through all entities
                for (auto& entity : entities) {

                    //Skip entities that are not present within layer & entities without transform component
                    if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
                        continue;

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

                        //Calculate netforce
                        Vector2f net_force;

                        //iterate through forces to get net force and remove inactive forces
                        for (auto it = e_dynamics.forces.begin(); it != e_dynamics.forces.end();) {
                            it->life_time -= dt;
                            if (it->life_time > 0.0f) {
                                net_force += it->direction;
                                it++;
                            }
                            else {
                                it = e_dynamics.forces.erase(it);
                            }
                        }

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
                    Physics::Dynamics def_dynamics;
                    auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);
                    if (e_collider_comp.has_value()) {
                        auto& e_collider = e_collider_comp.value().get();
                        auto& e_dynamics = e_dynamics_comp.has_value() ? e_dynamics_comp.value().get() : def_dynamics;

                        //Update collision transform
                        if (e_collider.b_bind_to_entity) {
                            e_collider.transform = e_transform;
                        }
                        else {
                            e_collider.transform.position = e_transform.position + e_collider.pos_offset;
                        }

                        // Check for collisions with other entities
                        for (auto& colliding_entity : entities) {

                            // Skip colliding entities that are not in the layer mask or are the same entity
                            if (!layer->getLayerMask().test(NIKE_ECS_MANAGER->getEntityLayerID(colliding_entity)) ||
                                entity == colliding_entity)
                                continue;

                            auto other_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(colliding_entity);
                            auto other_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(colliding_entity);
                            auto other_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(colliding_entity);

                            if (!other_collider_comp.has_value() || !other_transform_comp.has_value()) continue;

                            auto& other_transform = other_transform_comp.value().get();
                            auto& other_collider = other_collider_comp.value().get();
                            auto& other_dynamics = other_dynamics_comp.has_value() ? other_dynamics_comp.value().get() : def_dynamics;

                            //Update collision transform
                            if (other_collider.b_bind_to_entity) {
                                other_collider.transform = other_transform;
                            }
                            else {
                                other_collider.transform.position = other_transform.position + other_collider.pos_offset;
                            }

                            // Temporary code to get model_id for SAT collision, current SAT uses model_id to determine vertices.
                            std::string e_model_id = "square.model"; // Default model
                            std::string other_model_id = "square.model"; // Default model
                            auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);
                            if (e_shape_comp.has_value()) {
                                e_model_id = e_shape_comp.value().get().model_id;
                            }
                            auto other_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(colliding_entity);
                            if (other_shape_comp.has_value()) {
                                other_model_id = other_shape_comp.value().get().model_id;
                            }

                            Collision::CollisionInfo info;

                            // Perform AABB collision detection first
                            if (!(static_cast<int>(e_collider.transform.rotation) % 180) &&
                                !(static_cast<int>(other_collider.transform.rotation) % 180)) {

                                //If AABB collision
                                if (collision_system->detectAABBRectRect(e_dynamics, e_collider, other_dynamics, other_collider, info)) {
                                    // Set the collision flags
                                    e_collider.b_collided = true;
                                    other_collider.b_collided = true;

                                    // Perform collision resolution
                                    collision_system->collisionResolution(
                                        e_transform, e_dynamics, e_collider,
                                        other_transform, other_dynamics, other_collider,
                                        info
                                    );

                                    return;
                                }
                            }
                            // Perform SAT collision detection if AABB fails
                            else if (collision_system->detectSATCollision(e_collider, other_collider, e_model_id, other_model_id, info)) {

                                // Set the collision flags
                                e_collider.b_collided = true;
                                other_collider.b_collided = true;

                                // Perform collision resolution
                                collision_system->collisionResolution(
                                    e_transform, e_dynamics, e_collider,
                                    other_transform, other_dynamics, other_collider,
                                    info
                                );

                                return;
                            }

                            // Reset collision flags if no collision
                            e_collider.b_collided = false;
                            other_collider.b_collided = false;
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

    //void Physics::Manager::applyXForce(Entity::Type entity, float force) {
    //    if (!NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity))
    //        return;

    //    //Get dynamics
    //    auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
    //    if (!e_dynamics_comp.has_value()) return;
    //    e_dynamics_comp.value().get().force.x = force;
    //}

    //void Physics::Manager::applyYForce(Entity::Type entity, float force) {
    //    if (!NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity))
    //        return;

    //    //Get dynamics
    //    auto e_dynamics_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
    //    if (!e_dynamics_comp.has_value()) return;
    //    e_dynamics_comp.value().get().force.y = force;
    //}

    //void Physics::Manager::registerLuaBindings(sol::state& lua_state) {
    //    lua_state.set_function("applyXForce", [this](unsigned int entity, float x_force) {
    //        applyXForce(static_cast<Entity::Type>(entity), x_force);
    //        });
    //    lua_state.set_function("applyYForce", [this](unsigned int entity, float y_force) {
    //        applyYForce(static_cast<Entity::Type>(entity), y_force);
    //        });
    //}
}
