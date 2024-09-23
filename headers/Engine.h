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

#include "Entity.h"
#include "Component.h"
#include "System.h"

namespace Core {

	class Engine {
	private:
		
		//Engine Variables
		GLFWwindow* ptr_window;
		Vector2 window_size;
		std::string window_title;

		//Delta time variables
		float delta_time;
		double curr_time;

		//FPS Variables
		int target_fps;
		float actual_fps;

		//Pointer to engine systems
		std::vector<std::shared_ptr<System::ISystem>> systems;
		std::unordered_map<std::string, std::shared_ptr<System::ISystem>> systems_map;

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

		//Configure systems ( GLFW & GLEW )
		void configSystem();

		//Calculate Delta Time
		void calculateDeltaTime();

		//FPS control
		void controlFPS();

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
		void init(std::string const& file_path, int fps = 60);

		//Run Game Loop
		void run();

		//Terminate Game Loop
		void terminate();

		//Get Window
		GLFWwindow* getWindow() const;

		//Set Window Title
		void setWinTitle(std::string const& title);

		//Set Window Size
		void setWindowSize(float width, float height);

		//Get Window Size
		Vector2 const& getWindowSize() const;

		//Set Target FPS
		void setTargetFPS(int fps);

		//Get Current FPS
		float getCurrentFPS() const;

		//Get Delta Time
		float getDeltaTime() const;

		/**
		 * Add new system into core engine
		 *
		 * \param system			shared pointer to new system
		 * \param sys_identifier	system identifier
		 * \param index				index to insert system, if not specified or out of range
		 *							system will be inserted at the back
		 */
		void addSystem(std::shared_ptr<System::ISystem> system, std::string const& sys_identifier, size_t index = std::string::npos);

		/**
		 * Access system already within core engine
		 *
		 * \param sys_identifier	system identifier
		 *
		 */
		std::shared_ptr<System::ISystem> accessSystem(std::string const& sys_identifier);
	};

	//Predefined name for core engine
	#define NIKEEngine Core::Engine::getInstance()
}

#endif // !ENGINE_HPP