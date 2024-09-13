/*****************************************************************//**
 * \file   InputManager.h
 * \brief  input manager for engine
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/


#include "stdafx.h"
#include "InputManager.h"
#include "GLManager.h"

InputManager& InputManager::getInstance() {
	static InputManager instance;
	return instance;
}

void InputManager::fbsize_cb([[maybe_unused]] GLFWwindow* window, int width, int height) {
	GLManager::getInstance().window_width = width;
	GLManager::getInstance().window_height = height;
}

void InputManager::key_cb([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
	
}

void InputManager::mousebutton_cb([[maybe_unused]] GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			getInstance().mouse.is_lbutton_pressed = true;
		}
		else if (action == GLFW_RELEASE) {
			getInstance().mouse.is_lbutton_pressed = false;
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			getInstance().mouse.is_rbutton_pressed = true;
		}
		else if (action == GLFW_RELEASE) {
			getInstance().mouse.is_rbutton_pressed = false;
		}
		break;
	}
}

void InputManager::mousepos_cb([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) {
	getInstance().mouse.x = static_cast<float>(xpos);
	getInstance().mouse.y = static_cast<float>(ypos);
}

void InputManager::mousescroll_cb([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
	getInstance().mouse.scroll_x = static_cast<float>(xoffset);
	getInstance().mouse.scroll_y = static_cast<float>(yoffset);
}

bool InputManager::key_is_pressed(int key) const {
	return glfwGetKey(GLManager::getInstance().ptr_window, key) == GLFW_PRESS;
}
