/*****************************************************************//**
 * \file   mWindows.h
 * \brief  Window functions
 *
 * \author Ho
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef WINDOWS_HPP
#define WINDOWS_HPP

namespace Windows {
	class Manager {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

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

	public:
		//Default constructor
		Manager() : ptr_window{ nullptr }, window_size{}, window_title{ "" }, delta_time{ 0.0f },
			target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f } {}

		/**
		 * Read & Deserialize Data From Config File
		 * Basic Data: Window Dimensions & Title
		 */
		void readConfigFile(std::string const& file_path);

		//Configure systems ( GLFW & GLEW )
		void configSystem();

		//Terminate Game Loop
		void terminate();

		//Get Window
		GLFWwindow* getWindow() const;

		//Set Window Title
		void setWinTitle(std::string const& title);

		//Get Window Title
		std::string const& getWinTitle();

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

		//Calculate Delta Time
		void calculateDeltaTime();

		//FPS control
		void controlFPS();
	};
}

#endif //!WINDOWS_HPP