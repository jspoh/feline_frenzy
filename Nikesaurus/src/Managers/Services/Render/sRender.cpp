/*****************************************************************//**
 * \file   sRender.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Managers/Services/Render/sRender.h"

 // !TODO: jspoh reorg
namespace {
	Vector2f worldToScreen(const Vector2f& world_pos, bool use_world_pos) {
		// Get the active camera
		NIKE::Render::Cam cam = NIKE_CAMERA_SERVICE->getActiveCamera();

		// Get the appropriate world-to-NDC transformation matrix
		Matrix_33 world_to_ndc_xform = use_world_pos
			? NIKE_CAMERA_SERVICE->getWorldToNDCXform()
			: NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform();

		// Get screen dimensions
		const float screenWidth = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x);
		const float screenHeight = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);

		// Convert NDC (-1 to 1) to screen space (0 to width/height)
		Matrix_33 screen_xform{
			screenWidth * 0.5f, 0, screenWidth * 0.5f,
			0, screenHeight * 0.5f, screenHeight * 0.5f,
			0, 0, 1
		};

		// Final transformation
		Matrix_33 final_xform = screen_xform * world_to_ndc_xform;

		// Apply transformation
		return final_xform * world_pos;
	}

	float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}
}


namespace NIKE {

	void Render::Service::onEvent(std::shared_ptr<Windows::WindowResized> event) {
		
		// update main fbo
		{
			glDeleteFramebuffers(1, &fbo);
			glDeleteTextures(1, &fbo_texture);

			// create new framebuffer
			glCreateFramebuffers(1, &fbo);
			glCreateTextures(GL_TEXTURE_2D, 1, &fbo_texture);
			glTextureStorage2D(fbo_texture, 1, GL_RGBA8, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
			glTextureParameteri(fbo_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(fbo_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, fbo_texture, 0);
		}

		//Iterate through all frame buffers
		for (auto& frame_buffer : frame_buffers) {

			//Skip frame buffers that are not bound to the window size
			if (!frame_buffer.second.b_window_sized) continue;

			if (frame_buffer.second.width == event->frame_buffer.x &&
				frame_buffer.second.height == event->frame_buffer.y) {

				return; // Skip unnecessary recreation if size hasn't changed
			}
			frame_buffer.second.width = event->frame_buffer.x;
			frame_buffer.second.height = event->frame_buffer.y;

			GLenum err = glGetError();
			if (err != GL_NO_ERROR) {
				NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
			}

			// Cleanup old resources
			glDeleteFramebuffers(1, &frame_buffer.second.frame_buffer);
			glDeleteTextures(1, &frame_buffer.second.texture_color_buffer);

			// Create a new framebuffer
			glGenFramebuffers(1, &frame_buffer.second.frame_buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.second.frame_buffer);

			// Create a color attachment texture
			glGenTextures(1, &frame_buffer.second.texture_color_buffer);
			glBindTexture(GL_TEXTURE_2D, frame_buffer.second.texture_color_buffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, event->frame_buffer.x, event->frame_buffer.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.second.texture_color_buffer, 0);

			// Check if framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				NIKEE_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete! (Not an issue if triggered by focus loss)");

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			err = glGetError();
			if (err != GL_NO_ERROR) {
				NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
			}
		}

		event->setEventProcessed(true);
	}

	/*****************************************************************//**
	* INITIALIZATION
	*********************************************************************/

	void Render::FramebufferTexture::init(Vector2i const& size) {
		// Generate and bind the framebuffer
		glGenFramebuffers(1, &frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		// Create a color attachment texture
		glGenTextures(1, &texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

		// Specify the texture size
		if (b_window_sized) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		// Set texture parameters for filtering and wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Attach the texture to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NIKEE_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		// Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Render::TextBuffer::init() {
		//Create text buffer
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Render::Service::init() {

		if (BATCHED_RENDERING) {
			NIKEE_INFO("Using batched rendering");
		}
		else {
			NIKEE_INFO("Not using batched rendering");
		}

		// Reserved memory
		render_instances_quad.reserve(MAX_INSTANCES);
		render_instances_texture.reserve(MAX_INSTANCES);
		render_instances_bounding_box.reserve(MAX_INSTANCES);

		// Shader
		shader_manager = std::make_unique<Shader::ShaderManager>();

		//Particle manager
		particle_manager = std::make_unique<SysParticle::Manager>();

		//Video manager
		video_manager = std::make_unique<VideoPlayer::Manager>();

		if (!shader_manager) {
			NIKEE_CORE_ERROR("Failed to initialize Shader Manager.");
			return;
		}

		shader_manager->init();

		text_buffer.init();

		//Setup event listening for frame buffer resize
		std::shared_ptr<Render::Service> render_sys_wrapped(this, [](Render::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(render_sys_wrapped);

		//Start counter
		counter = 0;

		// framebuffer init
		{
			glCreateFramebuffers(1, &fbo);
			glCreateTextures(GL_TEXTURE_2D, 1, &fbo_texture);
			glTextureStorage2D(fbo_texture, 1, GL_RGBA8, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
			glTextureParameteri(fbo_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(fbo_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, fbo_texture, 0);
		}

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}

	}

	void Render::Service::bindShader(std::string const& shader_ref) {
		shader_manager->useShader(shader_ref);
	}

	void Render::Service::unbindShader() {
		shader_manager->unuseShader();
	}

	/*****************************************************************//**
	* FRAME BUFFERS
	*********************************************************************/

	unsigned int Render::Service::createFrameBuffer(Vector2i const& size, bool b_window_sized) {

		//Generate hash
		auto id = counter++;

		//Create and initialize new frame buffer
		frame_buffers[id].b_window_sized = b_window_sized;
		frame_buffers[id].init(size);

		//Return ID
		return id;
	}

	void Render::Service::deleteFrameBuffer(unsigned int id) {
		if (frame_buffers.find(id) == frame_buffers.end()) {
			NIKEE_CORE_WARN("Unable to delete frame buffer. ID not found");
			return;
		}

		//Delete frame buffer
		frame_buffers.erase(id);
	}

	Render::FramebufferTexture Render::Service::getFrameBuffer(unsigned int id) {
		if (frame_buffers.find(id) == frame_buffers.end()) {
			NIKEE_CORE_WARN("Unable to get frame buffer. ID not found");
			return Render::FramebufferTexture();
		}

		return frame_buffers.at(id);
	}

	void Render::Service::bindFrameBuffer(unsigned int id) {
		if (frame_buffers.find(id) == frame_buffers.end()) {
			NIKEE_CORE_WARN("Unable to bind frame buffer. ID not found");
			return;
		}

		//Bind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffers.at(id).frame_buffer);
	}

	void Render::Service::unbindFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/*****************************************************************//**
	* TRANSFORM
	*********************************************************************/

	void Render::Service::transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, const Vector2b& flip) {
		//Transform matrix here
		Matrix_33 result, scale_mat, rot_mat, trans_mat;

		const Matrix_33 FLIP_X_MAT = { -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		const Matrix_33 FLIP_Y_MAT = { 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

		Matrix_33RotDeg(rot_mat, obj.rotation);
		Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
		Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
		result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat
			* (flip.x ? FLIP_X_MAT : Matrix_33::Identity())
			* (flip.y ? FLIP_Y_MAT : Matrix_33::Identity());

		// OpenGL requires matrix in col maj so transpose
		Matrix_33Transpose(x_form, result);
	}

	void Render::Service::transformDirectionMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat) {
		//Transform matrix here
		Matrix_33 result, scale_mat, rot_mat, trans_mat, pre_trans_mat, post_trans_mat;

		// Rotate about a point
		Matrix_33RotDeg(rot_mat, obj.rotation);
		Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
		Matrix_33Translate(pre_trans_mat, -((obj.scale.x / 2.0f)), -((obj.scale.y / 2.0f)));
		Matrix_33Translate(post_trans_mat, ((obj.scale.x / 2.0f)), ((obj.scale.y / 2.0f)));
		Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
		result = world_to_ndc_mat * trans_mat * pre_trans_mat * rot_mat * post_trans_mat * scale_mat;

		// OpenGL requires matrix in col maj so transpose
		Matrix_33Transpose(x_form, result);
	}

	/*****************************************************************//**
	* DRAW CALLS
	*********************************************************************/

	void Render::Service::renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		constexpr std::array<const char*, 1> QUAD_SHAPE_MODELS = { "square.model" };


		if (!BATCHED_RENDERING || std::find(QUAD_SHAPE_MODELS.begin(), QUAD_SHAPE_MODELS.end(), e_shape.model_id) == QUAD_SHAPE_MODELS.end()) {

			//Set polygon mode
			// glPolygonMode(GL_FRONT, GL_FILL);
			glEnable(GL_BLEND);

			// use shader
			shader_manager->useShader("base");

			//Shader set uniform
			shader_manager->setUniform("base", "f_color", Vector3f(e_shape.color.r, e_shape.color.g, e_shape.color.b));
			shader_manager->setUniform("base", "f_opacity", e_shape.color.a);
			shader_manager->setUniform("base", "override_color", e_shape.use_override_color);
			shader_manager->setUniform("base", "model_to_ndc", x_form);

			//Get model
			auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(e_shape.model_id);

			//Draw
			glBindVertexArray(model->vaoid);
			glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

			//Unuse texture
			glBindVertexArray(0);
			shader_manager->unuseShader();
		}
		else {
			// prepare for batched rendering
			RenderInstance instance;
			instance.xform = x_form;
			instance.color = e_shape.color;

			render_instances_quad.push_back(instance);

			if (render_instances_quad.size() >= MAX_INSTANCES) {
				batchRenderObject();
			}
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Service::renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		//Caculate UV Offset
		const Vector2f framesize{ (1.0f / e_texture.frame_size.x) , (1.0f / e_texture.frame_size.y) };
		Vector2f uv_offset{ e_texture.frame_index.x * framesize.x, e_texture.frame_index.y * framesize.y };

		//Translate UV offset to bottom left
		uv_offset.y = std::abs(1 - uv_offset.y - framesize.y);

		const unsigned int tex_hdl = NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->gl_data;

		if (!BATCHED_RENDERING) {
			//Set polygon mode
			//glPolygonMode(GL_FRONT, GL_FILL);			// do not use this, 1280: invalid enum

			// use shader
			shader_manager->useShader("texture");

			//Texture unit
			static constexpr int texture_unit = 6;

			// set texture
			glBindTextureUnit(
				texture_unit, // texture unit (binding index)
				tex_hdl
			);

			glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//Set uniforms for texture rendering
			shader_manager->setUniform("texture", "u_tex2d", texture_unit);
			shader_manager->setUniform("texture", "u_opacity", e_texture.color.a);
			shader_manager->setUniform("texture", "u_transform", x_form);
			shader_manager->setUniform("texture", "uvOffset", uv_offset);
			shader_manager->setUniform("texture", "frameSize", framesize);

			//Blending options for texture
			shader_manager->setUniform("texture", "u_color", Vector3f(e_texture.color.r, e_texture.color.g, e_texture.color.b));
			shader_manager->setUniform("texture", "u_blend", e_texture.b_blend);
			shader_manager->setUniform("texture", "u_intensity", e_texture.intensity);

			//Flip texture options
			//shader_manager->setUniform("texture", "u_fliphorizontal", e_texture.b_flip.x);
			//shader_manager->setUniform("texture", "u_flipvertical", e_texture.b_flip.y);

			//Get model
			auto& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model");

			//Draw
			glBindVertexArray(model.vaoid);
			glDrawElements(model.primitive_type, model.draw_count, GL_UNSIGNED_INT, nullptr);

			//Unuse texture
			glBindVertexArray(0);
			shader_manager->unuseShader();
		}
		else {
			// prepare for batched rendering
			RenderInstance instance;
			instance.xform = x_form;
			instance.tex = tex_hdl;
			instance.framesize = framesize;
			instance.uv_offset = uv_offset;
			instance.to_blend_color = e_texture.b_blend;
			instance.color = e_texture.color;
			instance.blend_intensity = e_texture.intensity;

			render_instances_texture.push_back(instance);

			// used to track number of unique texture handles
			// system can only handle max 32 unique texture binding units, hence have to clear texture render instances or not all textures will render properly.
			curr_instance_unique_tex_hdls.insert(tex_hdl);

			if (render_instances_texture.size() >= MAX_INSTANCES || curr_instance_unique_tex_hdls.size() >= MAX_UNIQUE_TEX_HDLS) {
				batchRenderTextures();
			}
		}
		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Service::renderObject(Matrix_33 const& x_form, Render::Video const& e_video) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}
		// !TODO: batched rendering for texture incomplete

		//Render variables
		const Vector2f framesize{ 1.0f, 1.0f };
		Vector2f uv_offset{ 1.0f, 1.0f };
		Vector4f color = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool blend_mode = false;
		float intensity = 1.0f;

		const unsigned int tex_hdl = e_video.texture_id;

		if (!BATCHED_RENDERING) {
			//Set polygon mode
			//glPolygonMode(GL_FRONT, GL_FILL);			// do not use this, 1280: invalid enum

			// use shader
			shader_manager->useShader("texture");

			//Texture unit
			static constexpr int texture_unit = 6;

			// set texture
			glBindTextureUnit(
				texture_unit, // texture unit (binding index)
				tex_hdl
			);

			glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//Set uniforms for texture rendering
			shader_manager->setUniform("texture", "u_tex2d", texture_unit);
			shader_manager->setUniform("texture", "u_opacity", color.a);
			shader_manager->setUniform("texture", "u_transform", x_form);
			shader_manager->setUniform("texture", "uvOffset", uv_offset);
			shader_manager->setUniform("texture", "frameSize", framesize);

			//Blending options for texture
			shader_manager->setUniform("texture", "u_color", Vector3f(color.r, color.g, color.b));
			shader_manager->setUniform("texture", "u_blend", blend_mode);
			shader_manager->setUniform("texture", "u_intensity", intensity);

			//Flip texture options
			//shader_manager->setUniform("texture", "u_fliphorizontal", e_texture.b_flip.x);
			//shader_manager->setUniform("texture", "u_flipvertical", e_texture.b_flip.y);

			//Get model
			auto& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model");

			//Draw
			glBindVertexArray(model.vaoid);
			glDrawElements(model.primitive_type, model.draw_count, GL_UNSIGNED_INT, nullptr);

			//Unuse texture
			glBindVertexArray(0);
			shader_manager->unuseShader();
		}
		else {
			// prepare for batched rendering
			RenderInstance instance;
			instance.xform = x_form;
			instance.tex = tex_hdl;
			instance.framesize = framesize;
			instance.uv_offset = uv_offset;
			instance.to_blend_color = blend_mode;
			instance.color = color;
			instance.blend_intensity = intensity;

			render_instances_texture.push_back(instance);

			// used to track number of unique texture handles
			// system can only handle max 32 unique texture binding units, hence have to clear texture render instances or not all textures will render properly.
			curr_instance_unique_tex_hdls.insert(tex_hdl);

			if (render_instances_texture.size() >= MAX_INSTANCES || curr_instance_unique_tex_hdls.size() >= MAX_UNIQUE_TEX_HDLS) {
				batchRenderTextures();
			}
		}
		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Service::renderBoundingBox(Matrix_33 const& x_form, Vector4f const& e_color) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		if (!BATCHED_RENDERING) {

			// Set Polygon Mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// Use shader
			shader_manager->useShader("base");

			// Set shader uniforms
			shader_manager->setUniform("base", "f_color", Vector3f(e_color.r, e_color.g, e_color.b));
			shader_manager->setUniform("base", "f_opacity", e_color.a);
			shader_manager->setUniform("base", "model_to_ndc", x_form);

			// Get model
			auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square.model");

			// Draw model
			glBindVertexArray(model->vaoid);
			glDrawElements(GL_LINE_LOOP, model->draw_count, GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);

			// Unuse shader
			shader_manager->unuseShader();

			// Reset Polygon Mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			// Prepare for batched rendering
			RenderInstance instance;
			instance.xform = x_form;
			instance.color = e_color;

			render_instances_bounding_box.push_back(instance);

			if (render_instances_bounding_box.size() >= MAX_INSTANCES) {
				batchRenderBoundingBoxes();
			}
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Service::renderText(Matrix_33 const& x_form, Render::Text& e_text) {

		//Use text shader
		shader_manager->useShader("text");

		//Set shader values
		shader_manager->setUniform("text", "u_textColor", Vector3f(e_text.color.r, e_text.color.g, e_text.color.b));
		shader_manager->setUniform("text", "u_opacity", e_text.color.a);
		shader_manager->setUniform("text", "u_transform", x_form);

		//Set texture
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(text_buffer.vao);

		//Temp text size
		Vector2f text_size;

		//Calculate size of text
		for (char c : e_text.text) {
			Assets::Font::Character ch = NIKE_ASSETS_SERVICE->getAsset<Assets::Font>(e_text.font_id)->char_map[c];

			//Calculate width
			text_size.x += (ch.advance >> 6) * e_text.scale;

			//Calculate height
			text_size.y = ch.size.y * e_text.scale > text_size.y ? ch.size.y * e_text.scale : text_size.y;
		}

		//Assign size to e_text
		e_text.size = text_size;

		//Text rendering position based on bot left
		Vector2f pos;

		//Get text bottom left position for rendering
		switch (e_text.origin) {
		case TextOrigin::CENTER:
			pos = { -e_text.size.x / 2.0f, -e_text.size.y / 2.0f };
			break;
		case TextOrigin::TOP:
			pos = { -e_text.size.x / 2.0f, -e_text.size.y };
			break;
		case TextOrigin::BOTTOM:
			pos = { -e_text.size.x / 2.0f, 0.0f };
			break;
		case TextOrigin::RIGHT:
			pos = { -e_text.size.x, -e_text.size.y / 2.0f };
			break;
		case TextOrigin::LEFT:
			pos = { 0.0f, -e_text.size.y / 2.0f };
			break;
		default:
			break;
		}

		//Iterate through all characters
		for (char c : e_text.text)
		{
			Assets::Font::Character ch = NIKE_ASSETS_SERVICE->getAsset<Assets::Font>(e_text.font_id)->char_map[c];

			float xpos = pos.x + ch.bearing.x * e_text.scale;
			float ypos = pos.y - (ch.size.y - ch.bearing.y) * e_text.scale;

			float w = ch.size.x * e_text.scale;
			float h = ch.size.y * e_text.scale;

			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			//Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.texture);
			//Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, text_buffer.vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			//Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//Advance cursors for next glyph (note that advance is number of 1/64 pixels)
			pos.x += (ch.advance >> 6) * e_text.scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}

		//Unbind vertexs
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Render::Service::renderCursor(bool is_crosshair, bool cursor_entered) {
		//Render cursor 
		// FIX CURSOR IN FULLSCREEN, CURSOR OFFSET

#ifndef NDEBUG
		if (!NIKE_LVLEDITOR_SERVICE->getGameState())
			return;
#endif

		if (!cursor_entered) {
			NIKE_WINDOWS_SERVICE->getWindow()->setInputMode(NIKE_CURSOR, NIKE_CURSOR_NORMAL);
			return;
		}

		//Setup show cursor
		NIKE_WINDOWS_SERVICE->getWindow()->setInputMode(NIKE_CURSOR, NIKE_CURSOR_HIDDEN);

		auto texture_render = [&]() {
			static Render::Texture crosshair = { "crosshair.png", { 1.f, 1.f, 1.f, 1.f }, false, 1.f, false, Vector2i{ 1, 1 }, Vector2i{ 0, 0 }, Vector2b{false, false} };
			static Render::Texture cursor = { "cursor_highlighted.png", { 1.f, 1.f, 1.f, 1.f }, false, 1.f, false, Vector2i{ 1, 1 }, Vector2i{ 0, 0 }, Vector2b{false, false} };

			Matrix_33 cur_matrix;
			Transform::Transform cur_transform = { Vector2f(0.f, 0.f), Vector2f(50.0f, 50.0f), 0.0f };
			cur_transform.scale.x *= NIKE_CAMERA_SERVICE->getActiveCamera().zoom;
			cur_transform.scale.y *= NIKE_CAMERA_SERVICE->getActiveCamera().zoom;
			cur_transform.position.x = is_crosshair ? NIKE_INPUT_SERVICE->getMouseWorldPos().x : NIKE_INPUT_SERVICE->getMouseWorldPos().x + cur_transform.scale.x / 2;
			cur_transform.position.y = is_crosshair ? NIKE_INPUT_SERVICE->getMouseWorldPos().y : NIKE_INPUT_SERVICE->getMouseWorldPos().y - cur_transform.scale.y / 2;;
			cur_transform.use_screen_pos = true;

			// Render cursor
			transformMatrix(cur_transform, cur_matrix, NIKE_CAMERA_SERVICE->getWorldToNDCXform(false));
			renderObject(cur_matrix, is_crosshair ? crosshair : cursor);
			};

		screen_render_queue.push(texture_render);

	}

	void Render::Service::renderParticleSystem(const NIKE::SysParticle::ParticleSystem& ps, [[maybe_unused]] bool use_screen_pos, const std::string& texture_ref) {

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		int NUM_VERTICES = 6;		// defined in vertex shader
		GLenum PRIMITIVE = GL_TRIANGLES;

		std::string ref = NIKE::SysParticle::Data::particle_render_type_map.at(static_cast<NIKE::SysParticle::Data::ParticleRenderType>(ps.render_type));
		if (ps.preset == NIKE::SysParticle::Data::ParticlePresets::BASE) {
			ref = "base";
			NUM_VERTICES = 4;
			PRIMITIVE = GL_TRIANGLE_STRIP;
		}
		const std::string shader_name = ref + "_particle";


		shader_manager->useShader(shader_name);

		shader_manager->setUniform(shader_name, "iTime", (float)glfwGetTime());
		shader_manager->setUniform(shader_name, "iResolution", Vector2f{ NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize() }); // window size
		// Transform origin correctly based on screen position flag
		Vector2f particle_origin = worldToScreen(ps.origin, ps.using_world_pos);
		shader_manager->setUniform(shader_name, "particleScreenOrigin", particle_origin);

		if (static_cast<NIKE::SysParticle::Data::ParticleRenderType>(ps.render_type) == NIKE::SysParticle::Data::ParticleRenderType::TEXTURED) {
			// Check if asset exists
			if (NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(texture_ref) == nullptr) {
				return;
			}

			unsigned int tex_hdl;

			try {
				// using texture
				tex_hdl = NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(texture_ref)->gl_data;
			}
			catch (...) {
				NIKEE_CORE_ERROR("Error: Texture {0} not found in {1}: {2}", texture_ref, __FUNCTION__, "");
				return;
			}
			const int tex_binding_unit = 6;
			glBindTextureUnit(tex_binding_unit, tex_hdl);
			shader_manager->setUniform(shader_name, "u_tex2d", tex_binding_unit);
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL after setting uniform variables in {0}: {1}", __FUNCTION__, err);
		}

		//Particles to render (transform positions)
		// USE THIS PARTICLE BUFFER TO RENDER, NOT ps.particles
		auto particles = ps.particles;

		// transform each particle
		std::for_each(particles.begin(), particles.end(), [&](SysParticle::Particle& p) {
			p.pos = worldToScreen(p.pos, ps.using_world_pos);
			});


		//Bind buffer
		const unsigned int vbo = particle_manager->getVBO(ps.preset);
		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error before updating particle system vbo {0}: {1}", __FUNCTION__, err);
		}
		glNamedBufferSubData(vbo, 0, particles.size() * sizeof(SysParticle::Particle), particles.data());
		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error after updating particle system vbo {0}: {1}", __FUNCTION__, err);
		}

		const unsigned int vao = particle_manager->getVAO(static_cast<NIKE::SysParticle::Data::ParticlePresets>(ps.preset));
		glBindVertexArray(vao);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL after binding vao in {0}: {1}", __FUNCTION__, err);
		}

		const int draw_count = max(1, static_cast<int>(particles.size()));	// number of objects to draw, min 1

		glDrawArraysInstanced(PRIMITIVE, 0, NUM_VERTICES, draw_count);


		glBindVertexArray(0);
		shader_manager->unuseShader();

		while ((err = glGetError()) != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Service::renderComponents(std::unordered_map<std::string, std::shared_ptr<void>> comps, bool debug) {

		//Get transform
		auto trans_it = comps.find(Utility::convertTypeString(typeid(Transform::Transform).name()));
		if (trans_it == comps.end()) {
			return;
		}
		auto& e_transform = *std::static_pointer_cast<Transform::Transform>(trans_it->second);

		//Camera matrix
		Matrix_33 cam_ndcx = e_transform.use_screen_pos ? NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform() : NIKE_CAMERA_SERVICE->getWorldToNDCXform();
		//Get Texture
		{
			auto tex_it = comps.find(Utility::convertTypeString(typeid(Render::Texture).name()));
			if (tex_it != comps.end()) {

				//Texture component
				auto& e_texture = *std::static_pointer_cast<Render::Texture>(tex_it->second);

				//Check if texture is loaded
				if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_texture.texture_id)) {

					//Allow stretching of texture
					if (!e_texture.b_stretch) {
						//Copy transform for texture mapping ( Locks the transformation of a texture )
						Vector2f tex_size{
							static_cast<float>(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->size.x) / e_texture.frame_size.x,
							static_cast<float>(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->size.y) / e_texture.frame_size.y
						};

						e_transform.scale = tex_size.normalized() * e_transform.scale.length();
					}

					//Texture render function
					auto texture_render = [cam_ndcx, e_texture, e_transform]() {
						//Matrix used for rendering
						Matrix_33 matrix;

						// Transform matrix here
						NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx, Vector2b{ e_texture.b_flip.x, e_texture.b_flip.y });

						// Render Texture
						NIKE_RENDER_SERVICE->renderObject(matrix, e_texture);
						};

					//Check for screen position
					if (e_transform.use_screen_pos) {
						screen_render_queue.push(texture_render);
					}
					else {
						world_render_queue.push(texture_render);
					}
				}
			}
		}

		//Get Shape
		{
			auto shape_it = comps.find(Utility::convertTypeString(typeid(Render::Shape).name()));
			if (shape_it != comps.end()) {

				//Shape component
				auto& e_shape = *std::static_pointer_cast<Render::Shape>(shape_it->second);

				//Check if model exists
				if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {

					//Shape render function
					auto shape_render = [e_shape, e_transform, cam_ndcx]() {
						//Matrix used for rendering
						Matrix_33 matrix;

						// Transform matrix here
						NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx);

						//Render Shape
						NIKE_RENDER_SERVICE->renderObject(matrix, e_shape);
						};

					//Check for screen position
					if (e_transform.use_screen_pos) {
						screen_render_queue.push(shape_render);
					}
					else {
						world_render_queue.push(shape_render);
					}
				}
			}
		}

		//Debug mode rendering
		{
			if (debug) {
				// Render debugging bounding box
				Vector4f bounding_box_color{ 1.0f, 0.0f, 0.0f, 1.0f };

				//Get Collider
				auto collider_it = comps.find(Utility::convertTypeString(typeid(Physics::Collider).name()));
				if (collider_it != comps.end()) {

					//Collider comp
					auto& e_collider = *std::static_pointer_cast<Physics::Collider>(collider_it->second);

					//Change color of bounding box on collision
					if (e_collider.b_collided) {
						bounding_box_color = { 0.0f, 1.0f, 0.0f, 1.0f };
					}

					//Shape render function
					auto collider_render = [e_collider, bounding_box_color, cam_ndcx]() {
						//Matrix used for rendering
						Matrix_33 matrix;

						//Calculate bounding box matrix
						NIKE_RENDER_SERVICE->transformMatrix(e_collider.transform, matrix, cam_ndcx);
						NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
						};

					//Check for screen position
					if (e_transform.use_screen_pos) {
						screen_render_queue.push(collider_render);
					}
					else {
						world_render_queue.push(collider_render);
					}
				}
				else {

					//Shape render function
					auto collider_render = [bounding_box_color, e_transform, cam_ndcx]() {
						//Matrix used for rendering
						Matrix_33 matrix;

						//Calculate bounding box matrix
						NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx);
						NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
						};

					//Check for screen position
					if (e_transform.use_screen_pos) {
						screen_render_queue.push(collider_render);
					}
					else {
						world_render_queue.push(collider_render);
					}
				}

				//Get Dynamics
				auto dynamics_it = comps.find(Utility::convertTypeString(typeid(Physics::Dynamics).name()));
				if (dynamics_it != comps.end()) {

					//Collider comp
					auto& e_dynamics = *std::static_pointer_cast<Physics::Dynamics>(dynamics_it->second);

					if (e_dynamics.velocity.x != 0.0f || e_dynamics.velocity.y != 0.0f) {

						//Shape render function
						auto dir_render = [e_transform, e_dynamics, cam_ndcx, bounding_box_color]() {
							//Matrix used for rendering
							Matrix_33 matrix;

							Transform::Transform dir_transform = e_transform;
							dir_transform.scale.x = 1.0f;
							dir_transform.rotation = -atan2(e_dynamics.velocity.x, e_dynamics.velocity.y) * static_cast<float>(180.0f / M_PI);
							dir_transform.position += {0.0f, e_transform.scale.y / 2.0f};
							NIKE_RENDER_SERVICE->transformDirectionMatrix(dir_transform, matrix, cam_ndcx);
							NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
							};

						//Check for screen position
						if (e_transform.use_screen_pos) {
							screen_render_queue.push(dir_render);
						}
						else {
							world_render_queue.push(dir_render);
						}
					}
				}
			}
		}

		//Get Text
		{
			auto text_it = comps.find(Utility::convertTypeString(typeid(Render::Text).name()));
			if (text_it != comps.end()) {

				//Text component
				auto& e_text = *std::static_pointer_cast<Render::Text>(text_it->second);

				//Check if font exists
				if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_text.font_id)) {

					//Text render function
					auto text_render = [e_transform, cam_ndcx, &e_text]() {

						//Transform matrix
						Matrix_33 matrix;

						//Make copy of transform, scale to 1.0f for calculating matrix
						Transform::Transform copy = e_transform;
						copy.scale = { 1.0f, 1.0f };

						//Transform text matrix
						NIKE_RENDER_SERVICE->transformMatrix(copy, matrix, cam_ndcx);

						//Render text
						NIKE_RENDER_SERVICE->renderText(matrix, e_text);
						};

					//Check for screen position
					if (e_transform.use_screen_pos) {
						screen_text_render_queue.push(text_render);
					}
					else {
						world_text_render_queue.push(text_render);
					}
				}
			}
		}

		//Get particle emitter
		{
			auto particle_it = comps.find(Utility::convertTypeString(typeid(Render::ParticleEmitter).name()));
			if (particle_it != comps.end()) {

				//Particle component
				auto& e_particle = *std::static_pointer_cast<Render::ParticleEmitter>(particle_it->second);

				//Get particle system
				auto& particle_sys = *e_particle.p_system;

				//Update particle system with updated particle emitter data
				particle_sys.preset = static_cast<SysParticle::Data::ParticlePresets>(e_particle.preset);
				particle_sys.origin = e_transform.position + e_particle.offset;
				particle_sys.duration = e_particle.duration;
				particle_sys.render_type = static_cast<SysParticle::Data::ParticleRenderType>(e_particle.render_type);

				particle_sys.using_world_pos = !e_transform.use_screen_pos;

				particle_sys.num_new_particles_per_second = e_particle.num_new_particles_per_second;
				particle_sys.particle_lifespan = e_particle.particle_lifespan;
				particle_sys.particle_acceleration = e_particle.particle_acceleration;
				particle_sys.particle_velocity_range = e_particle.particle_velocity_range;
				particle_sys.particle_vector_x_range = e_particle.particle_vector_x_range;
				particle_sys.particle_vector_y_range = e_particle.particle_vector_y_range;
				particle_sys.particle_color_is_random = e_particle.particle_color_is_random;
				particle_sys.particle_color = e_particle.particle_color;
				particle_sys.particle_rand_x_offset_range = e_particle.particle_rand_x_offset_range;
				particle_sys.particle_rand_y_offset_range = e_particle.particle_rand_y_offset_range;
				particle_sys.particle_rotation = e_particle.particle_rotation;
				particle_sys.particle_rand_width_range = e_particle.particle_rand_width_range;
				particle_sys.particle_size_changes_over_time = e_particle.particle_size_changes_over_time;
				particle_sys.particle_final_size = e_particle.particle_final_size;
				particle_sys.particle_color_changes_over_time = e_particle.particle_color_changes_over_time;
				particle_sys.particle_final_color = e_particle.particle_final_color;
				particle_sys.particle_rotation_speed = e_particle.particle_rotation_speed;
				particle_sys.texture_ref = e_particle.texture_ref;

				//Update particle system
				particle_manager->updateParticleSystem(particle_sys);

				//Particle render function
				auto particle_render = [&particle_sys, ref = e_particle.ref]() {

					NIKE_RENDER_SERVICE->renderParticleSystem(particle_sys, ref == "mouseps1", particle_sys.texture_ref);
					};

				//Check for screen position !!!More work to be done here to ensure screen particles are rendered correctly
				if (e_transform.use_screen_pos) {
					screen_particle_render_queue.push(particle_render);
				}
				else {
					world_particle_render_queue.push(particle_render);
				}
			}
		}

		//Get video
		{
			auto video_it = comps.find(Utility::convertTypeString(typeid(Render::Video).name()));
			if (video_it != comps.end()) {

				//Video component
				auto& e_video = *std::static_pointer_cast<Render::Video>(video_it->second);

				//Check if video exists
				if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_video.video_id)) {

					//Update video player
					video_manager->update(e_video);

					//Check for valid mpeg
					if (e_video.mpeg) {

						//Clamp aspect ratio of texture
						e_transform.scale = e_video.texture_size.normalized() * e_transform.scale.length();

						//Video render function
						auto video_render = [e_transform, cam_ndcx, &e_video]() {

							//Transform matrix
							Matrix_33 matrix;

							//Transform video matrix
							NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx);

							//Render video
							NIKE_RENDER_SERVICE->renderObject(matrix, e_video);
							};

						//Check for screen position
						if (e_transform.use_screen_pos) {
							screen_render_queue.push(video_render);
						}
						else {
							world_render_queue.push(video_render);
						}
					}
				}
			}
		}
	}

	/*****************************************************************//**
	* BATCH RENDERING
	*********************************************************************/

	std::unordered_set<unsigned int> Render::Service::curr_instance_unique_tex_hdls{};

	void Render::Service::batchRenderObject() {

		if (!BATCHED_RENDERING || render_instances_quad.empty()) {
			return;
		}

		Assets::Model& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("batched_square.model");

		// create buffer of vertices
		std::vector<Assets::Vertex> vertices;

		vertices.reserve(render_instances_quad.size() * NUM_VERTICES_IN_MODEL);

		for (size_t i{}; i < render_instances_quad.size(); i++) {
			//Modify the vertices directly without copying the model repeatedly
			for (Assets::Vertex& v : model.vertices) {
				Assets::Vertex temp = v; // Work on a temporary copy
				temp.col = render_instances_quad[i].color;
				temp.transform = render_instances_quad[i].xform;
				vertices.push_back(temp);
			}

			// create temp model to populate with current instance's data
			//Assets::Model m{ model };
			//for (Assets::Vertex& v : m.vertices) {
			//	v.col = render_instances_quad[i].color;
			//	v.transform = render_instances_quad[i].xform;
			//}
			//vertices.insert(vertices.end(), m.vertices.begin(), m.vertices.end());
		}

		// Update the vertex buffer object (VBO)
		glNamedBufferSubData(model.vboid, 0, vertices.size() * sizeof(Assets::Vertex), vertices.data());

		// Create buffer of indices for indexed rendering
		std::vector<unsigned int> indices;

		indices.reserve(render_instances_quad.size() * NUM_INDICES_FOR_QUAD); // Adjust based on expected batch size

		// 0 1 2 2 3 0 -> 4 5 6 6 7 4
		for (size_t i{}; i < render_instances_quad.size(); i++) {
			unsigned int baseOffset = static_cast<unsigned int>(i * NUM_VERTICES_IN_MODEL);
			for (unsigned int idx : model.indices) {
				indices.push_back(idx + baseOffset);
			}
			//for (size_t j{}; j < model.indices.size(); j++) {
			//	indices.push_back(model.indices[j] + baseOffset);
			//	
			//}
		}

		// Update the element buffer object (EBO)
		glNamedBufferSubData(model.eboid, 0, indices.size() * sizeof(unsigned int), indices.data());

		// use shader
		shader_manager->useShader("batched_base");

		// bind vao
		glBindVertexArray(model.vaoid);

		// Draw all instances using instanced rendering
		glDrawElementsInstanced(model.primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(render_instances_quad.size()));

		// cleanup
		glBindVertexArray(0);
		shader_manager->unuseShader();
		render_instances_quad.clear();
	}

	void Render::Service::batchRenderBoundingBoxes() {
		if (!BATCHED_RENDERING || render_instances_bounding_box.empty()) {
			return;
		}

		Assets::Model& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("batched_square.model");

		// Create a buffer for vertices
		std::vector<Assets::Vertex> vertices;
		vertices.reserve(render_instances_bounding_box.size() * NUM_VERTICES_IN_MODEL);

		for (const auto& instance : render_instances_bounding_box) {
			for (Assets::Vertex v : model.vertices) {
				// Modify a temporary vertex with the instance data
				v.col = instance.color;
				v.transform = instance.xform;
				vertices.push_back(v);
			}
		}

		// Update the vertex buffer object (VBO)
		glNamedBufferSubData(model.vboid, 0, vertices.size() * sizeof(Assets::Vertex), vertices.data());

		// Create a buffer for indices
		std::vector<unsigned int> indices;
		indices.reserve(render_instances_bounding_box.size() * 8); // 8 indices per bounding box

		for (size_t i = 0; i < render_instances_bounding_box.size(); ++i) {
			unsigned int baseOffset = static_cast<unsigned int>(i * 4); // 4 vertices per bounding box

			// Define edges manually for GL_LINES
			indices.push_back(baseOffset + 0); indices.push_back(baseOffset + 1); // Bottom edge
			indices.push_back(baseOffset + 1); indices.push_back(baseOffset + 2); // Right edge
			indices.push_back(baseOffset + 2); indices.push_back(baseOffset + 3); // Top edge
			indices.push_back(baseOffset + 3); indices.push_back(baseOffset + 0); // Left edge
		}

		// Update the element buffer object (EBO)
		glNamedBufferSubData(model.eboid, 0, indices.size() * sizeof(unsigned int), indices.data());

		// Use the wireframe shader
		shader_manager->useShader("batched_base");

		// Set polygon mode for wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Bind the VAO
		glBindVertexArray(model.vaoid);

		// Draw all bounding box instances
		glDrawElements(GL_LINES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

		// Cleanup
		glBindVertexArray(0);
		shader_manager->unuseShader();

		// Reset polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Clear the batch
		glBindVertexArray(0);
		shader_manager->unuseShader();
		render_instances_bounding_box.clear();
	}

	void Render::Service::batchRenderTextures() {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		if (!BATCHED_RENDERING || render_instances_texture.empty()) {
			return;
		}

		Assets::Model& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("batched_texture.model");

		// create vector of texture handles
		// map with texture handle as key and binding unit as value
		// not using unordered_map as it uses hashingand i need the index
		std::map<unsigned int, unsigned int> texture_binding_unit_map;

		// Bind textures and build the map
		for (const auto& instance : render_instances_texture) {
			if (texture_binding_unit_map.find(instance.tex) == texture_binding_unit_map.end()) {
				// binding unit for this texture does not exist yet

				// get binding unit. size of texture_binding_unit_map is the next available binding unit(size changes during this loop)
				// cant use `i` as the max texture unit (at least on my system) is 32
				const unsigned int binding_unit = static_cast<unsigned int>(texture_binding_unit_map.size());
				const unsigned int tex_hdl = instance.tex;

				// bind texture to binding unit
				glBindTextureUnit(static_cast<unsigned int>(binding_unit), tex_hdl);

				// store binding unit in map
				texture_binding_unit_map[tex_hdl] = binding_unit;

				// set texture parameters
				glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(tex_hdl, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
		}

		// raw vector of binding units
		std::vector<unsigned int> texture_binding_units;
		std::transform(texture_binding_unit_map.begin(), texture_binding_unit_map.end(), std::back_inserter(texture_binding_units), [](const std::pair<unsigned int, unsigned int>& pair) { return pair.second; });

		// create buffer of vertices
		std::vector<Assets::Vertex> vertices;
		vertices.reserve(render_instances_quad.size() * NUM_VERTICES_IN_MODEL);

		// Populate vertices
		for (const auto& instance : render_instances_texture) {
			// create temp model to populate with current instance's data
			for (const Assets::Vertex& v : model.vertices) {
				Assets::Vertex temp = v; // Work on a temporary copy
				temp.transform = instance.xform;
				temp.framesize = instance.framesize;
				temp.uv_offset = instance.uv_offset;
				temp.to_blend_color = instance.to_blend_color ? 1.0f : 0.0f;

				if (temp.to_blend_color) {
					temp.col = instance.color;
					temp.blend_intensity = instance.blend_intensity;
				}

				// get index of texture hdl in texture_binding_unit_map vector
				auto it = texture_binding_unit_map.find(instance.tex);
				if (it == texture_binding_unit_map.end()) {
					NIKEE_CORE_ERROR("Texture handle not found in texture_binding_unit_map");
					throw std::runtime_error("Invalid texture handle");
				}

				temp.sampler_idx = static_cast<float>(std::distance(texture_binding_unit_map.begin(), it));
				vertices.push_back(temp);
			}
		}

		// populate vbo
		glNamedBufferSubData(model.vboid, 0, vertices.size() * sizeof(Assets::Vertex), vertices.data());

		// create buffer of indices for indexed rendering
		std::vector<unsigned int> indices;
		indices.reserve(render_instances_quad.size() * NUM_INDICES_FOR_QUAD);

		// 0 1 2 2 3 0 -> 4 5 6 6 7 4
		for (size_t i = 0; i < render_instances_texture.size(); i++) {
			unsigned int baseOffset = static_cast<unsigned int>(i * NUM_VERTICES_IN_MODEL);
			for (unsigned int idx : model.indices) {
				indices.push_back(idx + baseOffset);
			}
		}

		// populate ebo
		glNamedBufferSubData(model.eboid, 0, indices.size() * sizeof(unsigned int), indices.data());

		// use shader
		shader_manager->useShader("batched_texture");

		// set uniform
		shader_manager->setUniform("batched_texture", "u_tex2d", texture_binding_units);

		// bind vao
		glBindVertexArray(model.vaoid);

		// Draw all instances using instanced rendering
		glDrawElementsInstanced(model.primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(render_instances_texture.size()));

		// check framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		//if (status != GL_FRAMEBUFFER_COMPLETE) {
		//	NIKEE_CORE_ERROR("Incomplete framebuffer in {0} with status: {1}", __FUNCTION__, status);
		//}

		// cleanup
		glBindVertexArray(0);
		shader_manager->unuseShader();
		render_instances_texture.clear();

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of batchRenderTextures: {0}", err);
		}
	}

	/*****************************************************************//**
	* RENDER COMPLETION CALL
	*********************************************************************/
	void Render::Service::completeRender() {

		//Update particle manager
		//particle_manager->update();

		//Render world elements
		while (!world_render_queue.empty()) {
			world_render_queue.front()();
			world_render_queue.pop();
		}

		//Batch render
		if (NIKE_RENDER_SERVICE->BATCHED_RENDERING) {
			NIKE_RENDER_SERVICE->batchRenderTextures();
			NIKE_RENDER_SERVICE->batchRenderObject();
			NIKE_RENDER_SERVICE->batchRenderBoundingBoxes();
		}

		//Render world text elements
		while (!world_text_render_queue.empty()) {
			world_text_render_queue.front()();
			world_text_render_queue.pop();
		}

		//Render world particle elements
		while (!world_particle_render_queue.empty()) {
			world_particle_render_queue.front()();
			world_particle_render_queue.pop();
		}

		//Render screen elements
		while (!screen_render_queue.empty()) {
			screen_render_queue.front()();
			screen_render_queue.pop();
		}

		//Batch render
		if (NIKE_RENDER_SERVICE->BATCHED_RENDERING) {
			NIKE_RENDER_SERVICE->batchRenderTextures();
			NIKE_RENDER_SERVICE->batchRenderObject();
			NIKE_RENDER_SERVICE->batchRenderBoundingBoxes();
		}

		//Render screen text elements
		while (!screen_text_render_queue.empty()) {
			screen_text_render_queue.front()();
			screen_text_render_queue.pop();
		}

		//Render screen particle elements
		while (!screen_particle_render_queue.empty()) {
			screen_particle_render_queue.front()();
			screen_particle_render_queue.pop();
		}

		// render from fbo

		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			const Vector2f framesize{ 1.0f , 1.0f };
			Vector2f uv_offset{ 0, 0 };

			shader_manager->useShader("texture");

			//Texture unit
			static constexpr int texture_unit = 6;

			// set texture
			glBindTextureUnit(
				texture_unit, // texture unit (binding index)
				fbo_texture
			);

			glTextureParameteri(fbo_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(fbo_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//Set uniforms for texture rendering
			shader_manager->setUniform("texture", "u_tex2d", texture_unit);
			shader_manager->setUniform("texture", "u_opacity", fbo_opacity);
			shader_manager->setUniform("texture", "u_transform", 2 * Matrix_33::Identity());
			shader_manager->setUniform("texture", "uvOffset", uv_offset);
			shader_manager->setUniform("texture", "frameSize", framesize);

			//Blending options for texture
			shader_manager->setUniform("texture", "u_color", Vector3f(1.f, 1.f, 1.f));
			shader_manager->setUniform("texture", "u_blend", true);
			shader_manager->setUniform("texture", "u_intensity", 0.f);

			//Flip texture options
			//shader_manager->setUniform("texture", "u_fliphorizontal", e_texture.b_flip.x);
			//shader_manager->setUniform("texture", "u_flipvertical", e_texture.b_flip.y);

			//Get model
			auto& model = *NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model");

			//Draw
			glClear(GL_COLOR_BUFFER_BIT);
			glBindVertexArray(model.vaoid);
			glDrawElements(model.primitive_type, model.draw_count, GL_UNSIGNED_INT, nullptr);

			//Unuse texture
			glBindVertexArray(0);
			shader_manager->unuseShader();
		}

		 // fbo update
		{
			switch (fade_state) {
			case FADE_STATE::FADE_IN:
				fadeInHelper();
				break;
			case FADE_STATE::FADE_OUT:
				fadeOutHelper();
				break;
			}
		}
	}

	void Render::Service::fadeIn(float duration) {
		fade_duration = duration;
		fade_elapsed_time = 0.f;
		fbo_opacity = 0.f;
		fade_state = FADE_STATE::FADE_IN;
	}

	void Render::Service::fadeOut(float duration) {
		fade_duration = duration;
		fade_elapsed_time = 0.f;
		fbo_opacity = 1.f;
		fade_state = FADE_STATE::FADE_OUT;
	}

	void Render::Service::fadeInHelper() {
		if (fade_elapsed_time >= fade_duration) {
			fbo_opacity = 1.f;
			fade_state = FADE_STATE::NONE;
			return;
		}

		fade_elapsed_time += NIKE_WINDOWS_SERVICE->getDeltaTime();

		static constexpr float start_opacity = 0.f;
		static constexpr float end_opacity = 1.f;

		fbo_opacity = lerp(start_opacity, end_opacity, fade_elapsed_time / fade_duration);
	}

	void Render::Service::fadeOutHelper() {
		if (fade_elapsed_time >= fade_duration) {
			fbo_opacity = 0.f;
			fade_state = FADE_STATE::NONE;
			return;
		}
		fade_elapsed_time += NIKE_WINDOWS_SERVICE->getDeltaTime();
		static constexpr float start_opacity = 1.f;
		static constexpr float end_opacity = 0.f;
		fbo_opacity = lerp(start_opacity, end_opacity, fade_elapsed_time / fade_duration);
	}

	float Render::Service::getFboOpacity() const {
		return fbo_opacity;
	}

	Render::FADE_STATE Render::Service::getFadeState() const {
		return fade_state;
	}
}