/*****************************************************************//**
 * \file   sysShader.cpp
 * \brief
 *
 * \author Sean Gwee, 2301326, g.boonxuensean@digipen.edu(90%)
 * \coauthor Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (10%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/Render/sysShader.h"
#include "Core/Engine.h"

namespace NIKESAURUS {

	void Shader::Manager::useShader(const std::string& shader_ref) {
		glUseProgram(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref));
	}

	void Shader::Manager::unuseShader() {
		glUseProgram(0);
	}

	// Uniform setters

	void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, int value) {
		int location = glGetUniformLocation(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform1i(location, value);
		}
		else {
			cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, float value) {
		int location = glGetUniformLocation(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform1f(location, value);
		}
		else {
			cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, const Matrix_33& value) {
		int location = glGetUniformLocation(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref), name.c_str());
		if (location >= 0) {
			glUniformMatrix3fv(location, 1, GL_FALSE, &value(0, 0));
		}
		else {
			cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, const Vector3f& value) {
		int location = glGetUniformLocation(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform3fv(location, 1, &value.x);
		}
		else {
			cerr << "Uniform location not found for: " << name << endl;
		}
	}

	void Shader::Manager::setUniform(const std::string& shader_ref, const std::string& name, const Vector2f& value) {
		int location = glGetUniformLocation(NIKEEngine.getService<Assets::Service>()->getShader(shader_ref), name.c_str());
		if (location >= 0) {
			glUniform2f(location, value.x, value.y);
		}
		else {
			cerr << "Uniform location not found for: " << name << endl;
		}
	}
}