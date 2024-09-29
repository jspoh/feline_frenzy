/*****************************************************************//**
 * \file   sysInput.cpp
 * \brief  input manager for engine
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysInput.h"
#include "../headers/Core/Engine.h"

void Input::Manager::setupEventCallbacks() {
	glfwSetFramebufferSizeCallback(NIKEEngine.getWindow(), fbsize_cb);
	glfwSetKeyCallback(NIKEEngine.getWindow(), key_cb);
	glfwSetMouseButtonCallback(NIKEEngine.getWindow(), mousebutton_cb);
	glfwSetCursorPosCallback(NIKEEngine.getWindow(), mousepos_cb);
	glfwSetScrollCallback(NIKEEngine.getWindow(), mousescroll_cb);
}

void Input::Manager::init() {
	//Setup event callbacks
	setupEventCallbacks();

	//Setup input mode
	glfwSetInputMode(NIKEEngine.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::Manager::update() {

	//Update all entities
	for (auto& entity : entities) {
		if (NIKEEngine.checkEntityComponent<Input::Mouse>(entity)) {
			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);
			e_mouse = mouse;
		}
	}

	//Temp changing scene here
	if (mouse.is_lbutton_pressed) {
		NIKEEngine.changeScene("MENU");
	}

	//Check if escape key is pressed
	if (key_is_pressed(GLFW_KEY_ESCAPE)) {
		NIKEEngine.terminate();
	}
}

void Input::Manager::fbsize_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int width, [[maybe_unused]] int height) {

}

void Input::Manager::key_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {

}

void Input::Manager::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			getInstance()->mouse.is_lbutton_pressed = true;
		}
		else if (action == GLFW_RELEASE) {
			getInstance()->mouse.is_lbutton_pressed = false;
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			getInstance()->mouse.is_rbutton_pressed = true;
		}
		else if (action == GLFW_RELEASE) {
			getInstance()->mouse.is_rbutton_pressed = false;
		}
		break;
	}
}

void Input::Manager::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
	getInstance()->mouse.x = static_cast<float>(xpos);
	getInstance()->mouse.y = static_cast<float>(ypos);
}

void Input::Manager::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
	getInstance()->mouse.scroll_x = static_cast<float>(xoffset);
	getInstance()->mouse.scroll_y = static_cast<float>(yoffset);
}

bool Input::Manager::key_is_pressed(int key) const {
	return glfwGetKey(NIKEEngine.getWindow(), key) == GLFW_PRESS;
}
