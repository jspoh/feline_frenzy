/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysPhysics.h"

void Physics::Manager::init() {

}

void Physics::Manager::update() {
    //Physics::Manager::getInstance().move(RenderManager::getInstance().getObject("camera"));
    //NIKEEngine.accessSystem<Physics::Manager>()->getInstance()->move(Render::Manager::getInstance().getObject("camera"));
    NIKEEngine.accessSystem<Physics::Manager>()->getInstance()->move(NIKEEngine.accessSystem<Render::Manager>()->getObject("camera"));
}

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

    // Calculate magnitude of movement
    //float magnitude = sqrt((moveVec.x * moveVec.x) + (moveVec.y * moveVec.y));

    // Normalizing movement
    if (moveVec.lengthSq() > 0.0f) {
        moveVec = moveVec.normalized();
    }

    // Applying movement
    objPos += moveVec * speed * dt;
    object->setPosition(objPos.x, objPos.y);
}