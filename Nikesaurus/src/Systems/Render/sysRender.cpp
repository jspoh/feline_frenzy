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
#include "Systems/sysParticle.h"


// !TODO: jspoh reorg
namespace {

	Matrix_33 getWorldToScreenMtx() {
		// transform to screen coords
		NIKE::Render::Cam cam = NIKE_CAMERA_SERVICE->getActiveCamera();

		Matrix_33 view_xform{
			1, 0,  -cam.position.x,
			0, 1,  -cam.position.y,
			0, 0, 1
		};

		const float cam_height = NIKE_CAMERA_SERVICE->getCameraHeight();

		Matrix_33 cam_to_ndc_xform{
			2.0f / NIKE_WINDOWS_SERVICE->getWindow()->getAspectRatio() / (cam_height * cam.zoom), 0, 0,
			0, 2.0f / (cam_height * cam.zoom), 0,
			0, 0, 1
		};

		const float screenWidth = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().x);
		const float screenHeight = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);

		Matrix_33 screen_xform = Matrix_33{
			screenWidth * 0.5f, 0, screenWidth * 0.5f,
			0, screenHeight * 0.5f, screenHeight * 0.5f,
			0, 0, 1
		} *cam_to_ndc_xform * view_xform;

		return screen_xform;
	};

	Vector2f worldToScreen(const Vector2f& world_pos) {
		Matrix_33 screen_xform = getWorldToScreenMtx();
		return screen_xform * world_pos;
	}
}

namespace NIKE {

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
			// Render debugging bounding box
			Vector4f bounding_box_color{ 1.0f, 0.0f, 0.0f, 1.0f };

