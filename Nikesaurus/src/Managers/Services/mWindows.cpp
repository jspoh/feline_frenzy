 /*****************************************************************//**
 * \file   mWindows.cpp
 * \brief  Windows manager
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Managers/Services/mWindows.h"

namespace NIKESAURUS {
	/*****************************************************************//**
	* NIKE Window
	*********************************************************************/
	Windows::NIKEWindow::NIKEWindow(Vector2 window_size, std::string window_title)
		: ptr_window{ nullptr }, window_size{ window_size }, window_title{ window_title }
	{
	}

	Windows::NIKEWindow::NIKEWindow(std::string const& file_path) 
		: ptr_window{nullptr}
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
		cout << "GL init success" << endl;
	}

	void Windows::NIKEWindow::pollEvents() {
		//Poll system events ( Interativity with app )
		glfwPollEvents();
	}

	void Windows::NIKEWindow::swapBuffers() {
		glfwSwapBuffers(ptr_window);
	}

	void Windows::NIKEWindow::setWindowTitle(const std::string& title) {
		glfwSetWindowTitle(ptr_window, title.c_str());
	}

	std::string Windows::NIKEWindow::getWindowTitle() const {
		return window_title;
	}

	void Windows::NIKEWindow::setWindowSize(float width, float height) {
		window_size.x = width;
		window_size.y = height;

		glfwSetWindowSize(ptr_window, static_cast<int>(window_size.x), static_cast<int>(window_size.y));
	}

	Vector2 Windows::NIKEWindow::getWindowSize() const {
		return window_size;
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
		cout << "Press any key to close the console..." << endl;
		std::cin.get();
	}

	/*****************************************************************//**
	* Window Manager
	*********************************************************************/
	Windows::Manager::Manager(std::shared_ptr<IWindow> window) 
		: ptr_window{ window }, delta_time{ 0.0f }, target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f } {}

	void Windows::Manager::setWindow(std::shared_ptr<IWindow> window) {
		ptr_window = window;
	}

	std::shared_ptr<Windows::IWindow> Windows::Manager::getWindow() {
		return ptr_window;
	}

	//Create console
	void Windows::Manager::createConsole(std::string const& custom_welcome) {
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

	void Windows::Manager::setTargetFPS(int fps) {
		target_fps = fps;
	}

	float Windows::Manager::getCurrentFPS() const {
		return actual_fps;
	}

	float Windows::Manager::getDeltaTime() const {
		return delta_time;
	}

	void Windows::Manager::calculateDeltaTime() {
		//Static prev time
		static double prev_time = glfwGetTime();

		//Calculate delta time
		curr_time = glfwGetTime();
		delta_time = static_cast<float>(curr_time - prev_time);
		actual_fps = 1.0f / delta_time;
		prev_time = curr_time;
	}

	void Windows::Manager::controlFPS() {

		//Target delta time
		double target_frame_time = 1.0 / target_fps;

		//Limit FPS based on target frame time
		while (glfwGetTime() - curr_time < target_frame_time) {
		}
	}
}
