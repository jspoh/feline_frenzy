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
#include "../headers/Components/cInput.h"
#include "../headers/Managers/mEvents.h"

namespace Input {

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

		//Mouse input
		Mouse mouse;

		//Boolean for mouse inputs
		bool b_mouse_triggered;
		bool b_mouse_released;

		//Input mode checkss
		bool mouseTriggerCheck();
		bool mouseReleaseCheck();

		//Key input
		Key key;

		//Boolean for mouse inputs
		bool b_key_triggered;
		bool b_key_released;

		//Input mode checkss
		bool keyTriggerCheck();
		bool keyReleaseCheck();

	public:

		//Default Constructor
		Manager() : mouse(), b_mouse_triggered{false}, b_mouse_released{false},
			key(), b_key_triggered{false}, b_key_released{false} {}

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
	};
}

#endif //!INPUT_HPP
