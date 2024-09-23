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
	: ptr_window{ nullptr }, window_size{}, window_title{""}, delta_time{0.0f},
	target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f }
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
		std::stringstream(data) >> temp >> window_size.x;
		std::getline(fileStream, data);
		std::stringstream(data) >> temp >> window_size.y;
	}

	//Close file stream
	fileStream.close();
}

void Core::Engine::configSystem() {
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

void Core::Engine::calculateDeltaTime() {
	//Static prev time
	static double prev_time = glfwGetTime();

	//Calculate delta time
	curr_time = glfwGetTime();
	delta_time = static_cast<float>(curr_time - prev_time);
	actual_fps = 1.0f / delta_time;
	prev_time = curr_time;
}

void Core::Engine::controlFPS() {

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

void Core::Engine::init(std::string const& file_path, int fps) {
	//Read config file
	readConfigFile(file_path);

	//Config glfw window system
	configSystem();

	//Set Target FPS
	target_fps = fps;

	// initialize states
	StateManager::getInstance().register_all_states();		// important!
	StateManager::getInstance().set_active_state("main_menu");
}

void Core::Engine::run() {

	while (!glfwWindowShouldClose(NIKEEngine.getWindow())) {

		//Calculate Delta Time
		calculateDeltaTime();

		//Poll system events ( Interativity with app )
		glfwPollEvents();

		//Set BG Color
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set Window Title
		setWinTitle(window_title + " | " + std::to_string(actual_fps) + " fps");

		//Update all systems
		for (auto& system : systems) {
			if (system->getActiveState()) {
				system->update();
			}
		}

		//To disable v-sync ( testing fps control )
		//glfwSwapInterval(0);

		//State Manager
		StateManager::getInstance().run();

		//Might move this into render system
		glfwSwapBuffers(ptr_window);

		//Check if window is open
		if (glfwWindowShouldClose(NIKEEngine.getWindow())) {

			//Terminate window
			NIKEEngine.terminate();
		}

		//Control FPS
		controlFPS();
	}
}

void Core::Engine::terminate() {
	glfwSetWindowShouldClose(NIKEEngine.ptr_window, GLFW_TRUE);
}

void Core::Engine::setWinTitle(std::string const& title) {
	glfwSetWindowTitle(ptr_window, title.c_str());
}

GLFWwindow* Core::Engine::getWindow() const {
	return ptr_window;
}

void Core::Engine::setWindowSize(float width, float height) {
	window_size.x = width;
	window_size.y = height;

	glfwSetWindowSize(ptr_window, static_cast<int>(window_size.x), static_cast<int>(window_size.y));
}

Vector2 const& Core::Engine::getWindowSize() const {
	return window_size;
}

void Core::Engine::setTargetFPS(int fps) {
	target_fps = fps;
}

float Core::Engine::getCurrentFPS() const {
	return actual_fps;
}

float Core::Engine::getDeltaTime() const {
	return delta_time;
}

void Core::Engine::addSystem(std::shared_ptr<System::ISystem> system, std::string const& sys_identifier, size_t index) {

	//Check if system has already been created
	if (systems_map.find(sys_identifier) != systems_map.end()) {
		return;
	}

	//Check for index
	if (index == std::string::npos && index >= systems.size()) {
		//Insert system at back
		systems.push_back(system);
	}
	else {
		//Insert system
		auto it = systems.begin();
		systems.insert(it + index, system);
	}

	//Emplace shared pointer to system in map
	systems_map.emplace(std::piecewise_construct, std::forward_as_tuple(sys_identifier), std::forward_as_tuple(system));
}

std::shared_ptr<System::ISystem> Core::Engine::accessSystem(std::string const& sys_identifier) {
	return systems_map.at(sys_identifier);
}
