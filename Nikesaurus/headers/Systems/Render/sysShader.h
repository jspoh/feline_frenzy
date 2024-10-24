/*****************************************************************//**
 * \file   sysShader.h
 * \brief
 *
 * \author Sean Gwee, g.boonxuensean@digipen.edu (90%)
 * \co-author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (10%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

namespace NIKESAURUS {
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
			void setUniform(const std::string& shader_ref, const std::string& name, const Matrix_33& value);
			void setUniform(const std::string& shader_ref, const std::string& name, const Vector3f& value);

			//Sprite sheet
			void setUniform(const std::string& shader_ref, const std::string& name, const Vector2f& value);
		};
	}
}

#endif // !SHADER_MANAGER_HPP