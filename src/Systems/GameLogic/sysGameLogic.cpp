/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 *
 * \author ho
 * \co-author Sean Gwee, g.boonxuensean@digipen.edu
 * \co-author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysGameLogic.h"

void GameLogic::Manager::init() {

}

void GameLogic::Manager::update() {

	//Loop through entities
	for (auto& entity : entities) {
		//Object spawning logic
		if (NIKEEngine.checkEntityComponent<GameLogic::ObjectSpawner>(entity)) {
			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

			if (e_mouse.b_output && (e_mouse.button_type == GLFW_MOUSE_BUTTON_LEFT)) {
				auto& camera_system = NIKEEngine.accessSystem<Render::Manager>()->getCamEntity();
				float ndcX = (2.0f * e_mouse.button_pos.x) / NIKEEngine.accessWindow()->getWindowSize().x - 1.0f;
				float ndcY = 1.0f - (2.0f * e_mouse.button_pos.y) / NIKEEngine.accessWindow()->getWindowSize().y;

				Matrix33::Matrix_33 ndc_to_world_xform;
				Matrix33::Matrix_33Inverse(ndc_to_world_xform, camera_system->getWorldToNDCXform());

				Vector3 ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
				Vector3 world_coords = ndc_coords * ndc_to_world_xform;

				// Render start from 3
				Entity::Type new_entity = NIKEEngine.cloneEntity(5 + std::rand() % 2);
				NIKEEngine.getEntityComponent<Transform::Transform>(new_entity).position = { world_coords.x ,world_coords.y };
			}
		}

		//Changing scene to menu
		if (NIKEEngine.checkEntityComponent<Scenes::ChangeSceneEvent>(entity)) {
			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);
			if (e_mouse.b_output && (e_mouse.button_type == GLFW_MOUSE_BUTTON_LEFT)) {
				auto menu_event = std::make_shared<Scenes::ChangeSceneEvent>(Scenes::Actions::CHANGE, "MENU");
				NIKEEngine.accessEvents()->dispatchEvent(menu_event);
			}
		}

		/***********************************************
		* Check your keypresses here!!!
		************************************************/
		if (NIKEEngine.checkEntityComponent<Input::Key>(entity)) {
			auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);
			//Check if escape key is pressed
			//Escape program
			if (e_key.b_output && (e_key.key_type == GLFW_KEY_ESCAPE)) {
				NIKEEngine.accessWindow()->terminate();
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_C))
			{
				try
				{
					throw std::runtime_error("crash");
				}
				catch (const std::exception&)
				{
					NIKEEngine.accessDebug()->logCrash();
					NIKEEngine.accessWindow()->terminate();
				}
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_T)) {
				NIKEEngine.accessSystem<Render::Manager>()->debug_mode = !NIKEEngine.accessSystem<Render::Manager>()->debug_mode;;
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_P))
			{
				NIKEEngine.accessSystem<Audio::Manager>()->NEAudioStopGroup(NIKEEngine.accessAssets()->getAudioGroup("test_group"));
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_K))
			{
				auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::END, "AME-ANIMATOR");
				NIKEEngine.accessEvents()->dispatchEvent(animation_event);
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_L))
			{
				auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESTART, "AME-ANIMATOR");
				NIKEEngine.accessEvents()->dispatchEvent(animation_event);
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_I))
			{
				auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::PAUSE, "AME-ANIMATOR");
				NIKEEngine.accessEvents()->dispatchEvent(animation_event);
			}

			if (e_key.b_output && (e_key.key_type == GLFW_KEY_J))
			{
				auto animation_event = std::make_shared<Animation::AnimationEvent>(Animation::Mode::RESUME, "AME-ANIMATOR");
				NIKEEngine.accessEvents()->dispatchEvent(animation_event);
			}

			// Input for Player
			if (NIKEEngine.checkEntityComponent<Move::Movement>(entity) && NIKEEngine.checkEntityComponent<Collision::Collider>(entity)) {
				const float movespeed = 300.0f;
				Transform::Velocity& velocity = NIKEEngine.getEntityComponent<Transform::Velocity>(entity);
				Collision::Collider& collider = NIKEEngine.getEntityComponent<Collision::Collider>(entity);

				velocity.velocity.x = 0;
				velocity.velocity.y = 0;

				// Up
				if (e_key.b_output && (e_key.key_type == GLFW_KEY_W) && !collider.top) {
					velocity.velocity.y += movespeed;
				}
				// Left
				if (e_key.b_output && (e_key.key_type == GLFW_KEY_A) && !collider.left) {
					velocity.velocity.x -= movespeed;
				}
				// Down
				if (e_key.b_output && (e_key.key_type == GLFW_KEY_S) && !collider.bottom) {
					velocity.velocity.y -= movespeed;
				}
				// Right
				if (e_key.b_output && (e_key.key_type == GLFW_KEY_D) && !collider.right) {
					velocity.velocity.x += movespeed;
				}
			}
		}
	}
}

