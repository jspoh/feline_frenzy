/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author Poh Jing Seng (jingseng.poh@digipen.edu)
 * \date   11 September 2024
 *********************************************************************/

#include "stdafx.h"
#include "GLManager.h"
#include "InputManager.h"

// debug stuff
bool DEBUG = false;
NullStream nullstream;


int main() {
	bool gl_init_success = GLManager::getInstance().init();
	if (!gl_init_success) {
		cerr << "GL init failed" << endl;
		throw std::exception();
	}
	cout << "GL init success" << endl;

	while (!glfwWindowShouldClose(GLManager::getInstance().ptr_window)) {
		GLManager::getInstance().run();

		//glfwPollEvents();
		if (InputManager::getInstance().key_is_pressed(GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(GLManager::getInstance().ptr_window, GLFW_TRUE);
		}
	}

	return 0;
}

