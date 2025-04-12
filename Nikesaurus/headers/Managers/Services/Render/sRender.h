/*****************************************************************//**
 * \file   sRender.h
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 *
 * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Managers/Services/Render/sShader.h"
#include "Components/cTransform.h"
#include "Components/cPhysics.h"
#include "Components/cRender.h"
#include "Managers/Services/Render/sParticle.h"
#include "Managers/Services/Render/sVideoPlayer.h"

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

			bool b_window_sized;

			void init(Vector2i const& size);

			//Default constructor
			FramebufferTexture() : frame_buffer{ 0 }, texture_color_buffer{ 0 }, width{ 1 }, height{ 1 }, b_window_sized{ false } {};
		};

		struct TextBuffer {
			unsigned int vao{}; // Vertex Array Object
			unsigned int vbo{}; // Vertex Buffer Object

			void init();

			//Default constructor
			TextBuffer() : vao{ 0 }, vbo{ 0 } {};
		};

		enum class FADE_STATE {
			NONE = 0,
			FADE_IN,
			FADE_OUT
		};

		class Service : public Events::IEventListener<Windows::WindowResized> {
			private:
				/* this fbo effects affect entire screen */
				unsigned int fbo{};
				unsigned int fbo_texture{};
				float fbo_opacity{1.f};

				FADE_STATE fade_state{ FADE_STATE::NONE };

				float fade_duration{};
				float fade_elapsed_time{};

				void fadeInHelper();
				void fadeOutHelper();

				/* end fbo effects */

				//Delete Copy Constructor & Copy Assignment
				Service(Service const& copy) = delete;
				void operator=(Service const& copy) = delete;

				//On windows resized event (Ensures game viewport's aspect ratio remains the same)
				void onEvent(std::shared_ptr<Windows::WindowResized> event) override;

				//Shader system
				std::unique_ptr<Shader::ShaderManager> shader_manager;

				//Particle system
				std::unique_ptr<SysParticle::Manager> particle_manager;

				//Video manager
				std::unique_ptr<VideoPlayer::Manager> video_manager;

				//Text buffer for rendering text
				TextBuffer text_buffer;

				//Hash counter
				std::atomic<unsigned int> counter;

				//Map of framebuffer
				std::unordered_map<unsigned int, FramebufferTexture> frame_buffers;

				//Queue of world render
				std::queue<std::function<void()>> world_render_queue;

				//Queue of world text render
				std::queue<std::function<void()>> world_text_render_queue;

				//Queue of world particle render
				std::queue<std::function<void()>> world_particle_render_queue;

				//Queue of screen render
				std::queue<std::function<void()>> screen_render_queue;

				//Queue of screen text render
				std::queue<std::function<void()>> screen_text_render_queue;

				//Queue of screen particle render
				std::queue<std::function<void()>> screen_particle_render_queue;

			public:

				unsigned int getFbo() const { return fbo; }

				//Rendering constants
				const bool BATCHED_RENDERING = true;
				static constexpr unsigned int MAX_INSTANCES = 500;
				static constexpr int MAX_UNIQUE_TEX_HDLS = 32;
				static constexpr int NUM_INDICES_FOR_QUAD = 6;
				static constexpr int NUM_VERTICES_IN_MODEL = 4;

				Service() = default;
				~Service() = default;

				/*****************************************************************//**
				* INITIALIZATION
				*********************************************************************/

				void init();

				/*****************************************************************//**
				* SHADERS
				*********************************************************************/

				//Bind shader
				void bindShader(std::string const& shader_ref);

				//Unbind shader
				void unbindShader();

				/*****************************************************************//**
				* FRAME BUFFERS
				*********************************************************************/

				//Create new frame buffer
				unsigned int createFrameBuffer(Vector2i const& size, bool b_window_sized = false);

				//Create new frame buffer
				void deleteFrameBuffer(unsigned int id);

				//Create new frame buffer
				FramebufferTexture getFrameBuffer(unsigned int id);

				//Bind frame buffer
				void bindFrameBuffer(unsigned int id);

				//Unbind frame buffer
				void unbindFrameBuffer();

				/*****************************************************************//**
				* TRANSFORM
				*********************************************************************/

				//Transform matrix
				void transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, const Vector2b& flip = { false, false });

				//Transform direction matrix 
				void transformDirectionMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat);

				/*****************************************************************//**
				* DRAW CALLS
				*********************************************************************/

				//Render Shape
				void renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape);

				//Render Texture
				void renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture);

				//Render Video
				void renderObject(Matrix_33 const& x_form, Render::Video const& e_video);

				//Render Bounding Box
				void renderBoundingBox(Matrix_33 const& x_form, Vector4f const& e_color);

				//Render text
				void renderText(Matrix_33 const& x_form, Render::Text& e_text);

				//Render Cursor
				void renderCursor(bool is_crosshair, bool cursor_entered);

				//Render particle system
				void renderParticleSystem(const NIKE::SysParticle::ParticleSystem& ps, bool use_screen_pos = false, const std::string& texture_ref = std::string{});

				//Render entity
				void renderComponents(std::unordered_map<std::string, std::shared_ptr<void>> comps, bool debug = false);

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

				// batch render bounding boxes
				void batchRenderBoundingBoxes();

				// batch render texture (uses renderObject)
				void batchRenderTextures();

				/*****************************************************************//**
				* RENDER COMPLETION CALL
				*********************************************************************/
				void completeRender();

				/*****************************************************************//**
				* SPECIAL EFFECTS
				*********************************************************************/

				void fadeIn(float duration);

				void fadeOut(float duration);

				float getFboOpacity() const;

				FADE_STATE getFadeState() const;
		};
	}
}

#endif // !RENDER_SERVICE_HPP
