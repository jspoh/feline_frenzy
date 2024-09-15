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
#include "System.h"

namespace Core {

	class Engine {
	private:
		
		//Engine Variables
		GLFWwindow* ptr_window;
		int window_width;
		int window_height;
		std::string window_title;
		float delta_time;

		//Pointer to engine systems
		std::vector<std::shared_ptr<System::Base>> systems;

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

		/**
		 * Add new system into core engine
		 *
		 * \param system	shared pointer to new system
		 * \param index		index to insert system, if not specified or out of range
		 *					system will be inserted at the back
		 */
		void addSystem(std::shared_ptr<System::Base> system, size_t index = std::string::npos);
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()
}

#endif // !ENGINE_HPP