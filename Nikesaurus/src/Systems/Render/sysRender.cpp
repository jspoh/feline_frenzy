/*****************************************************************//**
 * \file   sysRender.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"
#include "Components/cTransform.h"
#include "Components/cPhysics.h"
#include "Components/cRender.h"
#include "Math/Mtx33.h"

namespace NIKE {

	Render::Manager::Manager() : frame_buffer{ 0 }, texture_color_buffer{ 0 }, VAO{ 0 }, VBO{ 0 } {
	}

	void Render::Manager::onEvent(std::shared_ptr<Windows::WindowResized> event) {
		NIKEE_INFO("Window resize event triggered: {0}x{1}", event->frame_buffer.x, event->frame_buffer.y);

		// Save current framebuffer binding to restore later
		GLint previous_fbo;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_fbo);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

		// Ensure we're not trying to delete invalid resources
		if (glIsFramebuffer(frame_buffer)) {
			// Unbind framebuffer before deletion
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &frame_buffer);
			NIKEE_INFO("Deleted framebuffer: {0}", frame_buffer);
		}

		if (glIsTexture(texture_color_buffer)) {
			// Unbind texture before deletion
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &texture_color_buffer);
			NIKEE_INFO("Deleted texture: {0}", texture_color_buffer);
		}

		// Create and validate new framebuffer
		glGenFramebuffers(1, &frame_buffer);
		if (!glIsFramebuffer(frame_buffer)) {
			NIKEE_CORE_ERROR("Failed to create new framebuffer");
			return;
		}
		NIKEE_INFO("Created new framebuffer: {0}", frame_buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		// Create and validate new texture
		glGenTextures(1, &texture_color_buffer);
		if (!glIsTexture(texture_color_buffer)) {
			NIKEE_CORE_ERROR("Failed to create new texture");
			glDeleteFramebuffers(1, &frame_buffer);
			return;
		}
		NIKEE_INFO("Created new texture: {0}", texture_color_buffer);

		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

		// Validate dimensions
		if (event->frame_buffer.x <= 0 || event->frame_buffer.y <= 0) {
			NIKEE_CORE_ERROR("Invalid framebuffer dimensions: {0}x{1}",
				event->frame_buffer.x, event->frame_buffer.y);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, event->frame_buffer.x, event->frame_buffer.y,
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture_color_buffer, 0);

		// Check framebuffer status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			NIKEE_CORE_ERROR("Framebuffer incomplete with status: {0}", status);
			// Cleanup on error
			glDeleteFramebuffers(1, &frame_buffer);
			glDeleteTextures(1, &texture_color_buffer);
			frame_buffer = 0;
			texture_color_buffer = 0;
			return;
		}

		// Restore previous bindings
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, previous_fbo);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}

		NIKEE_INFO("Window resize event completed successfully");
	}

	void Render::Manager::transformMatrix(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat) {
		//Transform matrix here
		Matrix_33 result, scale_mat, rot_mat, trans_mat;

		Matrix_33RotDeg(rot_mat, obj.rotation);

		Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
		Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
		result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

		// OpenGL requires matrix in col maj so transpose
		Matrix_33Transpose(x_form, result);
	}

	void Render::Manager::transformMatrixDebug(Transform::Transform const& obj, Matrix_33& x_form, Matrix_33 world_to_ndc_mat, bool render_wireframe) {
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

	void Render::Manager::renderObject(Matrix_33 const& x_form, Render::Shape const& e_shape) {
		//Set polygon mode
		glPolygonMode(GL_FRONT, GL_FILL);

		// use shader
		shader_system->useShader("base");

		//Shader set uniform
		shader_system->setUniform("base", "f_color", Vector3f(e_shape.color.r, e_shape.color.g, e_shape.color.b));
		shader_system->setUniform("base", "f_opacity", e_shape.color.a);
		shader_system->setUniform("base", "model_to_ndc", x_form);

		//Get model
		auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>(e_shape.model_id);

		//Draw
		glBindVertexArray(model->vaoid);
		glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderObject(Matrix_33 const& x_form, Render::Texture const& e_texture) {
		//Set polygon mode
		glPolygonMode(GL_FRONT, GL_FILL);

		// use shader
		shader_system->useShader("texture");

		//Texture unit
		constexpr int texture_unit = 6;

		// set texture
		glBindTextureUnit(
			texture_unit, // texture unit (binding index)
			NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->gl_data
		);

		glTextureParameteri(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->gl_data, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->gl_data, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Caculate UV Offset
		Vector2f framesize{ (1.0f / e_texture.frame_size.x) , (1.0f / e_texture.frame_size.y) };
		Vector2f uv_offset{ e_texture.frame_index.x * framesize.x, e_texture.frame_index.y * framesize.y };

		//Translate UV offset to bottom left
		uv_offset.y = std::abs(1 - uv_offset.y - framesize.y);

		//Set uniforms for texture rendering
		shader_system->setUniform("texture", "u_tex2d", texture_unit);
		shader_system->setUniform("texture", "u_opacity", e_texture.color.a);
		shader_system->setUniform("texture", "u_transform", x_form);
		shader_system->setUniform("texture", "uvOffset", uv_offset);
		shader_system->setUniform("texture", "frameSize", framesize);

		//Blending options for texture
		shader_system->setUniform("texture", "u_color", Vector3f(e_texture.color.r, e_texture.color.g, e_texture.color.b));
		shader_system->setUniform("texture", "u_blend", e_texture.b_blend);
		shader_system->setUniform("texture", "u_intensity", e_texture.intensity);

		//Flip texture options
		shader_system->setUniform("texture", "u_fliphorizontal", e_texture.b_flip.x);
		shader_system->setUniform("texture", "u_flipvertical", e_texture.b_flip.y);

		//Get model
		auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square-texture.model");

		//Draw
		glBindVertexArray(model->vaoid);
		glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderText(Matrix_33 const& x_form, Render::Text& e_text) {

		//Use text shader
		shader_system->useShader("text");

		//Set shader values
		shader_system->setUniform("text", "u_textColor", Vector3f(e_text.color.r, e_text.color.g, e_text.color.b));
		shader_system->setUniform("text", "u_opacity", e_text.color.a);
		shader_system->setUniform("text", "u_transform", x_form);

		//Set texture
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

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
		case TextOrigin::BOTTOM:
			pos = { -e_text.size.x / 2.0f, 0.0f };
			break;
		case TextOrigin::TOP:
			pos = { -e_text.size.x / 2.0f, -e_text.size.y };
			break;
		case TextOrigin::LEFT:
			pos = { 0.0f, -e_text.size.y / 2.0f };
			break;
		case TextOrigin::RIGHT:
			pos = { -e_text.size.x, -e_text.size.y / 2.0f };
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
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

	void Render::Manager::renderWireFrame(Matrix_33 const& x_form, Vector4f const& e_color) {
		//Set Polygon Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Use shader
		shader_system->useShader("base");

		//Shader set uniform
		shader_system->setUniform("base", "f_color", Vector3f(e_color.r, e_color.g, e_color.b));
		shader_system->setUniform("base", "f_opacity", e_color.a);
		shader_system->setUniform("base", "model_to_ndc", x_form);

		//Get model
		auto model = NIKE_ASSETS_SERVICE->getAsset<Assets::Model>("square.model");

		//Draw model
		glBindVertexArray(model->vaoid);
		glDrawElements(GL_LINE_LOOP, model->draw_count, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		//Unuse shader
		shader_system->unuseShader();

		//Reset Polygon Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Render::Manager::transformAndRenderEntity(Entity::Type entity, bool debugMode) {

		//Matrix used for rendering
		Matrix_33 matrix;

		Matrix_33 cam_ndcx = NIKE_UI_SERVICE->checkEntity(entity) ? NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform() : NIKE_CAMERA_SERVICE->getWorldToNDCXform();

		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) return; //Handling no value scenarios
		auto& e_transform = e_transform_comp.value().get();

		//Check If Texture
		if (auto e_texture_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);  e_texture_comp.has_value()) {
			auto& e_texture = e_texture_comp.value().get();

			//Check if texture is loaded
			if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_texture.texture_id)) {
				//Allow stretching of texture
				if (!e_texture.b_stretch) {
					//Copy transform for texture mapping ( Locks the transformation of a texture )
					Vector2f tex_size{ static_cast<float>(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->size.x) / e_texture.frame_size.x, static_cast<float>(NIKE_ASSETS_SERVICE->getAsset<Assets::Texture>(e_texture.texture_id)->size.y) / e_texture.frame_size.y };
					e_transform.scale = tex_size.normalized() * e_transform.scale.length();
				}

				// Transform matrix here
				transformMatrix(e_transform, matrix, cam_ndcx);

				//Render Texture
				renderObject(matrix, e_texture);
			}
		}
		else if (auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);  e_shape_comp.has_value()) {
			auto& e_shape = e_shape_comp.value().get();

			//Check if model exists
			if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {
				// Transform matrix here
				transformMatrix(e_transform, matrix, cam_ndcx);

				//Render Shape
				renderObject(matrix, e_shape);
			}
		}

		if (debugMode) {
			// Render debugging wireframe
			Vector4f wire_frame_color{ 1.0f, 0.0f, 0.0f, 1.0f };

			//Check for collider component
			if (auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);  e_collider_comp.has_value()) {
				auto const& e_collider = e_collider_comp.value().get();

				if (e_collider.b_collided) {
					wire_frame_color = { 0.0f, 1.0f, 0.0f, 1.0f };
				}
				//Calculate wireframe matrix
				transformMatrixDebug(e_collider.transform, matrix, cam_ndcx, true);
				renderWireFrame(matrix, wire_frame_color);
			}
			else {
				//Calculate wireframe matrix
				transformMatrixDebug(e_transform, matrix, cam_ndcx, true);
				renderWireFrame(matrix, wire_frame_color);
			}

			//Calculate direction matrix
			if (auto e_velo_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);  e_velo_comp.has_value()) {
				auto const& e_velo = e_velo_comp.value().get();

				if (e_velo.velocity.x != 0.0f || e_velo.velocity.y != 0.0f) {
					Transform::Transform dir_transform = e_transform;
					dir_transform.scale.x = 1.0f;
					dir_transform.rotation = -atan2(e_velo.velocity.x, e_velo.velocity.y) * static_cast<float>((180.0f / M_PI));
					dir_transform.position += {0.0f, e_transform.scale.y / 2.0f};
					transformMatrixDebug(dir_transform, matrix, cam_ndcx, false);
					renderWireFrame(matrix, wire_frame_color);
				}
			}
		}
	}

	void Render::Manager::transformAndRenderText(Entity::Type entity) {

		//Matrix used for rendering
		Matrix_33 matrix;

		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) return;
		auto& e_transform = e_transform_comp.value().get();

		//Get Text
		auto e_text_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity);
		if (!e_text_comp.has_value()) return;
		auto& e_text = e_text_comp.value().get();

		//Check if font exists
		if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_text.font_id)) {
			//Make copy of transform, scale to 1.0f for calculating matrix
			Transform::Transform copy = e_transform;
			copy.scale = { 1.0f, 1.0f };

			//Transform text matrix
			transformMatrix(copy, matrix, NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform());

			//Render text
			renderText(matrix, e_text);
		}
	}

	void Render::Manager::renderViewport() {

		//Render to frame buffer if imgui is active
		if (NIKE_LVLEDITOR_SERVICE->getEditorState()) {
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			//SKip inactive layer
			if (!layer->getLayerState())
				continue;

			for (auto& entity : entities) {
				if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;

				//Skip entity if no transform is present
				if (!NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity))
					continue;

				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Texture>(entity) || NIKE_ECS_MANAGER->checkEntityComponent<Render::Shape>(entity)) {
#ifndef NDEBUG
					transformAndRenderEntity(entity, NIKE_LVLEDITOR_SERVICE->getDebugState());
#else
					transformAndRenderEntity(entity, false);
#endif
				}
			}
		}

		// render text last
		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			if (!layer->getLayerState())
				continue;
			for (auto& entity : entities) {
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Text>(entity) && NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
					transformAndRenderText(entity);
				}
			}
		}

		if (NIKE_LVLEDITOR_SERVICE->getEditorState()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ViewportTexture>(texture_color_buffer));
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind after rendering
		}
	}

	void Render::Manager::init() {

		glGenFramebuffers(1, &frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		// Create a color attachment texture
		glGenTextures(1, &texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x, NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NIKEE_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Create shader system
		shader_system = std::make_unique<Shader::Manager>();

		shader_system->init();

		//GL enable opacity blending option
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Setup VAO & VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Render::Manager::update() {
		renderViewport();
	}
}
