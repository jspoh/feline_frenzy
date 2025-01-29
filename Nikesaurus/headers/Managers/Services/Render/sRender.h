/*****************************************************************//**
 * \file   sRender.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 *
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Managers/Services/Render/sShader.h"
#include "Components/cTransform.h"
#include "Components/cPhysics.h"
#include "Components/cRender.h"

#ifndef RENDER_SERVICE_HPP
#define RENDER_SERVICE_HPP

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

		struct FramebufferTexture {
			unsigned int frame_buffer{};
			unsigned int texture_color_buffer{};

			int width{};
			int height{};
		};

		struct TextBuffer {
			unsigned int vao{}; // Vertex Array Object
			unsigned int vbo{}; // Vertex Buffer Object
		};

		class Service : public Events::IEventListener<Windows::WindowResized> {
			private:

				//Delete Copy Constructor & Copy Assignment
				Service(Service const& copy) = delete;
				void operator=(Service const& copy) = delete;

				//On windows resized event (Ensures game viewport's aspect ratio remains the same)
				void onEvent(std::shared_ptr<Windows::WindowResized> event) override;

				//Shader system
				std::unique_ptr<Shader::ShaderManager> shader_manager;
				TextBuffer text_buffer;

			public:
				const bool BATCHED_RENDERING = true;
				static constexpr unsigned int MAX_INSTANCES = 500;
				static constexpr int MAX_UNIQUE_TEX_HDLS = 32;
				static constexpr int NUM_INDICES_FOR_QUAD = 6;
				static constexpr int NUM_VERTICES_IN_MODEL = 4;

				Service() = default;
				~Service() = default;
				
				FramebufferTexture framebuffer_tex;

				/*****************************************************************//**
				* INITIALIZATION
				*********************************************************************/

				void init();

				/*****************************************************************//**
				* TRANSFORM
				*********************************************************************/

				//Transform matrix
				void transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, const Vector2b& flip = { false, false });

				//Transform matrix debug
				void transformMatrixDebug(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, bool render_wireframe);

				/*****************************************************************//**
				* DRAW CALLS
				*********************************************************************/

				//Render Shape
				void renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape);

				//Render Texture
				void renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture);

				//Render Bounding Box
				void renderBoundingBox(Matrix_33 const& x_form, Vector4f const& e_color);

				//Render text
				void renderText(Matrix_33 const& x_form, Render::Text& e_text);

				/*****************************************************************//**
				* BATCH RENDERING
				*********************************************************************/

				// Stores unique texture handles for the current batch
				static std::unordered_set<unsigned int> curr_instance_unique_tex_hdls;

				// Shapes render instance
				std::vector<RenderInstance> render_instances_quad;

				// Textures render instance
				std::vector<RenderInstance> render_instances_texture;

				// Bounding box render instance
				std::vector<RenderInstance> render_instances_bounding_box;

				// batch render shape (uses renderObject)
				void batchRenderObject();

				void batchRenderBoundingBoxes();

				// batch render texture (uses renderObject)
				void batchRenderTextures();
		};
	}
}

#endif // !RENDER_SERVICE_HPP
