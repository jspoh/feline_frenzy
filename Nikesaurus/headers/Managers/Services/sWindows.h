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

namespace NIKE {
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

		// Window Focus class
		struct WindowFocusEvent : Events::IEvent {
			int focused;

			WindowFocusEvent(int focused)
				: focused{ focused } {}
		};

		//Abstract Window Class
		class IWindow 
			: public Events::IEventListener<WindowResized>, 
			public Events::IEventListener<WindowFocusEvent> {
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

			//Get full screen mode
			virtual bool getFullScreen() const = 0;

			//Get fullscreen scale
			virtual Vector2f getFullScreenScale() const = 0;

			//Setup Event Callbacks
			virtual void setupEventCallbacks() = 0;

			//Set input modes
			virtual void setInputMode(int mode, int value) = 0;

			//Poll window events
			virtual void pollEvents() = 0;

			//Swap buffers
			virtual void swapBuffers() = 0;

			//Clear buffer
			virtual void clearBuffer() = 0;

			//Set window title
			virtual void setWindowTitle(const std::string& title) = 0;

			//Get window title
			virtual std::string getWindowTitle() const = 0;

			//Get world size
			virtual Vector2f getWorldSize() const = 0;

			//Set window size
			virtual void setWindowSize(int width, int height) = 0;

			//Get window size
			virtual Vector2i getWindowSize() const = 0;

			//Get window viewport size
			virtual Vector2f getViewportSize() const = 0;

			//Get viewport ratio
			virtual Vector2f getViewportRatio() const = 0;

			//Get gap between viewport and window
			virtual Vector2f getViewportWindowGap() const = 0;

			//Set aspect ratio
			virtual void setAspectRatio(float ratio) = 0;

			//Get aspect ratio
			virtual float getAspectRatio() const = 0;

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

			virtual void onEvent(std::shared_ptr<WindowFocusEvent> event) override = 0;
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
			Vector2i size_before_fullscreen;
			Vector2i pos_before_fullscreen;
			float aspect_ratio;
			Vector2f viewport_size_before_fullscreen;
			Vector2f viewport_size;
			Vector2f world_size;

			//Internal viewport calculation
			void calculateViewport();

			//Configure Window
			void configWindow();

			//Warm up GPU
			void warmupGPU();
		public:

			NIKEWindow(Vector2i window_size, std::string window_title);

			NIKEWindow(nlohmann::json const& config);

			GLFWwindow* getWindowPtr() const;

			void setWindowMode(int mode, int value) override;

			int queryWindowMode(int mode) override;

			void setFullScreen(int value) override;

			bool getFullScreen() const override;

			Vector2f getFullScreenScale() const override;

			void setupEventCallbacks() override;

			void setInputMode(int mode, int value) override;

			void pollEvents() override;

			void swapBuffers() override;

			void clearBuffer() override;

			void setWindowTitle(const std::string& title) override;

			std::string getWindowTitle() const override;

			Vector2f getWorldSize() const override;

			void setWindowSize(int width, int height) override;

			Vector2i getWindowSize() const override;

			Vector2f getViewportSize() const override;

			Vector2f getViewportRatio() const override;

			Vector2f getViewportWindowGap() const override;

			void setAspectRatio(float ratio) override;

			float getAspectRatio() const override;

			Vector2i getWindowPos() override;

			bool windowState() const override;

			void terminate() override;

			void cleanUp() override;

			~NIKEWindow() override;

			void onEvent(std::shared_ptr<WindowResized> event) override;

			void onEvent(std::shared_ptr<WindowFocusEvent> event) override;
		};

		#endif //Expose implementation only to NIKE Engine

		/*****************************************************************//**
		* Window Service
		*********************************************************************/
		class NIKE_API Service {
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

			//Current number of steps ( Fixed DT )
			int curr_num_steps;

			//Accumulated time ( Fixed DT )
			double accumulated_time;

			bool window_is_focused{ true };

		public:
			//Default constructor
			Service() : ptr_window{ nullptr }, delta_time{ 0.0f }, target_fps{ 60 }, 
						actual_fps{ 0.0f }, curr_time{ 0.0f }, curr_num_steps{ 0 }, 
						accumulated_time{ 0.0 } {}

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

			//Get fixed delta time
			float getFixedDeltaTime() const;

			//Get current number of steps
			int getCurrentNumOfSteps() const;

			//Get interpolation factor
			float getInterpolationFactor() const;

			//Get window focus
			bool getWindowFocus() const;

			//Set window focus
			void setWindowFocus(bool focus);

			//Calculate Delta Time
			void calculateDeltaTime();

			//Reset opengl state
			void resetOpenGL();
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!WINDOWS_HPP