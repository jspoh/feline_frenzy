/*****************************************************************//**
 * \file   sysShader.cpp
 * \brief
 *
 * \author g.boonxuensean@digipen.edu
 * \coauthor Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/Render/sysShader.h"
#include "../headers/Core/Engine.h"

unsigned int Shader::Manager::compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
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

unsigned int Shader::Manager::loadShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
	return compileShader(shader_ref, vtx_path, frag_path);
}

void Shader::Manager::useShader(const std::string& shader_ref) {
	glUseProgram(NIKEEngine.accessAssets()->getShader(shader_ref));
}

void Shader::Manager::unuseShader() {
	glUseProgram(0);
}

// Uniform setters

void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, int value) {
	int location = glGetUniformLocation(NIKEEngine.accessAssets()->getShader(shader_ref), name.c_str());
	if (location >= 0) {
		glUniform1i(location, value);
	}
	else {
		cerr << "Uniform location not found for: " << name << endl;
	}
}

void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, float value) {
	int location = glGetUniformLocation(NIKEEngine.accessAssets()->getShader(shader_ref), name.c_str());
	if (location >= 0) {
		glUniform1f(location, value);
	}
	else {
		cerr << "Uniform location not found for: " << name << endl;
	}
}

void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, const Matrix33::Matrix_33& value) {
	int location = glGetUniformLocation(NIKEEngine.accessAssets()->getShader(shader_ref), name.c_str());
	if (location >= 0) {
		glUniformMatrix3fv(location, 1, GL_FALSE, &value(0,0));
	}
	else {
		cerr << "Uniform location not found for: " << name << endl;
	}
}

void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, const Vector3& value) {
	int location = glGetUniformLocation(NIKEEngine.accessAssets()->getShader(shader_ref), name.c_str());
	if (location >= 0) {
		glUniform3fv(location, 1, &value.x);
	}
	else {
		cerr << "Uniform location not found for: " << name << endl;
	}
}
