/*****************************************************************//**
 * \file   Engine.cpp
 * \brief  Graphics library manager for engine
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Core/Engine.h"

Core::Engine::Engine()
	: ptr_window{ nullptr }, window_size{}, window_title{""}, delta_time{0.0f},
	target_fps{ 60 }, actual_fps{ 0.0f }, curr_time{ 0.0f }
{
}

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
	entity_manager = std::make_unique<Entity::Manager>();
	component_manager = std::make_unique<Component::Manager>();
	system_manager = std::make_unique<System::Manager>();
	events_manager = std::make_unique<Events::Manager>();
	asset_manager = std::make_unique<Asset::Manager>();
	collision_manager = std::make_unique<Collision::Manager>();

	//Scenes manager
	scenes_manager = std::make_unique<Scenes::Manager>();
	addEventListeners<Scenes::ChangeSceneEvent>(scenes_manager);

	//Read config file
	readConfigFile(file_path);

	//Config glfw window system
	configSystem();

	//Set Target FPS
	target_fps = fps;
}

void Core::Engine::run() {

	while (!glfwWindowShouldClose(NIKEEngine.getWindow())) {

		//Calculate Delta Time
		calculateDeltaTime();

		//Poll system events ( Interativity with app )
		glfwPollEvents();

		//Set Window Title
		setWinTitle(window_title + " | " + std::to_string(actual_fps) + " fps");

		//Update game scenes
		scenes_manager->updateScenes();

		//Update all systems
		system_manager->updateSystems();

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

/*****************************************************************//**
* Entity Methods
*********************************************************************/
Entity::Type Core::Engine::createEntity() {
	return entity_manager->createEntity();
}

void Core::Engine::destroyEntity(Entity::Type entity) {

	//Destroy all data related to entity
	entity_manager->destroyEntity(entity);
	component_manager->entityDestroyed(entity);
	system_manager->entityDestroyed(entity);
}
