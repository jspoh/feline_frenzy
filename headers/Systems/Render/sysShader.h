/*****************************************************************//**
 * \file   sysShader.h
 * \brief
 *
 * \author g.boonxuensean@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

namespace Shader {
	class Manager {
	private:
		/**
		 * compiles shader and adds to shader_programs.
		 *
		 * \param shader_ref	shader program's reference string
		 * \param vtx_path		path to vertex shader
		 * \param frag_path		path to fragment shader
		 */
		unsigned int compileShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);

	public:
		Manager() = default;
		~Manager() = default;

		unsigned int loadShader(const std::string& shader_ref, const std::string& vtx_path, const std::string& frag_path);
		void useShader(const std::string& shader_ref);
		/**
		* Unbind the active shader program.
		*/
		void unuseShader();

		// Uniform setter overload functions
		void setUniform(const std::string& shader_ref, const std::string& name, int value);
		void setUniform(const std::string& shader_ref, const std::string& name, float value);
		void setUniform(const std::string& shader_ref, const std::string& name, const Matrix33::Matrix_33& value);
		void setUniform(const std::string& shader_ref, const std::string& name, const Vector3& value);
	};
}

#endif // !SHADER_MANAGER_HPP