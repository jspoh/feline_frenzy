/*****************************************************************//**
 * \file   GLManager.h
 * \brief  
 * 
 * \author jings
 * \date   11 September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"

class GLManager {
private:
	GLManager();
	~GLManager();

	static constexpr int INITIAL_WINDOW_WIDTH = 1600;
	static constexpr int INITIAL_WINDOW_HEIGHT = 900;
	static constexpr const char* INITIAL_WINDOW_TITLE = "Feline Frenzy";

	/**
	 * sets up event callback functions using InputManager class.
	 *
	 */
	void setupEventCallbacks();

	std::unordered_map<std::string, unsigned int> shader_programs;

	/**
	 * compiles shader and adds to shader_programs.
	 * 
	 * \param shader_ref	shader program's reference string
	 * \param vtx_path		path to vertex shader
	 * \param frag_path		path to fragment shader
	 */
	void compileShader(std::string shader_ref, std::string vtx_path, std::string frag_path);
public:
	GLFWwindow* ptr_window = nullptr;
	int window_width = INITIAL_WINDOW_WIDTH;
	int window_height = INITIAL_WINDOW_HEIGHT;
	static float dt;

	/**
	 * instance manager for singleton class.
	 * 
	 * \return instance
	 */
	static GLManager& getInstance();

	/**
	 * initializes GLManager.
	 * 
	 * \return 
	 */
	bool init();

	void run();

	void useShader(std::string shader_ref);

	void unuseShader();

	void setTitle(std::string title);
};
