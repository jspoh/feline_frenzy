/*****************************************************************//**
 * \file   ShaderManager.h
 * \brief
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#include "stdafx.h"

class ShaderManager {
	private:
		std::unordered_map<std::string, unsigned int> shader_programs;

		/**
		 * compiles shader and adds to shader_programs.
		 *
		 * \param shader_ref	shader program's reference string
		 * \param vtx_path		path to vertex shader
		 * \param frag_path		path to fragment shader
		 */
		unsigned int compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);

	public :
		ShaderManager();
		~ShaderManager();

		void loadShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);
		void useShader(const std::string& shader_ref);
		/**
		* Unbind the active shader program.
		*/
		void unuseShader();

		// Uniform setter overload functions
		void setUniform(const std::string& shader_ref, const std::string& name, int value);
		void setUniform(const std::string& shader_ref, const std::string& name, float value);
		void setUniform(const std::string& shader_ref, const std::string& name, const glm::mat3& value);

};