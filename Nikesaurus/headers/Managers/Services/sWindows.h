/*****************************************************************//**
 * \file   mWindows.h
 * \brief  Window functions
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (80%)
 * \co-author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (20%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef WINDOWS_HPP
#define WINDOWS_HPP

#include "sEvents.h"

namespace NIKESAURUS {
	namespace Windows {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		/*****************************************************************//**
		* Window Abstract Classes
		*********************************************************************/
		//Window Resize Event
		struct WindowResized : Events::IEvent {
			Vector2i frame_buffer;

			WindowResized(int width, int height)
				:frame_buffer{ width, height } {}
		};

		//Abstract Window Class
		class NIKESAURUS_API IWindow : public Events::IEventListener<WindowResized> {
		private:
		public:
			//Defaults
			IWindow() = default;
			virtual ~IWindow() = default;

			//Set Window Mode
			virtual void setWindowMode(int mode, int value) = 0;

			//Query Window Mode
			virtual int queryWindowMode(int mode) = 0;

			//Toggle Full Screen Mode
			virtual void setFullScreen(int value) = 0;

			//Setup Event Callbacks
			virtual void setupEventCallbacks() = 0;

			//Set input modes
			virtual void setInputMode(int mode, int value) = 0;

			//Poll window events
			virtual void pollEvents() = 0;

			//Swap buffers
			virtual void swapBuffers() = 0;

			//Set window title
			virtual void setWindowTitle(const std::string& title) = 0;

			//Get window title
			virtual std::string getWindowTitle() const = 0;

			//Set window size
			virtual void setWindowSize(int width, int height) = 0;

			//Get window size
			virtual Vector2i getWindowSize() const = 0;

			//Get window pos
			virtual Vector2i getWindowPos() = 0;

			//Check window state
			virtual bool windowState() const = 0;

			//Terminate window
			virtual void terminate() = 0;

			//Clean up window resources
			virtual void cleanUp() = 0;

		private:
			//Window Event
			virtual void onEvent(std::shared_ptr<WindowResized> event) override = 0;
		};

		/*****************************************************************//**
		* DLL Build Implementation
		*********************************************************************/

		#ifdef NIKE_BUILD_DLL //Expose implementation only to NIKE Engine

		//Nike Engine Window
		class NIKEWindow : public IWindow {
		private:

			//Window Variables
			GLFWwindow* ptr_window;
			Vector2i window_pos;
			Vector2i window_size;
			std::string window_title;
			bool b_full_screen;

			//Configure Window
			void configWindow();
		public:

			NIKEWindow(Vector2i window_size, std::string window_title);

			NIKEWindow(std::string const& file_path);

			void setWindowMode(int mode, int value) override;

			int queryWindowMode(int mode) override;

			void setFullScreen(int value) override;

			void setupEventCallbacks() override;

			void setInputMode(int mode, int value) override;

			void pollEvents() override;

			void swapBuffers() override;

			void setWindowTitle(const std::string& title) override;

			std::string getWindowTitle() const override;

			void setWindowSize(int width, int height) override;

			Vector2i getWindowSize() const override;

			Vector2i getWindowPos() override;

			bool windowState() const override;

			void terminate() override;

			void cleanUp() override;

			~NIKEWindow() override;

			void onEvent(std::shared_ptr<WindowResized> event) override;
		};

		#endif //Expose implementation only to NIKE Engine

		/*****************************************************************//**
		* Window Service
		*********************************************************************/
		class NIKESAURUS_API Service {
		private:
			//Delete Copy Constructor & Copy Assignment
			Service(Service const& copy) = delete;
			void operator=(Service const& copy) = delete;

			//Engine Variables
			std::shared_ptr<IWindow> ptr_window;

			//Delta time variables
			float delta_time;
			double curr_time;

			//FPS Variables
			int target_fps;
			float actual_fps;

		public:
			//Default constructor
			Service() : ptr_window{ nullptr }, delta_time{ 0.0f }, target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f } {}

			//Arguement Constructor
			Service(std::shared_ptr<IWindow> window);

			//Set Window
			void setWindow(std::shared_ptr<IWindow> window);

			//Get Window
			std::shared_ptr<IWindow> getWindow();

			//Create Console
			void createConsole(std::string const& custom_welcome);

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

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!WINDOWS_HPP