			//Check for collider component
			if (auto e_collider_comp = NIKE_ECS_MANAGER->getEntityComponent<Physics::Collider>(entity);  e_collider_comp.has_value()) {
				auto const& e_collider = e_collider_comp.value().get();

				if (e_collider.b_collided) {
					bounding_box_color = { 0.0f, 1.0f, 0.0f, 1.0f };
				}

				//Calculate bounding box matrix
				NIKE_RENDER_SERVICE->transformMatrixDebug(e_collider.transform, matrix, cam_ndcx, true);
				NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
			}
			else {
				//Calculate bounding box matrix
				NIKE_RENDER_SERVICE->transformMatrixDebug(e_transform, matrix, cam_ndcx, true);
				NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
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
					NIKE_RENDER_SERVICE->renderBoundingBox(matrix, bounding_box_color);
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
			NIKE_RENDER_SERVICE->renderText(matrix, e_text);
		}
	}

	void Render::Manager::init() {

		NIKE_RENDER_SERVICE->init();

		//GL enable opacity blending option
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	void Render::Manager::update() {

		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at beginning of {0}: {1}", __FUNCTION__, err);
		}

#ifndef NDEBUG
		//Bind frame buffer for rendering to editor ( Binding occurs when editor is active )
		NIKE_LVLEDITOR_SERVICE->bindEditorFrameBuffer();
#endif

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

			//SKip inactive layer
			if (!layer->getLayerState())
				continue;
			
			for (auto& entity : layer->getEntitites()) {

				//Skip entity not registered to this system
				if (entities.find(entity) == entities.end()) continue;

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
					// !TODO: jspoh move this code to entity update
					// update entity particles
					if (NIKE_ECS_MANAGER->checkEntityComponent<Render::ParticleEmitter>(entity)) {

						const std::unordered_map<std::string, std::shared_ptr<void>> comps = NIKE_ECS_MANAGER->getAllEntityComponents(entity);

						// get particle emitter component
						const Render::ParticleEmitter* pe_comp = reinterpret_cast<Render::ParticleEmitter*>(comps.at("Render::ParticleEmitter").get());

						// get transform component
						const Transform::Transform* transform_comp = reinterpret_cast<Transform::Transform*>(comps.at("Transform::Transform").get());

						// get particle location in screen coords
						const Vector2f world_particle_origin = transform_comp->position + pe_comp->offset;

						NIKE::SysParticle::ParticleSystem& ps = NIKE::SysParticle::Manager::getInstance().getParticleSystem(pe_comp->ref);

						// update particle location
						if (ps.origin != world_particle_origin) {
							NIKE::SysParticle::Manager::getInstance().setParticleSystemOrigin(pe_comp->ref, world_particle_origin);
						}

						// update changes to particle preset
						if (static_cast<int>(ps.preset) != pe_comp->preset) {
							NIKE::SysParticle::Manager::getInstance().setParticleSystemPreset(pe_comp->ref, static_cast<SysParticle::Data::ParticlePresets>(pe_comp->preset));
						}

						// update changes to particle render type
						if (static_cast<int>(ps.render_type) != pe_comp->render_type) {
							ps.render_type = static_cast<SysParticle::Data::ParticleRenderType>(pe_comp->render_type);
						}

						// update particle duration
						if (ps.duration != pe_comp->duration) {
							NIKE::SysParticle::Manager::getInstance().setParticleSystemDuration(pe_comp->ref, pe_comp->duration);
						}
					}
				}
			}
		}

		if (NIKE_RENDER_SERVICE->BATCHED_RENDERING) {
			NIKE_RENDER_SERVICE->batchRenderTextures();	// at least 1 call to this is required every frame at the very end
			NIKE_RENDER_SERVICE->batchRenderObject();		// at least 1 call to this is required every frame at the very end
			NIKE_RENDER_SERVICE->batchRenderBoundingBoxes();
		}

		// render text
		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {
			if (!layer->getLayerState())
				continue;
			for (auto& entity : layer->getEntitites()) {
				
				//Skip entity not registered to this system
				if (entities.find(entity) == entities.end()) continue;

				// skip entity if is hidden
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Hidden>(entity))
					continue;
				if (NIKE_ECS_MANAGER->checkEntityComponent<Render::Text>(entity) && NIKE_ECS_MANAGER->checkEntityComponent<Transform::Transform>(entity)) {
					transformAndRenderText(entity);
				}
			}
		}

		// render particles
		// for non entity related particles


		// !TODO: jspoh move this update function
		NIKE::SysParticle::Manager::getInstance().update();

		Vector2f mouse_pos = NIKE_INPUT_SERVICE->getMouseWindowPos();
		Vector2f window_size = NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize();
		Vector2f mouse_particle_pos = { mouse_pos.x, window_size.y - mouse_pos.y };

		using namespace NIKE::SysParticle;
		//NIKE_RENDER_SERVICE->renderParticleSystem("cluster", mouse_particle_pos);

		auto& PM = NIKE::SysParticle::Manager::getInstance();
		for (const auto& ps : PM.getActiveParticleSystems()) {
			const unsigned int vao = PM.getVAO(ps.preset);
			const unsigned int vbo = PM.getVBO(ps.preset);

			std::vector<Particle> particles = ps.particles;

			// assume world pos
			if (ps.using_world_pos) {
				std::for_each(particles.begin(), particles.end(), [&](Particle& p) {
					p.pos = worldToScreen(p.pos);
				});
			}

			glNamedBufferSubData(vbo, 0, particles.size() * sizeof(Particle), particles.data());

			NIKE_RENDER_SERVICE->renderParticleSystem(static_cast<int>(ps.preset), ps.origin, static_cast<int>(ps.render_type), static_cast<int>(particles.size()));
		}

		// mouse particles
		NIKE::SysParticle::Manager::getInstance().setParticleSystemOrigin("ps1", mouse_particle_pos);
		//NIKE_RENDER_SERVICE->renderParticleSystem(static_cast<int>(Data::ParticlePresets::BASE), mouse_particle_pos);


#ifndef NDEBUG
		//Unbind when editor is active for rendering
		NIKE_LVLEDITOR_SERVICE->unbindEditorFrameBuffer();
#endif

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}
}