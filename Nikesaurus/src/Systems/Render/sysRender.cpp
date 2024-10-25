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

	void Render::Manager::transformMatrix(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat) {
		//Transform matrix here
		Matrix33::Matrix_33 result, scale_mat, rot_mat, trans_mat;

		float angleDisp = obj.rotation;

		Matrix_33RotDeg(rot_mat, angleDisp);
		Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
		Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
		result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

		// OpenGL requires matrix in col maj so transpose
		Matrix_33Transpose(x_form, result);
	}

	void Render::Manager::transformMatrixDebug(Transform::Transform const& obj, Matrix33::Matrix_33& x_form, Matrix33::Matrix_33 world_to_ndc_mat) {
		//Transform matrix here
		Matrix33::Matrix_33 result, scale_mat, rot_mat, trans_mat;

		Matrix_33Rot(rot_mat, 0);
		Matrix_33Scale(scale_mat, obj.scale.x, obj.scale.y);
		Matrix_33Translate(trans_mat, obj.position.x, obj.position.y);
		result = world_to_ndc_mat * trans_mat * rot_mat * scale_mat;

		// OpenGL requires matrix in col maj so transpose
		Matrix_33Transpose(x_form, result);
	}

	void Render::Manager::renderObject(Render::Shape const& e_shape) {
		//Set polygon mode
		glPolygonMode(GL_FRONT, GL_FILL);

		// use shader
		shader_system->useShader("base");

		//Shader set uniform
		shader_system->setUniform("base", "f_color", e_shape.color.color);
		shader_system->setUniform("base", "f_opacity", e_shape.color.alpha);
		shader_system->setUniform("base", "model_to_ndc", e_shape.x_form);

		//Get model
		auto model = NIKEEngine.accessAssets()->getModel(e_shape.model_ref);

		//Draw
		glBindVertexArray(model->vaoid);
		glDrawElements(model->primitive_type, model->draw_count, GL_UNSIGNED_INT, nullptr);

		//Unuse texture
		glBindVertexArray(0);
		shader_system->unuseShader();
	}

	void Render::Manager::renderObject(Render::Texture const& e_texture) {
		//Set polygon mode
		glPolygonMode(GL_FRONT, GL_FILL);

		// use shader
		shader_system->useShader("texture");

		//Texture unit
		constexpr int texture_unit = 6;

		// set texture
		glBindTextureUnit(
			texture_unit, // texture unit (binding index)
			NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref)
		);

		glTextureParameteri(NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref), GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(NIKEEngine.accessAssets()->getTexture(e_texture.texture_ref), GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Caculate UV Offset
		Vector2 uv_offset{ e_texture.frame_index.x * e_texture.frame_size.x, e_texture.frame_index.y * e_texture.frame_size.y };

		//Translate UV offset to bottom left
		uv_offset.y = std::abs(1 - uv_offset.y - e_texture.frame_size.y);

		//Set uniforms for texture rendering
		shader_system->setUniform("texture", "u_tex2d", texture_unit);
		shader_system->setUniform("texture", "u_opacity", e_texture.color.alpha);
		shader_system->setUniform("texture", "u_transform", e_texture.x_form);
		shader_system->setUniform("texture", "uvOffset", uv_offset);
		shader_system->setUniform("texture", "frameSize", e_texture.frame_size);
		shader_system->setUniform("texture", "u_is_font", false);

		//Get model
		auto model = NIKEEngine.accessAssets()->getModel("square-texture");

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
		const auto& model = NIKEEngine.accessAssets()->getModel("square-texture");

		// num chars counter
		int i{};

		// Iterate through all characters in the string
		for (char c : e_text.text) {
			const Character& ch = NIKEEngine.accessAssets()->getFont(e_text.font_ref).at(c);
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
			Matrix33::Matrix_33 xform = Matrix33::Matrix_33::Identity();
			xform *= e_text.scale;
			xform.matrix_33[2][0] = xpos;
			xform.matrix_33[2][1] = ypos;

			// required to flip around for opengl rendering
			xform.matrix_33[1][1] *= -1;

			const Vector2 uv_offset{ 0, 0 };
			const Vector2 frame_size{ 1, 1 };

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

	void Render::Manager::renderWireFrame(Matrix33::Matrix_33 const& x_form, Render::Color const& e_color) {
		//Set Polygon Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Use shader
		shader_system->useShader("base");

		//Shader set uniform
		shader_system->setUniform("base", "f_color", e_color.color);
		shader_system->setUniform("base", "f_opacity", e_color.alpha);
		shader_system->setUniform("base", "model_to_ndc", x_form);

		//Get model
		auto model = NIKEEngine.accessAssets()->getModel("square");

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
		auto& e_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);

		//Check If Shape
		if (NIKEEngine.checkEntityComponent<Render::Shape>(entity)) {
			auto& e_shape = NIKEEngine.getEntityComponent<Render::Shape>(entity);

			// Transform matrix here
			transformMatrix(e_transform, e_shape.x_form, camera_system->getWorldToNDCXform());

			//Render Shape
			renderObject(e_shape);
		}
		else if (NIKEEngine.checkEntityComponent<Render::Texture>(entity)) {
			auto& e_texture = NIKEEngine.getEntityComponent<Render::Texture>(entity);

			//Copy transform for texture mapping
			Transform::Transform copy = e_transform;
			copy.scale = e_texture.texture_size.normalized() * copy.scale.length();

			// Transform matrix here
			transformMatrix(copy, e_texture.x_form, camera_system->getWorldToNDCXform());

			// Render Texture
			renderObject(e_texture);
		}

		if (debugMode) {
			Matrix33::Matrix_33 mtx_wireframe;
			//Calculate wireframe matrix
			transformMatrixDebug(e_transform, mtx_wireframe, camera_system->getWorldToNDCXform());

			// Render debugging wireframe
			Render::Color wire_frame_color{ { 1.0f, 0.0f, 0.0f }, 1.0f };
			renderWireFrame(mtx_wireframe, wire_frame_color);
		}
	}

	void Render::Manager::trackCamEntity(std::string const& cam_identifier) {
		camera_system->trackCamEntity(cam_identifier);
	}

	std::unique_ptr<Camera::System>& Render::Manager::getCamEntity() {
		return camera_system;
	}

	void Render::Manager::init() {
		//Create font system
		font_system = std::make_unique<Font::Manager>();

		//Create shader system
		shader_system = std::make_unique<Shader::Manager>();

		//Create camera system
		camera_system = std::make_unique<Camera::System>();

		//Init Camera ( Camera height defaulted at window height )
		camera_system->init(NIKEEngine.accessWindow()->getWindowSize().y);

		//GL enable opacity blending option
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Render::Manager::update() {

		//Swap buffers
		NIKEEngine.getService<Windows::Manager>()->swapBuffers();

		// Before drawing clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		//Update set of cameras
		std::set<Entity::Type> cam_entities;
		for (auto& entity : entities) {
			//Update camera position
			if (!NIKEEngine.checkEntityComponent<Render::Cam>(entity))
				continue;

			//Update camera and push to list
			auto& e_transform = NIKEEngine.getEntityComponent<Transform::Transform>(entity);
			auto& e_camera = NIKEEngine.getEntityComponent<Render::Cam>(entity);
			e_camera.position = e_transform.position;
			cam_entities.insert(entity);
		}

		//Update camera with set of cam entitiess
		camera_system->updateCameraEntities(std::move(cam_entities));

		//Update camera system
		camera_system->update();

		//Update all and render except camera entities
		for (auto& entity : entities) {
			//Check for text objects
			if (NIKEEngine.checkEntityComponent<Render::Text>(entity)) {
				renderText(NIKEEngine.getEntityComponent<Render::Text>(entity));
				continue;
			}

			//Skip camera
			if (NIKEEngine.checkEntityComponent<Render::Cam>(entity)) continue;

			//Transform and render object
			transformAndRenderEntity(entity, debug_mode);
		}

		// Render camera above all components
		for (auto& entity : entities) {
			if (NIKEEngine.checkEntityComponent<Render::Cam>(entity)) {
				transformAndRenderEntity(entity, debug_mode);
			}
		}
	}
}
