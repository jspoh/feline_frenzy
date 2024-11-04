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
		auto model = NIKE_ASSETS_SERVICE->getModel(e_shape.model_ref);

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
			NIKE_ASSETS_SERVICE->getTexture(e_texture.texture_ref)->gl_data
		);

		glTextureParameteri(NIKE_ASSETS_SERVICE->getTexture(e_texture.texture_ref)->gl_data, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(NIKE_ASSETS_SERVICE->getTexture(e_texture.texture_ref)->gl_data, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		auto model = NIKE_ASSETS_SERVICE->getModel("square-texture");

		//Draw
		glBindVertexArray(model->vaoid);
		glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderText(Matrix_33 const& x_form, Render::Text const& e_text, Transform::Transform& e_transform) {

		//Use text shader
		shader_system->useShader("text");

		//Set shader values
		shader_system->setUniform("text", "u_textColor", Vector3f(e_text.color.r, e_text.color.g, e_text.color.b));
		shader_system->setUniform("text", "u_opacity", e_text.color.a);
		shader_system->setUniform("text", "u_transform", x_form);

		//Set texture
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		//Calculate size of text
		for (char c : e_text.text) {
			Assets::Font::Character ch = NIKE_ASSETS_SERVICE->getFont(e_text.font_ref)->char_map[c];

			//Calculate width
			e_transform.scale.x += (ch.advance >> 6) * e_text.scale;

			//Calculate height
			e_transform.scale.y = ch.size.y * e_text.scale > e_transform.scale.y ? ch.size.y * e_text.scale : e_transform.scale.y;
		}

		//Text rendering position based on bot left
		Vector2f pos;

		//Get text bottom left position for rendering
		switch (e_text.origin) {
		case TextOrigin::CENTER:
			pos = { -e_transform.scale.x / 2.0f, -e_transform.scale.y / 2.0f };
			break;
		case TextOrigin::BOTTOM:
			pos = { -e_transform.scale.x / 2.0f, 0.0f };
			break;
		case TextOrigin::TOP:
			pos = { -e_transform.scale.x / 2.0f, -e_transform.scale.y };
			break;
		case TextOrigin::LEFT:
			pos = { 0.0f, -e_transform.scale.y / 2.0f };
			break;
		case TextOrigin::RIGHT:
			pos = { -e_transform.scale.x, -e_transform.scale.y / 2.0f };
			break;
		default:
			break;
		}

		//Iterate through all characters
		for (char c : e_text.text)
		{
			Assets::Font::Character ch = NIKE_ASSETS_SERVICE->getFont(e_text.font_ref)->char_map[c];

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
		auto model = NIKE_ASSETS_SERVICE->getModel("square");

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

		//Get transform
		auto& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);

		//Check If Shape
		if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Shape>(entity)) {
			auto& e_shape = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);

			// Transform matrix here
			transformMatrix(e_transform, matrix, camera_system->getWorldToNDCXform());

			//Render Shape
			renderObject(matrix, e_shape);
		}
		else if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Texture>(entity)) {
			auto& e_texture = NIKE_ECS_MANAGER->getEntityComponent<Render::Texture>(entity);

			//Allow stretching of texture
			if (!e_texture.b_stretch) {
				//Copy transform for texture mapping ( Locks the transformation of a texture )
				Vector2f tex_size{ static_cast<float>(NIKE_ASSETS_SERVICE->getTexture(e_texture.texture_ref)->size.x) / e_texture.frame_size.x, static_cast<float>(NIKE_ASSETS_SERVICE->getTexture(e_texture.texture_ref)->size.y) / e_texture.frame_size.y };
				e_transform.scale = tex_size.normalized() * e_transform.scale.length();
			}

			// Transform matrix here
			transformMatrix(e_transform, matrix, camera_system->getWorldToNDCXform());

			// Render Texture
			renderObject(matrix, e_texture);
		}

		if (debugMode) {
			// Render debugging wireframe
			Vector4f wire_frame_color{ 1.0f, 0.0f, 0.0f, 1.0f };

			//Check for collider component
			if (NIKE_ECS_MANAGER->checkEntityComponent<Physics::Collider>(entity)) {
				auto const& e_collider = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);

				if (e_collider.b_collided) {
					wire_frame_color = { 0.0f, 1.0f, 0.0f, 1.0f };
				}
			}

			//Calculate wireframe matrix
			transformMatrixDebug(e_transform, matrix, camera_system->getWorldToNDCXform(), true);
			renderWireFrame(matrix, wire_frame_color);

			//Calculate direction matrix
			if (NIKE_ECS_MANAGER->checkEntityComponent<Physics::Dynamics>(entity)) {
				auto const& e_velo = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);

				if (e_velo.velocity.x != 0.0f || e_velo.velocity.y != 0.0f) {
					Transform::Transform dir_transform = e_transform;
					dir_transform.scale.x = 1.0f;
					dir_transform.rotation = -atan2(e_velo.velocity.x, e_velo.velocity.y) * static_cast<float>((180.0f / M_PI));
					dir_transform.position += {0.0f, e_transform.scale.y / 2.0f};
					transformMatrixDebug(dir_transform, matrix, camera_system->getWorldToNDCXform(), false);
					renderWireFrame(matrix, wire_frame_color);
				}
			}
		}
	}

	void Render::Manager::transformAndRenderText(Entity::Type entity) {

		//Matrix used for rendering
		Matrix_33 matrix;

		//Get transform
		auto& e_transform = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);

		auto& e_text = NIKE_ECS_MANAGER->getEntityComponent<Render::Text>(entity);

		//Set transform scale to 1.0f for calculating matrix
		e_transform.scale = { 1.0f, 1.0f };

		//Transform text matrix
		transformMatrix(e_transform, matrix, camera_system->getFixedWorldToNDCXform());

		//Render text
		renderText(matrix, e_text, e_transform);
	}

	void Render::Manager::renderViewport() {
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& layer : NIKE_SCENES_SERVICE->getCurrScene()->getLayers()) {
			for (auto& entity : entities) {
				if (!layer->checkEntity(entity))
					continue;

				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Text>(entity)) {
					transformAndRenderText(entity);
				}
				else {
					transformAndRenderEntity(entity, true);
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind after rendering

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

		//Create camera system
		camera_system = std::make_unique<Camera::System>();

		//Init Camera ( Camera height defaulted at window height )
		camera_system->init();

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
