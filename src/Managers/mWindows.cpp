/*****************************************************************//**
 * \file   mWindows.cpp
 * \brief  Windows manager
 *
 * \author Ho
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Managers/mWindows.h"

void Windows::Manager::readConfigFile(std::string const& file_path) {

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

void Windows::Manager::configSystem() {
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

void Windows::Manager::terminate() {
	glfwSetWindowShouldClose(ptr_window, GLFW_TRUE);
}

void Windows::Manager::setWinTitle(std::string const& title) {
	glfwSetWindowTitle(ptr_window, title.c_str());
}

std::string const& Windows::Manager::getWinTitle() {
	return window_title;
}

GLFWwindow* Windows::Manager::getWindow() const {
	return ptr_window;
}

void Windows::Manager::setWindowSize(float width, float height) {
	window_size.x = width;
	window_size.y = height;

	glfwSetWindowSize(ptr_window, static_cast<int>(window_size.x), static_cast<int>(window_size.y));
}

Vector2 const& Windows::Manager::getWindowSize() const {
	return window_size;
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
	//double frame_time = glfwGetTime() - curr_time;

	//Sleep thread for fps
	while (glfwGetTime() - curr_time < target_frame_time) {
		//Current default system timer resolution is around 15.6 ms
		//Sleeping this thread will lead to inaccurate time control, thread sleeps for too long
		//std::this_thread::sleep_for(std::chrono::duration<double>(target_frame_time - frame_time));
	}
}