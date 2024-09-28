/*****************************************************************//**
 * \file   sysInput.h
 * \brief  input manager for engine
 * 
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef INPUT_HPP
#define INPUT_HPP

#include "../headers/Managers/mSystem.h"

namespace Input {

	struct Key {

	};

	struct Mouse {
		//Pos of button
		float x;
		float y;

		//L & R Button pressed
		bool is_lbutton_pressed;
		bool is_rbutton_pressed;

		//most mousewheels will only scroll on y axis
		float scroll_x;
		float scroll_y;
	};

	//Input Mode
	enum class KeyMode {
		CHECK = GLFW_PRESS,
		RELEASE = GLFW_RELEASE,
		TRIGGERED
	};

	class Manager : public System::ISystem {
	private:

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

		//Input variables
		Mouse mouse;

	public:

		//Default Constructor
		Manager() = default;

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
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
