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


#define M_PI_3 1.0471975512
#define M_PI_6 0.52359877559



namespace NIKE {

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
#else
		glBindFramebuffer(GL_FRAMEBUFFER, NIKE_RENDER_SERVICE->getFbo());
#endif

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& layer : NIKE_SCENES_SERVICE->getLayers()) {

			//SKip inactive layer
			if (!layer->getLayerState())
				continue;
			if (layer->getLayerYSort()) {
				layer->sortEntitiesBasedOnYPosition();
			}

			for (auto& entity : layer->getEntitites()) {

				//Skip entity not registered to this system
				if (entities.find(entity) == entities.end()) continue;

				//Get relation
				auto relation = NIKE_METADATA_SERVICE->getEntityRelation(entity);

				//Storage of previous
				std::optional<Vector2f> child_pos_offset = std::nullopt;

				//Check if its a child relation
				auto* child = std::get_if<MetaData::Child>(&relation);
				if (child) {

					//Get child & parent entity
					auto child_entity = entity;
					auto parent_entity = NIKE_METADATA_SERVICE->getEntityByName(child->parent);

					//child transform
					auto c_trans = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(child_entity);

					// handle gun
					const std::string child_prefab = NIKE_METADATA_SERVICE->getEntityPrefabID(child_entity);
					if (child_prefab == "gun_enemy_n.prefab" || child_prefab == "gun_enemy_n_2.prefab") {

						auto getGunTransformData = [&]() -> std::optional<std::pair<Vector2f, float>> {
							auto opt_player_entity = NIKE_METADATA_SERVICE->getEntityByName("player");
							if (!opt_player_entity.has_value()) {
								NIKEE_CORE_ERROR("Player entity not found");
								return std::nullopt;
							}

							Entity::Type player_entity = opt_player_entity.value();
							auto player_comps = NIKE_ECS_MANAGER->getAllEntityComponents(player_entity);
							auto player_transform_comp = player_comps.find(Utility::convertTypeString(typeid(Transform::Transform).name()));
							if (player_transform_comp == player_comps.end()) {
								NIKEE_CORE_ERROR("Player missing Transform");
								return std::nullopt;
							}
							const Vector2f player_pos = std::static_pointer_cast<Transform::Transform>(player_transform_comp->second)->position;

							if (!parent_entity.has_value()) return std::nullopt;

							auto parent_comps = NIKE_ECS_MANAGER->getAllEntityComponents(parent_entity.value());
							auto parent_transform_comp = parent_comps.find(Utility::convertTypeString(typeid(Transform::Transform).name()));
							if (parent_transform_comp == parent_comps.end()) {
								NIKEE_CORE_ERROR("Parent missing Transform");
								return std::nullopt;
							}
							const Vector2f parent_pos = std::static_pointer_cast<Transform::Transform>(parent_transform_comp->second)->position;

							Vector2f dir = player_pos - parent_pos;
							float angle_rad = std::atan2(dir.y, dir.x); // Angle in radians

							static constexpr float GUN_DISTANCE = 75.f;
							Vector2f offset = Vector2f{ std::cos(angle_rad), std::sin(angle_rad) } *GUN_DISTANCE;

							return std::make_pair(offset, angle_rad);
						};

						auto gun_data = getGunTransformData();
						if (gun_data.has_value() && c_trans.has_value()) {
							auto& c_transform = c_trans.value().get();
							
							Vector2f pivot_offset = Vector2f(0.0f, -20.0f);  // Modify this to your desired pivot offset

							// Move gun to position around the pivot
							c_transform.position = gun_data->first + pivot_offset;

							// Set rotation in degrees
							const float angle_deg = (gun_data->second * 180.f / (float)M_PI) + 90.f;
							c_transform.rotation = angle_deg;

							// Get gun components
							auto gun_comps = NIKE_ECS_MANAGER->getAllEntityComponents(child_entity);

							auto sprite_comp_it = gun_comps.find(Utility::convertTypeString(typeid(Animation::Sprite).name()));
							if (sprite_comp_it != gun_comps.end()) {
								Animation::Sprite& gun_sprite = *std::static_pointer_cast<Animation::Sprite>(sprite_comp_it->second);

								gun_sprite.start_index.y = 0;
								gun_sprite.end_index.y = 0;

							}
						}
					}

					if (c_trans.has_value() && parent_entity.has_value()) {

						//Get child transform reference
						auto& c_transform = c_trans.value().get();

						//Get parent transform
						auto p_trans = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(parent_entity.value());
						if (p_trans.has_value()) {

							//Get parent transform reference
							auto const& p_transform = p_trans.value().get();

							//Old child transform
							child_pos_offset = c_transform.position;

							//Update child entity with parents transform data
							c_transform.use_screen_pos = p_transform.use_screen_pos;
							c_transform.position = p_transform.position + c_transform.position;
						}
					}
				}

#ifndef NDEBUG
				//Render call for all entity
				NIKE_RENDER_SERVICE->renderComponents(NIKE_ECS_MANAGER->getAllEntityComponents(entity), NIKE_LVLEDITOR_SERVICE->getDebugState());
#endif
#ifdef NDEBUG
				//Render call for all entity
				NIKE_RENDER_SERVICE->renderComponents(NIKE_ECS_MANAGER->getAllEntityComponents(entity), false);
#endif

				//Update old child position offset
				if (child_pos_offset.has_value()) {
					auto c_trans = NIKE_ECS_MANAGER->getEntityComponent<Transform::Transform>(entity);
					if (c_trans.has_value()) {

						//Get child transform reference
						auto& c_transform = c_trans.value().get();

						//update old child position offset back
						c_transform.position = child_pos_offset.value();
					}
				}
			}
		}

		//Render custom cursor
		NIKE_RENDER_SERVICE->renderCursor(NIKE_INPUT_SERVICE->getCrosshair(), NIKE_INPUT_SERVICE->getCursorEntererd());

		//Complete rendering process
		NIKE_RENDER_SERVICE->completeRender();

		// Render FPS
		{
			//FPS Rendering variables
			static auto worldsize = NIKE_WINDOWS_SERVICE->getWindow()->getWorldSize();
			static Render::Text fps = { "Skranji-Bold.ttf", "FPS:", { 1.f, 1.f, 1.f, 1.f }, 1.0f , TextOrigin::LEFT };

			//Calculate avg FPS
			static std::vector<float> fps_history(300);
			static float elapsed_time = 0.0f;
			elapsed_time += NIKE_WINDOWS_SERVICE->getDeltaTime();
			float curr_fps = NIKE_WINDOWS_SERVICE->getCurrentFPS();
			if (fps_history.empty() || std::abs(curr_fps - fps_history.back()) < 10000) { //Ignore outlier FPS jumps of by 10000
				fps_history.push_back(curr_fps);
			}

			//Update avg fps every second
			if (elapsed_time > 1.f) {

				//Calculate average fps
				elapsed_time = 0.f;
				float sum_fps = 0.f;
				std::for_each(fps_history.begin(), fps_history.end(), [&sum_fps](float& fps) { sum_fps += fps; });
				const float avg_fps = sum_fps / fps_history.size();
				fps_history.clear();

				//Update fps
				std::stringstream ss;
				ss << "FPS: " << std::round(avg_fps);
				fps.text = ss.str();
			}

			//Render FPS Display
			Matrix_33 matrix;
			#ifndef NDEBUG
						static bool show_fps = true;
			#else
						static bool show_fps = false;
			#endif

			static Transform::Transform fps_transform = { Vector2f((worldsize.x / 2.0f) - 200.0f, (worldsize.y / 2.0f) - 30.0f), Vector2f(1.0f, 1.0f), 0.0f };
			fps_transform.use_screen_pos = true;

			//Toggle showing of FPS
			show_fps = NIKE_INPUT_SERVICE->isKeyTriggered(NIKE_KEY_F1) ? !show_fps : show_fps;
			if (show_fps) {
				NIKE_RENDER_SERVICE->transformMatrix(fps_transform, matrix, NIKE_CAMERA_SERVICE->getFixedWorldToNDCXform());
				NIKE_RENDER_SERVICE->renderText(matrix, fps);
			}
		}


#ifndef NDEBUG
		//Unbind when editor is active for rendering
		NIKE_LVLEDITOR_SERVICE->unbindEditorFrameBuffer();
#else
		glBindFramebuffer(GL_FRAMEBUFFER, NIKE_RENDER_SERVICE->getFbo());
#endif

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}
}