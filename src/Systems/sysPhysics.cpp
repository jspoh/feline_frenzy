/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author b.soh
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysPhysics.h"

// Forward declaration for Collision
//namespace Collision {
//    class Manager;
//}


void Physics::Manager::init() {

}

void Physics::Manager::update() {
    float dt = NIKEEngine.getDeltaTime();

    for (const auto& entity : entities) {
        // Check if entity contains Transform component and Velocity component
        if (NIKEEngine.checkEntityComponent<Transform::Transform>(entity) && NIKEEngine.checkEntityComponent<Transform::Velocity>(entity)) {

            // Reference to transform component
            Transform::Transform& transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

            // Ref to velocity component
            Transform::Velocity& velocity = NIKEEngine.getEntityComponent<Transform::Velocity>(entity);

            const float speed = 100.0f;

            // Check if entity contains Move component
            
            if (NIKEEngine.checkEntityComponent<Move::Movement>(entity)) {
                // Ref to Move component
                Move::Movement& move = NIKEEngine.getEntityComponent<Move::Movement>(entity);

                const float movespeed = 300.0f;

                // Reset Velocity
                velocity.velocity.y = 0.0f;
                velocity.velocity.x = 0.0f;

                if (move.Up) {
                    velocity.velocity.y += movespeed;
                }

                if (move.Down) {
                    velocity.velocity.y -= movespeed;
                }

                if (move.Left) {
                    velocity.velocity.x -= movespeed;
                }

                if (move.Right) {
                    velocity.velocity.x += movespeed;
                }
            }
            
            // Normalize Movement
            if (velocity.velocity.lengthSq() > 0.0f) {
                velocity.velocity = velocity.velocity.normalize();
            }

            // Apply velocity
            transform.position += velocity.velocity * speed * dt;
            //transform.position.x += velocity.velocity.x * dt;
            //transform.position.y += velocity.velocity.y * dt;
                
        }
    }
    

    //NIKEEngine.accessSystem<Entity::Manager>()->getEntitiesWithComponents();

    // Move Camera
    //NIKEEngine.accessSystem<Physics::Manager>()->getInstance()->move(NIKEEngine.accessSystem<Render::Manager>()->getObject("camera"));
}

/*
void Physics::Manager::move(Object* object) {
    const float speed = 1000.0f;
    const float dt = NIKEEngine.getDeltaTime();

    if (!object) {
        cout << "Object does not exist" << endl;
        return;
    }

    Vector2 moveVec{};
    Vector2 objPos = object->getPosition();

    if (NIKEEngine.accessSystem<Input::Manager>()->getInstance()->key_is_pressed(GLFW_KEY_D)){
        moveVec.x += 1.0f;
    }
    if (NIKEEngine.accessSystem<Input::Manager>()->getInstance()->key_is_pressed(GLFW_KEY_A)) {
        moveVec.x -= 1.0f;
    }
    if (NIKEEngine.accessSystem<Input::Manager>()->getInstance()->key_is_pressed(GLFW_KEY_W)) {
        moveVec.y += 1.0f;
    }
    if (NIKEEngine.accessSystem<Input::Manager>()->getInstance()->key_is_pressed(GLFW_KEY_S)) {
        moveVec.y -= 1.0f;
    }

    // Normalizing movement
    if (moveVec.lengthSq() > 0.0f) {
        moveVec = moveVec.normalized();
    }

    // Applying movement
    objPos += moveVec * speed * dt;
    object->setPosition(objPos.x, objPos.y);
}
*/