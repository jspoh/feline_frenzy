/*****************************************************************//**
 * \file   GLManager.cpp
 * \brief  Graphics library manager for engine
 * 
 * \author jings
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "GLManager.h"
#include "InputManager.h"
#include "StateManager.h"

// cannot set to 0 in case of division by 0!!
float GLManager::dt = 1.f;

GLManager& GLManager::getInstance() {
	static GLManager instance;
	return instance;
}


GLManager::GLManager() {
}

GLManager::~GLManager() {
	glfwTerminate();
}


bool GLManager::init() {
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW\n";
		return false;
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

	ptr_window = glfwCreateWindow(window_width, window_height, INITIAL_WINDOW_TITLE, nullptr, nullptr);
	if (!ptr_window) {
		cerr << "Failed to create window" << endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(ptr_window);

	setupEventCallbacks();

	glfwSetInputMode(ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cerr << "GLEW init failed: " << glewGetErrorString(err) << endl;
		return false;
	}

	// compile shaders
	compileShader("base", "shaders/base.vert", "shaders/base.frag");
	compileShader("tex", "shaders/textured_rendering.vert", "shaders/textured_rendering.frag");
	// !TODO: shader for text rendering(?)

	// initialize states
	StateManager::getInstance().register_all_states();		// important!
	StateManager::getInstance().set_active_state("main_menu");

	return true;
}


void GLManager::run() {
	static float prev_time = static_cast<float>(glfwGetTime());
	float curr_time = static_cast<float>(glfwGetTime());
	dt = curr_time - prev_time;
	prev_time = curr_time;

	glfwPollEvents();

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	setTitle(std::string{ INITIAL_WINDOW_TITLE } + " | " + std::to_string(1.f / dt) + " fps");

	StateManager::getInstance().run();

	glfwSwapBuffers(ptr_window);
}


void GLManager::setupEventCallbacks() {
	if (!ptr_window) {
		cerr << "Window not initialized" << endl;
		throw std::exception();
	}

	glfwSetFramebufferSizeCallback(ptr_window, InputManager::fbsize_cb);
	glfwSetKeyCallback(ptr_window, InputManager::key_cb);
	glfwSetMouseButtonCallback(ptr_window, InputManager::mousebutton_cb);
	glfwSetCursorPosCallback(ptr_window, InputManager::mousepos_cb);
	glfwSetScrollCallback(ptr_window, InputManager::mousescroll_cb);
}


void GLManager::compileShader(std::string shader_ref, std::string vtx_path, std::string frag_path) {
	// read and compile vertex shader
	std::ifstream vtx_file{ vtx_path };
	if (!vtx_file.is_open()) {
		cerr << "Failed to open vertex shader file: " << vtx_path << endl;
		throw std::exception();
	}

	std::stringstream vtx_buffer;
	vtx_buffer << vtx_file.rdbuf();
	vtx_file.close();
	const std::string vtx_str = vtx_buffer.str();
	const char* vtx_src = vtx_str.c_str();

	unsigned int vtx_handle = glCreateShader(GL_VERTEX_SHADER);
	if (!vtx_handle) {
		cerr << "Failed to create vertex shader program " << shader_ref << endl;
		throw std::exception();
	}
	glShaderSource(vtx_handle, 1, &vtx_src, nullptr);
	glCompileShader(vtx_handle);

	// read and compile fragment shader
	std::ifstream frag_file{ frag_path };
	if (!frag_file.is_open()) {
		cerr << "Failed to open fragment shader file: " << frag_path << endl;
		throw std::exception();
	}

	std::stringstream frag_buffer;
	frag_buffer << frag_file.rdbuf();
	frag_file.close();
	const std::string frag_str = frag_buffer.str();
	const char* frag_src = frag_str.c_str();

	unsigned int frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
	if (!frag_handle) {
		cerr << "Failed to create fragment shader program " << shader_ref << endl;
		throw std::exception();
	}
	glShaderSource(frag_handle, 1, &frag_src, nullptr);
	glCompileShader(frag_handle);

	// link shaders
	unsigned int shader_handle = glCreateProgram();
	if (!shader_handle) {
		cerr << "Failed to create shader program " << shader_ref << endl;
		throw std::exception();
	}

	glAttachShader(shader_handle, vtx_handle);
	glAttachShader(shader_handle, frag_handle);
	glLinkProgram(shader_handle);


	// validate shader program
	int success = false;
	glGetProgramiv(shader_handle, GL_LINK_STATUS, &success);

	if (!success) {
		char info_log[512];
		glGetProgramInfoLog(shader_handle, 512, nullptr, info_log);
		cerr << "Failed to link shader program " << shader_ref << ": " << info_log << endl;
		throw std::exception();
	}

	// cleanup shaders
	glDeleteShader(vtx_handle);
	glDeleteShader(frag_handle);

	shader_programs[shader_ref] = shader_handle;
}

void GLManager::useShader(std::string shader_ref) {
	if (shader_programs.find(shader_ref) == shader_programs.end()) {
		cerr << "Shader reference not found: " << shader_ref << endl;
		throw std::exception();
	}

	unsigned int shader_program = shader_programs[shader_ref];
	glUseProgram(shader_program);
}

void GLManager::unuseShader() {
	glUseProgram(0);
}

void GLManager::setTitle(std::string title) {
	glfwSetWindowTitle(ptr_window, title.c_str());
}
