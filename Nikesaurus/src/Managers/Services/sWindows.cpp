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

namespace NIKE {
	/*****************************************************************//**
	* NIKE Window
	*********************************************************************/
	Windows::NIKEWindow::NIKEWindow(Vector2i window_size, std::string window_title)
		: ptr_window{ nullptr }, window_pos(), window_size{ window_size }, window_title{ window_title }, b_full_screen{ false }
	{
	}

	Windows::NIKEWindow::NIKEWindow(std::string const& file_path)
		: ptr_window{ nullptr }, b_full_screen{ false }
	{
		//Get file stream
		std::fstream fileStream;
		fileStream.open(file_path, std::ios::in);

		//Temp string
		std::string temp;

		//Extract data from file stream
		if (fileStream) {
			std::string data;
			std::getline(fileStream, data);
			window_title = data.substr(data.find_first_of('"') + 1, data.find_last_of('"') - data.find_first_of('"') - 1);
			std::getline(fileStream, data);
			std::stringstream(data) >> temp >> window_size.x;
			std::getline(fileStream, data);
			std::stringstream(data) >> temp >> window_size.y;
		}

		//Close file stream
		fileStream.close();

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

		//Static Window Size For Remembering Size Before FullScreen
		static Vector2i win_size;

		if (value == GLFW_TRUE && !b_full_screen) {
			//Get FullScreen Attributes
			monitor = glfwGetPrimaryMonitor();
			mode = glfwGetVideoMode(monitor);

			// Get the window position and size
			glfwGetWindowPos(ptr_window, &window_pos.x, &window_pos.y);
			glfwGetWindowSize(ptr_window, &win_size.x, &win_size.y);

			// Recreate the window in fullscreen mode
			glfwSetWindowMonitor(ptr_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

			//Set Full Screen Mode True
			b_full_screen = true;
		}

		if (value == GLFW_FALSE && b_full_screen) {
			// Go to windowed mode
			monitor = nullptr;

			// Recreate the window in windowed mode at the stored position and size
			glfwSetWindowMonitor(ptr_window, nullptr, window_pos.x, window_pos.y, win_size.x, win_size.y, 0);

			//Set Full Screen Mode False
			b_full_screen = false;
		}
	}

	void Windows::NIKEWindow::setupEventCallbacks() {
		glfwSetFramebufferSizeCallback(ptr_window, Events::Service::fbsize_cb);
		glfwSetKeyCallback(ptr_window, Events::Service::key_cb);
		glfwSetMouseButtonCallback(ptr_window, Events::Service::mousebutton_cb);
		glfwSetCursorPosCallback(ptr_window, Events::Service::mousepos_cb);
		glfwSetScrollCallback(ptr_window, Events::Service::mousescroll_cb);
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
		return window_size;
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
		// These 3 functions are from cleanup() in imgui sys, this is temp, theres no accessSys anymore sadly
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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

	/*****************************************************************//**
	* Window Service
	*********************************************************************/
	Windows::Service::Service(std::shared_ptr<IWindow> window)
		: ptr_window{ window }, delta_time{ 0.0f }, target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f } {}

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

	void Windows::Service::calculateDeltaTime() {
		//Static prev time
		static double prev_time = glfwGetTime();

		//Calculate delta time
		curr_time = glfwGetTime();
		delta_time = static_cast<float>(curr_time - prev_time);
		actual_fps = 1.0f / delta_time;
		prev_time = curr_time;
	}

	void Windows::Service::controlFPS() {

		//Target delta time
		double target_frame_time = 1.0 / target_fps;

		//Limit FPS based on target frame time
		while (glfwGetTime() - curr_time < target_frame_time) {
		}
	}
}
