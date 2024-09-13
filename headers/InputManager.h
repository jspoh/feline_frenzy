/*****************************************************************//**
 * \file   InputManager.h
 * \brief  input manager for engine
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"

class InputManager {
private:
	InputManager() = default;
	~InputManager() = default;

	class Mouse {
	public:
		float x{};
		float y{};

		bool is_lbutton_pressed{};
		bool is_rbutton_pressed{};

		// most mousewheels will only scroll on y axis
		float scroll_x{};
		float scroll_y{};
	};

public:
	Mouse mouse{};

	static InputManager& getInstance();

	static void fbsize_cb(GLFWwindow* window, int width, int height);
	static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mousebutton_cb(GLFWwindow* window, int button, int action, int mods);
	static void mousepos_cb(GLFWwindow* window, double xpos, double ypos);
	static void mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset);

	// for event handling usage
	bool key_is_pressed(int key) const;
};
