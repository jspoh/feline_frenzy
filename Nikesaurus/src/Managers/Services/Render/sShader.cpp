/*****************************************************************//**
 * \file   sShader.cpp
 * \brief
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu(70%)
 * \coauthor Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (30%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Render/sShader.h"


namespace NIKE {
	void Shader::ShaderManager::compileShader(std::string const& shader_ref, const std::string& vtx_path, const std::string& frag_path) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

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

		// Check vertex shader compilation
		int success;
		char info_log[512];
		glGetShaderiv(vtx_handle, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vtx_handle, 512, nullptr, info_log);
			NIKEE_CORE_ERROR("Vertex shader compilation failed for {0}:\n{1}", shader_ref, info_log);
			throw std::exception();
		}

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

		// Check fragment shader compilation
		success;
		info_log[512];
		glGetShaderiv(frag_handle, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vtx_handle, 512, nullptr, info_log);
			NIKEE_CORE_ERROR("Fragment shader compilation failed for {0}:\n{1}", shader_ref, info_log);
			throw std::exception();
		}

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
		success = false;
		glGetProgramiv(shader_handle, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(shader_handle, 512, nullptr, info_log);
			cerr << "Failed to link shader program " << ": " << info_log << endl;
			throw std::exception();
		}

		// cleanup shaders
		glDeleteShader(vtx_handle);
		glDeleteShader(frag_handle);

		NIKEE_CORE_INFO("Sucessfully loaded shader from " + vtx_path + " " + frag_path);

		shaders[shader_ref] = shader_handle;

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Shader::ShaderManager::init() {
		//Load all shaders
		compileShader("base", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/base.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/base.frag").string());
		compileShader("batched_base", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/batched_base.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/batched_base.frag").string());
		compileShader("texture", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/texture.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/texture.frag").string());
		compileShader("batched_texture", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/batched_texture.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/batched_texture.frag").string());
		compileShader("text", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/text.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/text.frag").string());
		compileShader("base_particle", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/base_particle.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/base_particle.frag").string());
		compileShader("circle_particle", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/circle_particle.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/circle_particle.frag").string());
		compileShader("quad_particle", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/quad_particle.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/quad_particle.frag").string());
		compileShader("textured_particle", NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/textured_particle.vert").string(), NIKE_PATH_SERVICE->resolvePath("Engine_Assets:/Shaders/textured_particle.frag").string());
	}

	void Shader::ShaderManager::useShader(const std::string& shader_ref) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		glUseProgram(shaders.at(shader_ref));

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Shader::ShaderManager::unuseShader() {
		glUseProgram(0);
	}

	// Uniform setters

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, int value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform1i(location, value);
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, float value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform1f(location, value);
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, const Matrix_33& value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniformMatrix3fv(location, 1, GL_FALSE, &value(0, 0));
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, const Vector3f& value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform3fv(location, 1, &value.x);
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, const Vector2f& value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform2f(location, value.x, value.y);
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, const Vector4f& value) {
		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}
		int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform4f(location, value.x, value.y, value.z, value.w);
		}
		else {
			// cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::ShaderManager::setUniform(const std::string& shader_ref, const std::string& name, const std::vector<unsigned int>& vals) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at start of {0}: {1}", __FUNCTION__, err);
		}

		if (shaders.find(shader_ref) == shaders.end()) {
			throw std::runtime_error("Shader does not exist.");
		}

		const int location = glGetUniformLocation(shaders.at(shader_ref), name.c_str());
		if (location >= 0) {
			for (size_t i{}; i < vals.size(); i++) {
				const int idx_loc = glGetUniformLocation(shaders.at(shader_ref), (name + "[" + std::to_string(i) + "]").c_str());
				if (idx_loc >= 0) {
					glUniform1i(idx_loc, vals[i]);
				}
				else {
					cerr << "Uniform location not found in shader " << shader_ref << " for: " << name << "[" << i << "]" << endl;
					throw std::exception();
				}
			}
		}
		else {
			cerr << "Uniform location not found in shader " << shader_ref << " for: " << name << endl;
			//throw std::exception();
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	Shader::ShaderManager::~ShaderManager() {
		//Clear shaders
		for (auto& shader : shaders) {
			glDeleteProgram(shader.second);
		}
	}
}