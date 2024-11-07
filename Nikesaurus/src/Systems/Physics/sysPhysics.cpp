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


namespace NIKE {
    void Physics::Manager::init() {
        collision_system = std::make_unique<Collision::System>();

        std::shared_ptr<Physics::Manager> physics_sys_wrapped(this, [](Physics::Manager*) {});
        NIKE_EVENTS_SERVICE->addEventListeners<Physics::ChangePhysicsEvent>(physics_sys_wrapped);
    }

    void Physics::Manager::update() {
        float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

        //Iterate through layers
        for (auto& layer : NIKE_SCENES_SERVICE->getCurrScene()->getLayers()) {
            //SKip inactive layer
            if (!layer->getLayerState())
                continue;

            //Iterate through all entities
            for (auto& entity : entities) {

                //Skip entities that are not present within layer & entities without transform component
                if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity) || !NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity))
                    continue;

                //Update entities with dynamics
                if (NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity) && 
                    NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
                    auto& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                    auto& e_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);

                    //// Retrieve the logic state to determine behavior
                    //if (NIKE_ECS_MANAGER->checkEntityComponent<Logic::State>(entity)) {
                    //    auto& e_state = NIKE_ECS_MANAGER->getEntityComponent<Logic::State>(entity);

                    //    switch (e_state.current_state) {
                    //    case Logic::EntityStateType::IDLE:
                    //        // In IDLE, entity is stationary
                    //        e_dynamics.velocity = Vector2f(0.0f, 0.0f);
                    //        break;

                    //    case Logic::EntityStateType::PATROLLING:
                    //        // In PATROLLING, entity moves to random points in patrol radius
                    //        if (e_dynamics.velocity.length() < 0.01f) {
                    //            // Check if entity has reached patrol target, if so wait 2 seconds
                    //            e_dynamics.velocity = getRandomDirection() * e_dynamics.max_speed;
                    //        }
                    //        e_transform.position += e_dynamics.velocity * dt;
                    //        break;

                    //    case Logic::EntityStateType::CHASING: {
                    //        // In CHASING, entity moves toward the player
                    //        auto player_entity = GameLogic::Manager::getPlayerEntity();
                    //        if (player_entity && NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(player_entity)) {
                    //            auto& player_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(player_entity);
                    //            Vector2f chase_direction = (player_transform.position - e_transform.position).normalize();
                    //            e_dynamics.velocity = chase_direction * e_dynamics.max_speed;
                    //            e_transform.position += e_dynamics.velocity * dt;
                    //        }
                    //        break;
                    //    }

                    //    default:
                    //        break;
                    //    }
                    //}


                    //Ensure that mass is not negative
                    e_dynamics.mass = e_dynamics.mass == 0.0f ? EPSILON : e_dynamics.mass;

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
                    if (e_dynamics.velocity.length() < 0.0001f) {
                        e_dynamics.velocity = { 0.0f, 0.0f };
                    }

                    //Update position based on velocity
                    e_transform.position.x += e_dynamics.velocity.x * dt;
                    e_transform.position.y += e_dynamics.velocity.y * dt;
                }

                //Collision detection
                if (NIKE_ECS_MANAGER->checkEntityComponent<Physics::Collider>(entity) &&
                    NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity) &&
                    NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity)
                    ) {

                    //Get entity collider
                    auto& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
                    auto& e_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
                    auto& e_collider = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);

                    //Check for collision with other entities
                    for (auto& colliding_entity : entities) {

                        //Skip entities colliding entites that are not in the layer mask
                        if (!layer->getLayerMask().test(NIKE_ECS_MANAGER->getEntityLayerID(colliding_entity)) ||
                            entity == colliding_entity ||
                            !NIKE_ECS_MANAGER->checkEntityComponent<Physics::Collider>(colliding_entity))
                            continue;

                        //Get colliding entity's data
                        auto& other_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(colliding_entity);
                        auto& other_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(colliding_entity);
                        auto& other_collider = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(colliding_entity);
                       
                        // Temporary code to get model_id for SAT collision, current SAT uses model_id to determine vertices.
                        std::string e_model_id;
                        std::string other_model_id;
                        if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Shape>(entity)) {
                            e_model_id = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity).model_id;
                        }
                        else {
                            e_model_id = "square"; // Default to square for SAT
                        }
                        if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Shape>(colliding_entity)) {
                            other_model_id = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(colliding_entity).model_id;
                        }
                        else {
                            other_model_id = "square"; // Default to square for SAT
                        }

                        Collision::CollisionInfo info;
                        if (!(static_cast<int>(e_transform.rotation) % 180) && !(static_cast<int>(other_transform.rotation) % 180) && collision_system->detectAABBRectRect(e_transform, e_dynamics, other_transform, other_dynamics, info)) {
                            //Set the flag of colliders
                            e_collider.b_collided = true;
                            other_collider.b_collided = true;

                            //Collision resolution
                            collision_system->collisionResolution(e_transform, e_dynamics, e_collider, other_transform, other_dynamics, other_collider, info);
                        }
                        else if (collision_system->detectSATCollision(e_transform, other_transform, e_model_id, other_model_id, info)) {
                            //Set the flag of colliders
                            e_collider.b_collided = true;
                            other_collider.b_collided = true;

                            //Collision resolution
                            collision_system->collisionResolution(e_transform, e_dynamics, e_collider, other_transform, other_dynamics, other_collider, info);
                        }
                        else {
                            //Set the flag of colliders
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

    void Physics::Manager::applyForce(Entity::Type entity, Vector2f const& force) {
        cout << "HERE" << endl;

        if (!NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity))
            return;

        //Get dynamics
        auto& e_dynamics = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);
        e_dynamics.force = force;
    }

    void Physics::Manager::registerLuaBindings(sol::state& lua_state) {
        lua_state.set_function("applyForce", [this](unsigned int entity, float x_force, float y_force) {
            applyForce(static_cast<Entity::Type>(entity), Vector2f(x_force, y_force));
            });
    }
}
