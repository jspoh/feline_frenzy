/*****************************************************************//**
 * \file   InputManager.h
 * \brief  input manager for engine
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef INPUT_HPP
#define INPUT_HPP

#include "stdafx.h"
#include "Engine.h"

namespace Input {

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

	class Manager : public System::Base {
	private:

		//Private Constructor
		Manager() = default;

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		/**
		 * sets up event callback functions using InputManager class.
		 *
		 */
		void setupEventCallbacks();

		//Input Keys
		static void fbsize_cb(GLFWwindow* window, int width, int height);
		static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mousebutton_cb(GLFWwindow* window, int button, int action, int mods);
		static void mousepos_cb(GLFWwindow* window, double xpos, double ypos);
		static void mousescroll_cb(GLFWwindow* pwin, double xoffset, double yoffset);

	public:
		Mouse mouse{};

		//Singleton Of Manager Class
		static Manager& getInstance() {
			static Manager instance;
			return instance;
		}

		//Init Inputs
		void init() override;

		//Update Inputs
		void update() override;

		//Default Destructor
		~Manager() override = default;

		// for event handling usage
		bool key_is_pressed(int key) const;
	};
}

#endif //!INPUT_HPP