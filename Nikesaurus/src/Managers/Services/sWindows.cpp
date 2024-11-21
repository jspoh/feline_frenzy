/*****************************************************************//**
* \file   mWindows.cpp
* \brief  Windows manager
*
* \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (80%)
* \co-author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (20%)
* \date   September 2024
* All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
*********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/sWindows.h"
#include "Managers/Services/sEvents.h"
#include "Core/Engine.h"

namespace NIKE {
	/*****************************************************************//**
	* NIKE Window
	*********************************************************************/
	Windows::NIKEWindow::NIKEWindow(Vector2i window_size, std::string window_title)
		: ptr_window{ nullptr }, window_pos(), window_size{ window_size }, window_title{ window_title }, b_full_screen{ false }
	{
	}

	Windows::NIKEWindow::NIKEWindow(nlohmann::json const& config)
		: ptr_window{ nullptr }, b_full_screen{ false }
	{
		try {
			auto const& data = config.at("WindowsConfig");
			window_title = data.at("Title").get<std::string>();
			window_size.fromJson(data.at("Window_Size"));
		}
		catch(const nlohmann::json::exception& e) {
			NIKEE_CORE_WARN(e.what());
			NIKEE_CORE_WARN("Window config invalid! Reverting to default window config");

			window_title = "Window";
			window_size = { 1600, 900 };
		}

		//Configure Window Setup
		configWindow();
	}

	void Windows::NIKEWindow::configWindow() {
		if (!glfwInit()) {
			cerr << "Failed to initialize GLFW\n";
			throw std::exception();
		}

		glfwSetErrorCallback([](int error, const char* description) {
			cerr << "Error " << error << ": " << description << endl;
			throw std::exception();
			});

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window dimensions are static

		//Create window
		ptr_window = glfwCreateWindow(static_cast<int>(window_size.x), static_cast<int>(window_size.y), window_title.c_str(), nullptr, nullptr);
		if (!ptr_window) {
			cerr << "Failed to create window" << endl;
			glfwTerminate();
			throw std::exception();
		}

		glfwMakeContextCurrent(ptr_window);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			cerr << "GLEW init failed: " << glewGetErrorString(err) << endl;
			throw std::exception();
		}

		//Engine Init Successful
		NIKEE_CORE_INFO("GL init success");

		// enable debug logging
