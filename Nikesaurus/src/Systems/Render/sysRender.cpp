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
		NIKE_RENDER_SERVICE->render_instances_quad.reserve(NIKE_RENDER_SERVICE->MAX_INSTANCES);

		if (NIKE_RENDER_SERVICE->BATCHED_RENDERING) {
			NIKEE_INFO("Using batched rendering");
		}
		else {
			NIKEE_INFO("Not using batched rendering");
		}
	}

	void Render::Manager::onEvent(std::shared_ptr<Windows::WindowResized> event) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
		}

		// Cleanup old resources
		glDeleteFramebuffers(1, &frame_buffer);
		glDeleteTextures(1, &texture_color_buffer);

		// Create a new framebuffer
		glGenFramebuffers(1, &frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

		// Create a color attachment texture
		glGenTextures(1, &texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, event->frame_buffer.x, event->frame_buffer.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

		// Check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NIKEE_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete! (Not an issue if triggered by focus loss)");

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
		}
	}

	void Render::Manager::renderText(Matrix_33 const& x_form, Render::Text& e_text) {

		//Use text shader
		shader_manager->useShader("text");

		//Set shader values
		shader_manager->setUniform("text", "u_textColor", Vector3f(e_text.color.r, e_text.color.g, e_text.color.b));
		shader_manager->setUniform("text", "u_opacity", e_text.color.a);
		shader_manager->setUniform("text", "u_transform", x_form);

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


	void Render::Manager::transformAndRenderEntity(Entity::Type entity, bool debugMode) {
		//Matrix used for rendering
		Matrix_33 matrix;

		//Get transform
		auto e_transform_comp = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
		if (!e_transform_comp.has_value()) return; //Handling no value scenarios
		auto& e_transform = e_transform_comp.value().get();

		Matrix_33 cam_ndcx = NIKE_UI_SERVICE->checkEntity(entity) ? NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform() : NIKE_CAMERA_SERVICE->getWorldToNDCXform();
		if (e_transform.use_screen_pos) {
			cam_ndcx = NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform();
		}

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
				NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx, Vector2b{ e_texture.b_flip.x, e_texture.b_flip.y });

				// Render Texture
				NIKE_RENDER_SERVICE->renderObject(matrix, e_texture);
			}
		}
		else if (auto e_shape_comp = NIKE_ECS_MANAGER->getEntityComponent<Render::Shape>(entity);  e_shape_comp.has_value()) {
			auto& e_shape = e_shape_comp.value().get();

			//Check if model exists
			if (NIKE_ASSETS_SERVICE->isAssetRegistered(e_shape.model_id)) {
				// Transform matrix here
				NIKE_RENDER_SERVICE->transformMatrix(e_transform, matrix, cam_ndcx);

				//Render Shape
				NIKE_RENDER_SERVICE->renderObject(matrix, e_shape);
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
				NIKE_RENDER_SERVICE->transformMatrixDebug(e_collider.transform, matrix, cam_ndcx, true);
				NIKE_RENDER_SERVICE->renderBoundingBox(matrix, wire_frame_color);
			}
			else {
				//Calculate wireframe matrix
				NIKE_RENDER_SERVICE->transformMatrixDebug(e_transform, matrix, cam_ndcx, true);
				NIKE_RENDER_SERVICE->renderBoundingBox(matrix, wire_frame_color);
			}

			//Calculate direction matrix
			if (auto e_velo_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Dynamics>(entity);  e_velo_comp.has_value()) {
				auto const& e_velo = e_velo_comp.value().get();

				if (e_velo.velocity.x != 0.0f || e_velo.velocity.y != 0.0f) {
					Transform::Transform dir_transform = e_transform;
					dir_transform.scale.x = 1.0f;
					dir_transform.rotation = -atan2(e_velo.velocity.x, e_velo.velocity.y) * static_cast<float>((180.0f / M_PI));
					dir_transform.position += {0.0f, e_transform.scale.y / 2.0f};
					NIKE_RENDER_SERVICE->transformMatrixDebug(dir_transform, matrix, cam_ndcx, false);
					NIKE_RENDER_SERVICE->renderBoundingBox(matrix, wire_frame_color);
				}
			}
		}
	}

	void Render::Manager::transformAndRenderText(Entity::Type entity) {

		//Matrix used for rendering
		Matrix_33 matrix;

		//Get transform
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
			NIKE_RENDER_SERVICE->transformMatrix(copy, matrix, NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform());

			//Render text
			renderText(matrix, e_text);
		}
	}

	void Render::Manager::init() {
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

		//Setup event listening for frame buffer resize
		std::shared_ptr<Render::Manager> render_sys_wrapped(this, [](Render::Manager*) {});
		NIKE_EVENTS_SERVICE->addEventListeners<Windows::WindowResized>(render_sys_wrapped);

		//Create shader system
		shader_manager = std::make_unique<Shader::ShaderManager>();

		//Init shader system
		shader_manager->init();

		NIKE_RENDER_SERVICE->init();

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

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

		glClearColor(0, 0, 0, 1);

#ifndef NDEBUG
		//Render to frame buffer if imgui is active
		if (NIKE_LVLEDITOR_SERVICE->getEditorState()) {
			glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		}
#endif
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			//SKip inactive layer
			if (!layer->getLayerState())
				continue;

			for (auto& entity : entities) {

				// Skip entities marked for deletion
				//if (std::find(entities_to_destroy.begin(), entities_to_destroy.end(), entity) != entities_to_destroy.end())
				//	continue;

				if (layer->getLayerID() != NIKE_ECS_MANAGER->getEntityLayerID(entity))
					continue;

				//Skip entity if no transform is present
				if (!NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity))
					continue;

				// skip entity if is hidden
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Hidden>(entity))
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

		if (NIKE_RENDER_SERVICE->BATCHED_RENDERING) {
			NIKE_RENDER_SERVICE->batchRenderTextures();	// at least 1 call to this is required every frame at the very end
			NIKE_RENDER_SERVICE->batchRenderObject();		// at least 1 call to this is required every frame at the very end
			NIKE_RENDER_SERVICE->batchRenderBoundingBoxes();
		}

		// render text last
		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			if (!layer->getLayerState())
				continue;
			for (auto& entity : entities) {
				// skip entity if is hidden
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Hidden>(entity))
					continue;
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Text>(entity) && NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
					transformAndRenderText(entity);
				}
			}
		}

#ifndef NDEBUG
		if (NIKE_LVLEDITOR_SERVICE->getEditorState()) {
			NIKE_EVENTS_SERVICE->dispatchEvent(std::make_shared<Render::ViewportTexture>(texture_color_buffer));
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind after rendering
		}
#endif

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}
}