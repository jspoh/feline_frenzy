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

#include "Components/cRender.h"
#include "Components/cTransform.h"
#include "Core/Engine.h"

namespace NIKE {
	namespace Render {

		struct RenderInstance {
			Matrix_33 xform{};
			bool to_blend_color = false;
			Vector4f color{};
			unsigned int tex{};
			Vector2f framesize{};
			Vector2f uv_offset{};
			float blend_intensity{};
		};

		//Render Manager
		class Manager : public System::ISystem, public Events::IEventListener<Windows::WindowResized> {
		private:

			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//On windows resized event
			void onEvent(std::shared_ptr<Windows::WindowResized> event) override;

			//Shader system
			std::unique_ptr<Shader::Manager> shader_system;

			//For rendering viewport
			unsigned int frame_buffer, texture_color_buffer;

			//Quad rendering variables
			unsigned int VAO, VBO;

			//Transform matrix
			void transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, const Vector2b& flip = {false, false});

			//Transform matrix debug
			void transformMatrixDebug(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, bool render_wireframe);

			//Render Shape
			void renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape);

			//
			std::vector<RenderInstance> render_instances_quad;

			// batch render shape
			void batchRenderObject();

			//Render Texture
			void renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture);

			std::vector<RenderInstance> render_instances_texture;

			// batch render texture
			void batchRenderTextures();

			//Render text
			void renderText(Matrix_33 const& x_form, Render::Text& e_text);

			//Render debugging wireframe
			void renderWireFrame(Matrix_33 const& x_form, Vector4f const& e_color);

			//Render viewport
			void renderViewport();

			//Helper function to encapsulate rendering entities
			void transformAndRenderEntity(Entity::Type entity, bool debugMode);

			//Helper function to encapsulate rendering text
			void transformAndRenderText(Entity::Type entity);
		public:
			constexpr static unsigned int MAX_INSTANCES = 1000;

			// hashset used to store unique texture handles in
			static std::unordered_set<unsigned int> curr_instance_unique_tex_hdls;
			static constexpr int MAX_UNIQUE_TEX_HDLS = 32;

			//Constructor
			Manager();

			//Destructor
			~Manager() = default;

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
