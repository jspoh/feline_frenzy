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
#include "Systems/sysParticle.h"

namespace NIKE {

	void Render::Service::onEvent(std::shared_ptr<Windows::WindowResized> event) {

		if (framebuffer_tex.width == event->frame_buffer.x &&
			framebuffer_tex.height == event->frame_buffer.y) {

			return; // Skip unnecessary recreation if size hasn't changed
		}
		framebuffer_tex.width = event->frame_buffer.x;
		framebuffer_tex.height = event->frame_buffer.y;

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

		// Cleanup old resources
		glDeleteFramebuffers(1, &framebuffer_tex.frame_buffer);
		glDeleteTextures(1, &framebuffer_tex.texture_color_buffer);

		// Create a new framebuffer
		glGenFramebuffers(1, &framebuffer_tex.frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_tex.frame_buffer);

		// Create a color attachment texture
		glGenTextures(1, &framebuffer_tex.texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer_tex.texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, event->frame_buffer.x, event->frame_buffer.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex.texture_color_buffer, 0);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NIKEE_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete! (Not an issue if triggered by focus loss)");

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}

		event->setEventProcessed(true);
	}

	/*****************************************************************//**
	* INITIALIZATION
	*********************************************************************/

	void Render::FramebufferTexture::init() {
		// Generate and bind the framebuffer
		glGenFramebuffers(1, &frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		// Create a color attachment texture
		glGenTextures(1, &texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

		// Specify the texture size
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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

		if (!shader_manager) {
			NIKEE_CORE_ERROR("Failed to initialize Shader Manager.");
			return;
		}

		shader_manager->init();

		framebuffer_tex.init();

		text_buffer.init();

		//Setup event listening for frame buffer resize
		std::shared_ptr<Render::Service> render_sys_wrapped(this, [](Render::Service*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(render_sys_wrapped);
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

	void Render::Service::transformMatrixDebug(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, bool render_wireframe) {
		//Transform matrix here
		Matrix_33 result, scale_mat, rot_mat, trans_mat, pre_trans_mat, post_trans_mat;

		if (!render_wireframe) {
			Matrix_33RotDeg(rot_mat, obj.rotation);
			Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
			Matrix_33Translate(pre_trans_mat, -((obj.scale.x / 2.0f)), -((obj.scale.y / 2.0f)));
			Matrix_33Translate(post_trans_mat, ((obj.scale.x / 2.0f)), ((obj.scale.y / 2.0f)));
			Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
			result = world_to_ndc_mat * trans_mat * pre_trans_mat * rot_mat * post_trans_mat * scale_mat;
		}
		else {
			Matrix_33RotDeg(rot_mat, obj.rotation);
			Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
			Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
			result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;
		}

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
		// !TODO: batched rendering for texture incomplete

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

	void Render::Service::renderParticleSystem(int preset, const Vector2f& origin, unsigned int vao, int draw_count) { 
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		const std::string ref = NIKE::SysParticle::Data::particle_preset_map.at(static_cast<NIKE::SysParticle::Data::ParticlePresets>(preset));

		const std::string shader_name = ref + "_particle";

		shader_manager->useShader(shader_name);

		shader_manager->setUniform(shader_name, "iTime", (float)glfwGetTime());
		shader_manager->setUniform(shader_name, "particleOrigin", origin);
		shader_manager->setUniform(shader_name, "iResolution", Vector2f{ NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize() }); // window size

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL after setting uniform variables in {0}: {1}", __FUNCTION__, err);
		}

		if (vao == 0) {
			vao = NIKE::SysParticle::Manager::getInstance().getVAO(static_cast<NIKE::SysParticle::Data::ParticlePresets>(preset));
		}
		glBindVertexArray(vao);

		static constexpr int NUM_VERTICES = 6;		// defined in vertex shader
		glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_VERTICES, draw_count);

		glBindVertexArray(0);
		shader_manager->unuseShader();

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
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
}