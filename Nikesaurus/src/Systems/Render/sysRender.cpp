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


#define M_PI_3 (M_PI / 3.f)
#define M_PI_6 (M_PI / 6.f)



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
#endif

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

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

					// recalculate offset if child is a gun
					const std::string child_prefab = NIKE_METADATA_SERVICE->getEntityPrefabID(child_entity);
					if (child_prefab == "gun_enemy_n.prefab") {

						float angle_rad = M_PI + 1;		// angle from enemy to player

						auto getGunOffset = [&]() {
							// get player world pos
							auto opt_player_entity = NIKE_METADATA_SERVICE->getEntityByName("player");

							if (!opt_player_entity.has_value()) {
								NIKEE_CORE_ERROR("Player entity not found");
								return Vector2f{ 0, 0 };
							}

							Entity::Type player_entity = opt_player_entity.value();

							// get player transform pos
							auto comps = NIKE_ECS_MANAGER->getAllEntityComponents(player_entity);
							auto transform_comp = comps.find(Utility::convertTypeString(typeid(Transform::Transform).name()));
							if (transform_comp == comps.end()) {
								NIKEE_CORE_ERROR("Player entity missing Transform component");
								return Vector2f{ 0, 0 };
							}
							Transform::Transform& player_transform = *std::static_pointer_cast<Transform::Transform>(transform_comp->second);
							const Vector2f player_pos = player_transform.position;

							// get current enemy (gun's parent) pos
							if (!parent_entity.has_value()) {
								return Vector2f{ 0, 0 };
							}

							comps = NIKE_ECS_MANAGER->getAllEntityComponents(parent_entity.value());
							transform_comp = comps.find(Utility::convertTypeString(typeid(Transform::Transform).name()));
							if (transform_comp == comps.end()) {
								NIKEE_CORE_ERROR("Parent entity missing Transform component");
								return Vector2f{ 0, 0 };
							}
							Transform::Transform& parent_transform = *std::static_pointer_cast<Transform::Transform>(transform_comp->second);
							const Vector2f parent_pos = parent_transform.position;

							// calculate angle between enemy and player
							const Vector2f enemy_to_player = parent_pos - player_pos;
							angle_rad = -atan2(enemy_to_player.y, enemy_to_player.x);

							static constexpr float GUN_DISTANCE = 110.f;

							// calculate offset for closest point on bounding circle with radius GUM_DISTANCE from enemy to player
							const Vector2f gun_offset = Vector2f{ -cos(angle_rad), sin(angle_rad) } *GUN_DISTANCE;

							// modify child transform values
							return gun_offset;
							};

						const Vector2f gun_offset = getGunOffset();

						if (c_trans.has_value()) {
							c_trans.value().get().position = gun_offset;
						}

						// get gun entity components
						auto gun_comps = NIKE_ECS_MANAGER->getAllEntityComponents(child_entity);
						auto gun_comp_animation_sprite = gun_comps.find(Utility::convertTypeString(typeid(Animation::Sprite).name()));
						auto gun_comp_texture = gun_comps.find(Utility::convertTypeString(typeid(Render::Texture).name()));
						if (gun_comp_animation_sprite != gun_comps.end() && c_trans.has_value() && gun_comp_texture != gun_comps.end()) {
							Animation::Sprite& gun_sprite = *std::static_pointer_cast<Animation::Sprite>(gun_comp_animation_sprite->second);
							Render::Texture& gun_texture = *std::static_pointer_cast<Render::Texture>(gun_comp_texture->second);

							// update spritesheet used based on section
							// circle will be divided into 8 sections, with a focus on nsew
							cout << angle_rad << endl;
							if (angle_rad > M_PI_3 && angle_rad < M_PI - M_PI_3) {
								// up
								cout << "top" << endl;
								gun_sprite.start_index.y = 3;
								gun_sprite.end_index.y = 3;
								gun_texture.b_flip.x = false;
								c_trans.value().get().rotation = 0.f;
							}
							else if (angle_rad > -2 * M_PI_3&& angle_rad < -M_PI_3) {
								// down
								cout << "bottom" << endl;
								gun_sprite.start_index.y = 0;
								gun_sprite.end_index.y = 0;
								gun_texture.b_flip.x = false;
								c_trans.value().get().rotation = 0.f;
							}
							else if (angle_rad > -M_PI_6 && angle_rad < M_PI_6) {
								cout << "left" << endl;
								gun_texture.b_flip.x = true;

								gun_sprite.start_index.y = 3;
								gun_sprite.end_index.y = 3;

								c_trans.value().get().rotation = 90.f;
							}
							else if (angle_rad > 5 * M_PI_6 || angle_rad < -5 * M_PI_6) {
								// right
								cout << "right" << endl;
								gun_texture.b_flip.x = false;

								gun_sprite.start_index.y = 3;
								gun_sprite.end_index.y = 3;
								
								c_trans.value().get().rotation = 270.f;
							}
							else if (angle_rad > M_PI_6 && angle_rad < M_PI_3) {
								cout << "top left" << endl;
							}
							else if (angle_rad > 2 * M_PI_3 && angle_rad < 5 * M_PI_6) {
								cout << "top right" << endl;;
							}
							else if (angle_rad > -M_PI_3 && angle_rad < - M_PI_6) {
								cout << "bottom left" << endl;
							}
							else if (angle_rad > -5 * M_PI_6 && angle_rad < -2 * M_PI_3) {
								cout << "bottom right" << endl;
							}
							

							//else if (angle_rad > M_PI_4 && angle_rad < M_PI - M_PI_4) {
							//	// down
							//	cout << "// down" << endl;
							//	gun_sprite.start_index.y = 0;
							//	gun_sprite.end_index.y = 0;
							//}
							//else if (angle_rad > 0 && angle_rad < M_PI_4) {
							//	// bottom left 
							//	cout << "bottom left " << endl;
							//	gun_sprite.start_index.y = 1;
							//	gun_sprite.end_index.y = 1;
							//	// flip texture
							//	gun_texture.b_flip.x = true;
							//}
							//else if (angle_rad > M_PI - M_PI_4 && angle_rad < M_PI) {
							//	// bottom right 
							//	cout << "bottom right " << endl;
							//	gun_sprite.start_index.y = 1;
							//	gun_sprite.end_index.y = 1;
							//	gun_texture.b_flip.x = false;
							//}
							//else if (angle_rad < 0 && angle_rad > -M_PI_4) {
							//	// top left
							//	cout << "top left" << endl;
							//	gun_sprite.start_index.y = 2;
							//	gun_sprite.end_index.y = 2;
							//	// flip texture
							//	gun_texture.b_flip.x = true;
							//}
							//else if (angle_rad < -M_PI + M_PI_4 && angle_rad > -M_PI_4) {
							//	// top right
							//	cout << "top right" << endl;
							//	gun_sprite.start_index.y = 2;
							//	gun_sprite.end_index.y = 2;
							//	gun_texture.b_flip.x = false;
							//}
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
			static bool show_fps = true;
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
#endif

		err = glGetError();
		if (err != GL_NO_ERROR) {
			NIKEE_CORE_ERROR("OpenGL error at end of {0}: {1}", __FUNCTION__, err);
		}
	}
}