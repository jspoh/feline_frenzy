/*****************************************************************//**
 * \file   sysRender.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * 
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include "Managers/ECS/mSystem.h"

#include "Systems/Render/sysShader.h"
#include "Systems/Render/sysCamera.h"

#include "Components/cRender.h"
#include "Components/cTransform.h"
#include "Core/Engine.h"

namespace NIKE {
	namespace Render {

		//Render Manager
		class Manager : public System::ISystem {
		private:

			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Shader system
			std::unique_ptr<Shader::Manager> shader_system;

			//Camera System
			std::unique_ptr<Camera::System> camera_system;

			//Transform matrix
			void transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat);

			//Transform matrix debug
			void transformMatrixDebug(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, bool render_wireframe);

			//Render Shape
			void renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape);

			//Render Texture
			void renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture);

			//Render text
			void renderText(Render::Text const& e_text);

			//Render debugging wireframe
			void renderWireFrame(Matrix_33 const& x_form, Render::Color const& e_color);

			//Helper function to encapsulate rendering
			void transformAndRenderEntity(Entity::Type entity, bool debugMode);
		public:
			//Constructor
			Manager() = default;

			//Destructor
			~Manager() = default;

			static GLuint framebuffer, textureColorbuffer;

			std::string getSysName()
			{
				return "Render System";
			}

			/**
			* update all object's xform
			* */
			void init() override;

			/**
			* update all object's xform
			* */
			void update() override;
		};
	}
}

#endif // !RENDER_MANAGER_HPP
