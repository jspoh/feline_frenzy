/*****************************************************************//**
 * \file   mWindows.h
 * \brief  Window functions
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef WINDOWS_HPP
#define WINDOWS_HPP

namespace NIKESAURUS {
	namespace Windows {
		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Abstract Window Class
		class NIKESAURUS_API IWindow {
		private:
		public:
			//Defaults
			IWindow() = default;
			virtual ~IWindow() = default;

			//Configure Window Setup
			virtual void configWindow() = 0;

			//Poll window events
			virtual void pollEvents() = 0;

			//Swap buffers
			virtual void swapBuffers() = 0;

			//Set window title
			virtual void setWindowTitle(const std::string& title) = 0;

			//Get window title
			virtual std::string getWindowTitle() const = 0;

			//Set window size
			virtual void setWindowSize(float width, float height) = 0;

			//Get window size
			virtual Vector2 getWindowSize() const = 0;

			//Check window state
			virtual bool windowState() const = 0;

			//Terminate window
			virtual void terminate() = 0;

			//Clean up window resources
			virtual void cleanUp() = 0;
		};

		#ifdef NIKE_BUILD_DLL //Expose implementation only to NIKE Engine

		//Nike Engine Window
		class NIKEWindow : public IWindow {
		private:
			GLFWwindow* ptr_window;
			Vector2 window_size;
			std::string window_title;
		public:

			NIKEWindow(Vector2 window_size, std::string window_title);

			NIKEWindow(std::string const& file_path);

			void configWindow() override;

			void pollEvents() override;

			void swapBuffers() override;

			void setWindowTitle(const std::string& title) override;

			std::string getWindowTitle() const override;

			void setWindowSize(float width, float height) override;

			Vector2 getWindowSize() const override;

			bool windowState() const override;

			void terminate() override;

			void cleanUp() override;

			~NIKEWindow() override;
		};

		#endif //Expose implementation only to NIKE Engine

		class NIKESAURUS_API Manager {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

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
			Manager() : ptr_window{ nullptr }, delta_time{ 0.0f }, target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f } {}

			//Arguement Constructor
			Manager(std::shared_ptr<IWindow> window);

			//Set Window
			void setWindow(std::shared_ptr<IWindow> window);

			//Get Window
			std::shared_ptr<IWindow> getWindow();

			//Create Console
			void createConsole();

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