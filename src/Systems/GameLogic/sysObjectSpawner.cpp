/*****************************************************************//**
 * \file   sysAnimationController.cpp
 * \brief  animation controller system
 * 
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "../headers/Core/stdafx.h"
#include "../headers/Systems/GameLogic/sysObjectSpawner.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cRender.h"
#include "../headers/Components/cInput.h"
#include "../headers/Components/cTransform.h"

void ObjectSpawner::Manager::generateRandomObject() {
	Entity::Type new_entity = NIKEEngine.createEntity();

	//Calculate random pos, size & color
	Vector2 win_size{ NIKEEngine.accessWindow()->getWindowSize() };
	Vector3 color(Utility::randFloat(), Utility::randFloat(), Utility::randFloat());
	Vector2 pos(Utility::randFloat() * (win_size.x / 2.0f), Utility::randFloat() * (win_size.y / 2.0f));
	Vector2 size(Utility::randFloat() * (win_size.x / 5.0f), Utility::randFloat() * (win_size.y / 5.0f));
	pos.x = Utility::randFloat() <= 0.5 ? -pos.x : pos.x;
	pos.y = Utility::randFloat() <= 0.5 ? -pos.y : pos.y;

	//Generate random component
	NIKEEngine.addEntityComponentObj<Transform::Transform>(new_entity, { {pos}, {size}, Utility::randFloat() });

	if (Utility::randFloat() <= 0.5) {
		NIKEEngine.addEntityComponentObj<Render::Shape>(new_entity, { "square", Matrix33::Matrix_33::Identity(), {color, 1.0f} });
	}
	else {
		NIKEEngine.addEntityComponentObj<Render::Shape>(new_entity, { "triangle", Matrix33::Matrix_33::Identity(), {color, 1.0f} });
	}
}

void ObjectSpawner::Manager::init() {

}

bool ObjectSpawner::Manager::update() {

	//Loop through entities
	for (auto& entity : entities) {
		if (NIKEEngine.checkEntityComponent<ObjectSpawner::Spawn>(entity)) {
			auto& e_spawn = NIKEEngine.getEntityComponent<ObjectSpawner::Spawn>(entity);

			//Input checks
			if (e_spawn.mouse_type == -1) {
				//Assert if key not present
				assert(NIKEEngine.checkEntityComponent<Input::Key>(entity) && "Key animate entity created without key object.");
				auto& e_key = NIKEEngine.getEntityComponent<Input::Key>(entity);

				//Check for input
				if (e_key.b_output && e_key.key_type == e_spawn.key_type) {
					for (int i = 0; i < e_spawn.count; i++) {
						if (NIKEEngine.getEntitiesCount() == Entity::MAX)
							break;

						generateRandomObject();
					}
				}
			}
			if (e_spawn.key_type == -1) {
				//Assert if mouse not present
				assert(NIKEEngine.checkEntityComponent<Input::Mouse>(entity) && "Mouse animate entity created without key object.");
				auto& e_mouse = NIKEEngine.getEntityComponent<Input::Mouse>(entity);

				//Check for input
				if (e_mouse.b_output && e_mouse.button_type == e_spawn.mouse_type) {
					for (int i = 0; i < e_spawn.count; i++) {
						if (NIKEEngine.getEntitiesCount() == Entity::MAX)
							break;

						generateRandomObject();
					}
				}
			}
		}
	}

	//Disable debug mode
	NIKEEngine.accessSystem<Render::Manager>()->debug_mode = false;

	return false;
}
