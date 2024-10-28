/*****************************************************************//**
 * \file   sysRender.cpp
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (50%)
 * \co-author Sean Gwee, 2301326, g.boonxuensean@digipen.edu (50%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Systems/Render/sysRender.h"
#include "Components/cTransform.h"
#include "Components/cRender.h"
#include "Math/Mtx33.h"

namespace NIKESAURUS {

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
			Matrix_33RotDeg(rot_mat, 0);
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
		auto model = NIKEEngine.getService<Assets::Service>()->getModel(e_shape.model_ref);

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
			NIKEEngine.getService<Assets::Service>()->getTexture(e_texture.texture_ref)->gl_data
		);

		glTextureParameteri(NIKEEngine.getService<Assets::Service>()->getTexture(e_texture.texture_ref)->gl_data, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(NIKEEngine.getService<Assets::Service>()->getTexture(e_texture.texture_ref)->gl_data, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Caculate UV Offset
		Vector2 uv_offset{ e_texture.frame_index.x * e_texture.frame_size.x, e_texture.frame_index.y * e_texture.frame_size.y };

		//Translate UV offset to bottom left
		uv_offset.y = std::abs(1 - uv_offset.y - e_texture.frame_size.y);

		//Set uniforms for texture rendering
		shader_system->setUniform("texture", "u_tex2d", texture_unit);
		shader_system->setUniform("texture", "u_opacity", e_texture.color.a);
		shader_system->setUniform("texture", "u_transform", x_form);
		shader_system->setUniform("texture", "uvOffset", uv_offset);
		shader_system->setUniform("texture", "frameSize", e_texture.frame_size);
		shader_system->setUniform("texture", "u_is_font", false);

		//Get model
		auto model = NIKEEngine.getService<Assets::Service>()->getModel("square-texture");

		//Draw
		glBindVertexArray(model->vaoid);
		glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderText(Render::Text const& e_text) {
		//Set polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// use shader
		shader_system->useShader("texture");

		//Texture unit
		constexpr int texture_unit = 6;

		//Get model
		const auto& model = NIKEEngine.getService<Assets::Service>()->getModel("square-texture");

		// num chars counter
		int i{};

		// Iterate through all characters in the string
		for (char c : e_text.text) {
			const Assets::Font::Character& ch = NIKEEngine.getService<Assets::Service>()->getFont(e_text.font_ref)->char_map.at(c);
			const unsigned int ch_tex_hdl = ch.texture;

			// set texture
			glBindTextureUnit(
				texture_unit, // texture unit (binding index)
				ch_tex_hdl
			);

			glTextureParameteri(ch_tex_hdl, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(ch_tex_hdl, GL_TEXTURE_WRAP_T, GL_REPEAT);

			//Matrix33::Matrix_33 xform, scale_mat, rot_mat, trans_mat;
			//Matrix_33Rot(rot_mat, 0.0f);
			//Matrix_33Scale(scale_mat, e_text.scale, e_text.scale);
			//Matrix_33Translate(trans_mat, e_text.position.x, e_text.position.y);
			//xform = camera_system->getWorldToNDCXform() * trans_mat * rot_mat * scale_mat;
			//Matrix_33Transpose(xform, xform);

			// Calculate the position of the character
			constexpr float offset = 0.01f;
			const float xpos = e_text.position.x + e_text.scale * i + offset;
			const float ypos = e_text.position.y;

			// !TODO: refine this
			Matrix_33 xform = Matrix_33::Identity();
			xform *= e_text.scale;
			xform.matrix_33[2][0] = xpos;
			xform.matrix_33[2][1] = ypos;

			// required to flip around for opengl rendering
			xform.matrix_33[1][1] *= -1;

			const Vector2f uv_offset{ 0, 0 };
			const Vector2f frame_size{ 1, 1 };

			//Set uniforms for texture rendering
			shader_system->setUniform("texture", "u_tex2d", texture_unit);
			shader_system->setUniform("texture", "u_opacity", 1);
			shader_system->setUniform("texture", "u_transform", xform);
			shader_system->setUniform("texture", "uvOffset", uv_offset);
			shader_system->setUniform("texture", "frameSize", frame_size);
			shader_system->setUniform("texture", "u_is_font", true);

			//Draw
			glBindVertexArray(model->vaoid);
			glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

			i++;
		}

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderWireFrame(Matrix_33 const& x_form, Render::Color const& e_color) {
		//Set Polygon Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Use shader
		shader_system->useShader("base");

		//Shader set uniform
		shader_system->setUniform("base", "f_color", Vector3f(e_color.r, e_color.g, e_color.b));
		shader_system->setUniform("base", "f_opacity", e_color.a);
		shader_system->setUniform("base", "model_to_ndc", x_form);

		//Get model
		auto model = NIKEEngine.getService<Assets::Service>()->getModel("square");

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

		//Run time error if entity has no transform
		if (!NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Transform>(entity))
			throw std::runtime_error("Trying to render object with no transform. Render failed.");
		
		//Matrix used for rendering
		Matrix_33 matrix;

		//Get transform
		auto& e_transform = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Transform::Transform>(entity);

		//Check If Shape
		if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Render::Shape>(entity)) {
			auto& e_shape = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Render::Shape>(entity);

			// Transform matrix here
			transformMatrix(e_transform, matrix, camera_system->getWorldToNDCXform());

			//Render Shape
			renderObject(matrix, e_shape);
		}
		else if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Render::Texture>(entity)) {
			auto& e_texture = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Render::Texture>(entity);

			//Copy transform for texture mapping
			Transform::Transform copy = e_transform;
			Vector2f tex_size{ (float)NIKEEngine.getService<Assets::Service>()->getTexture(e_texture.texture_ref)->size.x, (float)NIKEEngine.getService<Assets::Service>()->getTexture(e_texture.texture_ref)->size.y };
			copy.scale = tex_size.normalized()* copy.scale.length();

			// Transform matrix here
			transformMatrix(copy, matrix, camera_system->getWorldToNDCXform());

			// Render Texture
			renderObject(matrix, e_texture);
		}

		if (debugMode) {
			// Render debugging wireframe
			Render::Color wire_frame_color{ 1.0f, 0.0f, 0.0f, 1.0f };

			//Calculate wireframe matrix
			transformMatrixDebug(e_transform, matrix, camera_system->getWorldToNDCXform(), true);
			renderWireFrame(matrix, wire_frame_color);

			//Calculate direction matrix
			if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Transform::Velocity>(entity)) {
				auto const& e_velo = NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Transform::Velocity>(entity);

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

	void Render::Manager::init() {

		//Create shader system
		shader_system = std::make_unique<Shader::Manager>();

		//Create camera system
		camera_system = std::make_unique<Camera::System>();

		//Init Camera ( Camera height defaulted at window height )
		camera_system->init();

		//GL enable opacity blending option
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Render::Manager::update() {

		//Before drawing clear screen
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& layer : NIKEEngine.getService<Scenes::Service>()->getCurrScene()->getLayers()) {
			for (auto& entity : entities) {
				if (!layer->checkEntity(entity))
					continue;

				//Transform and render object
				transformAndRenderEntity(entity, true);
			}
		}

		////Update all and render except camera entities
		//for (auto& entity : entities) {
		//	////Check for text objects
		//	//if (NIKEEngine.getService<Coordinator::Manager>()->checkEntityComponent<Render::Text>(entity)) {
		//	//	renderText(NIKEEngine.getService<Coordinator::Manager>()->getEntityComponent<Render::Text>(entity));
		//	//	continue;
		//	//}
		//	//Transform and render object
		//	transformAndRenderEntity(entity, true);
		//}
	}
}
