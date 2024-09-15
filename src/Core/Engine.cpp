/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Graphics library manager for engine
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "Engine.h"
#include "StateManager.h"

// cannot set to 0 in case of division by 0!!
//float Core::Engine::dt = 1.f;

Core::Engine::Engine()
	: ptr_window{ nullptr }, window_width{ 0 }, window_height{ 0 }, window_title{ "" }, delta_time{ 0.0f }
{}

Core::Engine::~Engine() {
	glfwTerminate();
}

void Core::Engine::readConfigFile(std::string const& file_path) {

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
		std::stringstream(data) >> temp >> window_width;
		std::getline(fileStream, data);
		std::stringstream(data) >> temp >> window_height;
	}

	//Close file stream
	fileStream.close();
}

void Core::Engine::calculateDeltaTime() {
	static float prev_time = static_cast<float>(glfwGetTime());
	float curr_time = static_cast<float>(glfwGetTime());
	delta_time = curr_time - prev_time;
	prev_time = curr_time;
}

bool Core::Engine::getGameRunning() const {
	return !glfwWindowShouldClose(NIKEEngine.getWindow());
}

void Core::Engine::init(std::string const& file_path) {
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

	//Read config file
	readConfigFile(file_path);

	//Create window
	ptr_window = glfwCreateWindow(window_width, window_height, window_title.c_str(), nullptr, nullptr);
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

	// initialize states
	StateManager::getInstance().register_all_states();		// important!
	StateManager::getInstance().set_active_state("main_menu");

	//Engine Init Successful
	cout << "GL init success" << endl;
}

void Core::Engine::run() {

	while (getGameRunning()) {

		//Calculate Delta Time
		calculateDeltaTime();

		glfwPollEvents();

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		setTitle(window_title + " | " + std::to_string(1.f / delta_time) + " fps");

		//Update all systems
		for (auto& system : systems) {
			if (system->getSystemActive()) {
				system->update();
			}
		}

		StateManager::getInstance().run();

		//Might move this into render system
		glfwSwapBuffers(ptr_window);

		//Input To Terminate Engine
		if (!getGameRunning()) {

			NIKEEngine.terminate();
		}
	}
}

void Core::Engine::terminate() {
	glfwSetWindowShouldClose(NIKEEngine.ptr_window, GLFW_TRUE);
}

void Core::Engine::setTitle(std::string const& title) {
	glfwSetWindowTitle(ptr_window, title.c_str());
}

GLFWwindow* Core::Engine::getWindow() const {
	return ptr_window;
}

void Core::Engine::setWindowWidth(int width) {
	window_width = width;
}

int Core::Engine::getWindowWidth() const {
	return window_width;
}

void Core::Engine::setWindowHeight(int height) {
	window_height = height;
}

int Core::Engine::getWindowHeight() const {
	return window_height;
}

float Core::Engine::getDeltaTime() const {
	return delta_time;
}

void Core::Engine::addSystem(std::shared_ptr<System::Base> system, size_t index) {

	if (index == std::string::npos && index >= systems.size()) {
		systems.push_back(system);
	}
	else {
		auto it = systems.begin();
		systems.insert(it + index, system);
	}
}
