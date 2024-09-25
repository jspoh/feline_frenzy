/*****************************************************************//**
 * \file   PhysicsSystem.cpp
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "RenderManager.h"

void Physics::Manager::init() {

}

void Physics::Manager::update() {
    Physics::Manager::getInstance().move(RenderManager::getInstance().getObject("camera"));
}

void Physics::Manager::move(Object* object) {
    if (!object) {
        return;
    }

    Vector2 objPos = object->getPosition();

    if (Input::Manager::getInstance().key_is_pressed(GLFW_KEY_D)) {
        objPos.x += 0.1f;
    }
    if (Input::Manager::getInstance().key_is_pressed(GLFW_KEY_A)) {
        objPos.x -= 0.1f;
    }
    if (Input::Manager::getInstance().key_is_pressed(GLFW_KEY_W)) {
        objPos.y += 0.1f;
    }
    if (Input::Manager::getInstance().key_is_pressed(GLFW_KEY_S)) {
        objPos.y -= 0.1f;
    }

    object->setPosition(objPos.x, objPos.y);
}