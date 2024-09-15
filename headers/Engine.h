/*****************************************************************//**
 * \file   Engine.h
 * \brief  
 * 
 * \author jings
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "stdafx.h"

namespace Core {

	class Engine {
	private:
		//Window
		GLFWwindow* ptr_window;

		//Window Width
		int window_width;

		//Window Height
		int window_height;

		//Window Title
		std::string window_title;

		//Delta Time
		float delta_time;

		//Default Constructor For Engine
		Engine();

		//Delete Copy Constructor & Copy Assignment
		Engine(Engine const& copy) = delete;
		void operator=(Engine const& copy) = delete;

		//Destructor
		~Engine();

		/**
		 * Read & Deserialize Data From Config File
		 * Basic Data: Window Dimensions & Title
		 */
		void readConfigFile(std::string const& file_path);

		/**
		 * sets up event callback functions using InputManager class.
		 *
		 */
		void setupEventCallbacks();

		//Calculate Delta Time
		void calculateDeltaTime();

		//Get Game Running
		bool getGameRunning() const;

	public:

		/**
		 * instance manager for singleton class.
		 *
		 * \return instance
		 */
		static Engine& getInstance() {
			static Core::Engine instance;
			return instance;
		}

		//Init Window with config file
		void init(std::string const& file_path);

		//Run Game Loop
		void run();

		//Terminate Game Loop
		void terminate();

		//Get Window
		GLFWwindow* getWindow() const;

		//Set Window Title
		void setTitle(std::string const& title);

		//Set Window Width
		void setWindowWidth(int width);

		//Get Window Width
		int getWindowWidth() const;

		//Set Window Height
		void setWindowHeight(int height);

		//Get Window Height
		int getWindowHeight() const;

		//Get Delta Time
		float getDeltaTime() const;
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()
}

#endif // !ENGINE_HPP