#ifndef NDEBUG
		// !TODO: re-enable this
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback([]([[maybe_unused]] GLenum source, [[maybe_unused]] GLenum type, [[maybe_unused]] GLuint id, [[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length, [[maybe_unused]] const GLchar* message, [[maybe_unused]] const void* userParam) {
			//cerr << "GL Debug Message: " << message << "\nSource: " << source << endl;
			//NIKEE_CORE_ERROR("GL Debug Message: {0}\nSource: {1}", message, source);
			}, nullptr);
#endif
	}

	std::shared_ptr<Windows::IWindow> Windows::Service::getWindow() {
		return ptr_window;
	}

	void Windows::NIKEWindow::setWindowMode(int mode, int value) {
		glfwSetWindowAttrib(ptr_window, mode, value);
	}

	int Windows::NIKEWindow::queryWindowMode(int mode) {
		return glfwGetWindowAttrib(ptr_window, mode);
	}

	void Windows::NIKEWindow::setFullScreen(int value) {

		//Attributes
		GLFWmonitor* monitor;
		const GLFWvidmode* mode;


		if (value == GLFW_TRUE && !b_full_screen) {
			//Get FullScreen Attributes
			monitor = glfwGetPrimaryMonitor();
			mode = glfwGetVideoMode(monitor);

			// Get the window position and size
			glfwGetWindowPos(ptr_window, &window_pos.x, &window_pos.y);
			glfwGetWindowSize(ptr_window, &size_before_fullscreen.x, &size_before_fullscreen.y);

			// Recreate the window in fullscreen mode
			glfwSetWindowMonitor(ptr_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

			//Set Full Screen Mode True
			b_full_screen = true;
		}

		if (value == GLFW_FALSE && b_full_screen) {
			// Go to windowed mode
			monitor = nullptr;

			// Recreate the window in windowed mode at the stored position and size
			glfwSetWindowMonitor(ptr_window, nullptr, window_pos.x, window_pos.y, size_before_fullscreen.x, size_before_fullscreen.y, 0);

			//Set Full Screen Mode False
			b_full_screen = false;
		}
	}

	bool Windows::NIKEWindow::getFullScreen() const {
		return b_full_screen;
	}

	void Windows::NIKEWindow::setupEventCallbacks() {
		glfwSetFramebufferSizeCallback(ptr_window, Events::Service::fbsize_cb);
		glfwSetKeyCallback(ptr_window, Events::Service::key_cb);
		glfwSetMouseButtonCallback(ptr_window, Events::Service::mousebutton_cb);
		glfwSetCursorPosCallback(ptr_window, Events::Service::mousepos_cb);
		glfwSetScrollCallback(ptr_window, Events::Service::mousescroll_cb);
		glfwSetWindowFocusCallback(ptr_window, Events::Service::windowfocus_cb);
	    glfwSetDropCallback(ptr_window, Events::Service::dropfile_cb);
	}

	void Windows::NIKEWindow::setInputMode(int mode, int value) {
		glfwSetInputMode(ptr_window, mode, value);
	}

	void Windows::NIKEWindow::pollEvents() {
		//Poll system events ( Interativity with app )
		glfwPollEvents();
	}

	void Windows::NIKEWindow::swapBuffers() {
		glfwSwapBuffers(ptr_window);
	}

	void Windows::NIKEWindow::clearBuffer() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Windows::NIKEWindow::setWindowTitle(const std::string& title) {
		glfwSetWindowTitle(ptr_window, title.c_str());
	}

	std::string Windows::NIKEWindow::getWindowTitle() const {
		return window_title;
	}

	void Windows::NIKEWindow::setWindowSize(int width, int height) {
		window_size.x = width;
		window_size.y = height;

		glfwSetWindowSize(ptr_window, static_cast<int>(window_size.x), static_cast<int>(window_size.y));
	}

	Vector2i Windows::NIKEWindow::getWindowSize() const {
		if (b_full_screen) {
			return size_before_fullscreen;
		}
		else {
			return window_size;
		}
	}

	GLFWwindow* Windows::NIKEWindow::getWindowPtr() const {
		return ptr_window;
	}

	Vector2i Windows::NIKEWindow::getWindowPos() {
		glfwGetWindowPos(ptr_window, &window_pos.x, &window_pos.y);
		return window_pos;
	}

	bool Windows::NIKEWindow::windowState() const {
		return !glfwWindowShouldClose(ptr_window);
	}

	void Windows::NIKEWindow::terminate() {
		//Close Window
		glfwSetWindowShouldClose(ptr_window, GLFW_TRUE);
	}

	void Windows::NIKEWindow::cleanUp() {
		//Clean up window
		glfwDestroyWindow(ptr_window);
		glfwTerminate();
	}

	Windows::NIKEWindow::~NIKEWindow() {
		// When the window closes, wait for user input before closing the console
		cout << "Press enter to close the console..." << endl;
		std::cin.get();
	}

	void Windows::NIKEWindow::onEvent(std::shared_ptr<WindowResized> event) {
		glViewport(0, 0, event->frame_buffer.x, event->frame_buffer.y);
		window_size = event->frame_buffer;
	}

	void Windows::NIKEWindow::onEvent(std::shared_ptr<WindowFocusEvent> event) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL WindowFocusEvent error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		static bool is_fullscreen;

		if (event->focused) {
			glfwRestoreWindow(ptr_window);

			if (is_fullscreen) {
				setFullScreen(true);
			}

			// in case of resizes
			int width, height;
			glfwGetFramebufferSize(ptr_window, &width, &height);
			glViewport(0, 0, width, height);

			// just in case
			glfwMakeContextCurrent(ptr_window);

			NIKE_AUDIO_SERVICE->resumeAllChannels();
		}
		else {
			GLFWmonitor* monitor = glfwGetWindowMonitor(ptr_window);
			is_fullscreen = !!monitor;		// will be NULL if not fullscreen

			NIKE_AUDIO_SERVICE->pauseAllChannels();
			glfwIconifyWindow(ptr_window);
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL WindowFocusEvent error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	/*****************************************************************//**
	* Window Service
	*********************************************************************/
	Windows::Service::Service(std::shared_ptr<IWindow> window)
		:	ptr_window{ window }, delta_time{ 0.0f }, target_fps{ 60 }, 
			actual_fps{ 0.0f }, curr_time{ 0.0f }, curr_num_steps{ 0 },
			accumulated_time{ 0.0 } {}

	void Windows::Service::setWindow(std::shared_ptr<IWindow> window) {
		ptr_window = window;
	}

	//Create console
	void Windows::Service::createConsole(std::string const& custom_welcome) {
		// Create a new console window
		AllocConsole();

		FILE* fp;

		// Redirect cin and cout to the new console
		// Redirect stdout
		freopen_s(&fp, "CONOUT$", "w", stdout);
		// Redirect stderr
		freopen_s(&fp, "CONOUT$", "w", stderr);
		// Redirect stdin
		freopen_s(&fp, "CONIN$", "r", stdin);

		//Welcome Message
		cout << custom_welcome << endl;
	}

	void Windows::Service::setTargetFPS(int fps) {
		target_fps = fps;
	}

	float Windows::Service::getCurrentFPS() const {
		return actual_fps;
	}

	float Windows::Service::getDeltaTime() const {
		return delta_time;
	}

	float Windows::Service::getFixedDeltaTime() const {
		return (static_cast<float>(1) / target_fps);
	}

	int Windows::Service::getCurrentNumOfSteps() const {
		return curr_num_steps;
	}

	float Windows::Service::getInterpolationFactor() const {
		return static_cast<float>(accumulated_time / (static_cast<float>(1) / target_fps));
	}

	void Windows::Service::calculateDeltaTime() {
		//Static prev time
		static double prev_time = glfwGetTime();

		//Calculate delta time
		curr_time = glfwGetTime();
		delta_time = static_cast<float>(curr_time - prev_time);
		actual_fps = 1.0f / delta_time;
		prev_time = curr_time;

		//Reset curr num of steps
		curr_num_steps = 0;

		//control frame rate
		accumulated_time += delta_time;
		while (accumulated_time >= (static_cast<double>(1) / target_fps)) {
			accumulated_time -= (static_cast<double>(1) / target_fps);
			curr_num_steps++;
		}
	}
}
