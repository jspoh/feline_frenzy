/*****************************************************************//**
 * \file   sysGameLogic.cpp
 * \brief  game logic system
 *
 * \author ho
 * \date   September 2024
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/sysGameLogic.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cInput.h"
#include "../headers/Components/cScene.h"
#include "../headers/Managers/mEvents.h"

void GameLogic::Manager::init() {

}

void GameLogic::Manager::update() {

	//Loop through entities
	for (auto& entity : entities) {

		//Object spawning logic
		if (NIKEEngine.checkEntityComponent<GameLogic::ObjectSpawner>(entity)) {
			auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

			if (e_mouse.b_output && (e_mouse.button_type == GLFW_MOUSE_BUTTON_RIGHT)) {
				auto camera_system = NIKEEngine.accessSystem<Render::Manager>()->getCamEntity();
				float ndcX = (2.0f * e_mouse.button_pos.x) / NIKEEngine.accessWindow()->getWindowSize().x - 1.0f;
				float ndcY = 1.0f - (2.0f * e_mouse.button_pos.y) / NIKEEngine.accessWindow()->getWindowSize().y;

				Matrix33::Matrix_33 ndc_to_world_xform;
				Matrix33::Matrix_33Inverse(ndc_to_world_xform, camera_system->getWorldToNDCXform());

				Vector3 ndc_coords = { ndcX, ndcY, 1.0f }; // NDC in homogeneous coordinates
				Vector3 world_coords = ndc_coords * ndc_to_world_xform;

				Entity::Type new_entity = NIKEEngine.createEntity();
				NIKEEngine.addEntityComponentObj<Render::Mesh>(new_entity, { "tex", "square-texture", Matrix33::Matrix_33::Identity(), "tree" });
				NIKEEngine.addEntityComponentObj<Transform::Transform>(new_entity, { {world_coords.x ,world_coords.y}, {200.f, 200.f}, 0.0f });
				NIKEEngine.addEntityComponentObj<Render::Color>(new_entity, { {1.0f, 1.0f, 1.0f}, 1.0f });
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

		//Escape program
		if (NIKEEngine.checkEntityComponent<Input::Key>(entity)) {
			auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);
			//Check if escape key is pressed
			if (e_key.b_output && (e_key.key_type == GLFW_KEY_ESCAPE)) {
				NIKEEngine.accessWindow()->terminate();
			}
		}
	}
}

