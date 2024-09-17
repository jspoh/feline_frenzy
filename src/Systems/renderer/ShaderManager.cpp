/*****************************************************************//**
 * \file   ShaderManager.cpp
 * \brief
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "stdafx.h"
#include "ShaderManager.h"

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
	for (auto& [ref, shader_program] : shader_programs) {
		glDeleteProgram(shader_program);
	}
}

unsigned int ShaderManager::compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
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

	return shader_handle;
}

void ShaderManager::loadShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
	if (shader_programs.find(shader_ref) == shader_programs.end()) {
		shader_programs[shader_ref] = compileShader(shader_ref, vtx_path, frag_path);
	}
}

void ShaderManager::useShader(const std::string& shader_ref) {
	if (shader_programs.find(shader_ref) != shader_programs.end()) {
		glUseProgram(shader_programs[shader_ref]);
	}
	else {
		cerr << "Shader reference not found: " << shader_ref << endl;
		throw std::exception();
	}
}

void ShaderManager::unuseShader() {
	glUseProgram(0);
}