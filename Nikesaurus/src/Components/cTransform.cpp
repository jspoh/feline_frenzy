/*****************************************************************//**
 * \file   cTransform.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cTransform.h"

namespace NIKE {
	void Transform::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Transform>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Transform>(
			//Serialize
			[](Transform const& comp) -> nlohmann::json {
				return	{
						{ "Position", comp.position.toJson() },
						{ "Scale", comp.scale.toJson() },
						{ "Rotation", comp.rotation }
				};
			},

			//Deserialize
			[](Transform& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.at("Position"));
				comp.scale.fromJson(data.at("Scale"));
				comp.rotation = data.at("Rotation").get<float>();
			}
		);

		//Register transform for level editor UI
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Transform>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Transform& comp) {

				//Transform text
				ImGui::Text("Edit Transform Variables:");

				//Edit Position
				{
					//Position before change
					static Vector2f pos_before_change;

					//Drag position
					ImGui::DragFloat2("Position", &comp.position.x, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						pos_before_change = comp.position;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_pos;

						//Change pos do action
						change_pos.do_action = [&, pos = comp.position]() {
							comp.position = pos;
							};

						//Change pos undo action
						change_pos.undo_action = [&, pos = pos_before_change]() {
							comp.position = pos;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_pos));
					}
				}

				//Edit Scale
				{
					//Scale before change
					static Vector2f scale_before_change;

					//Change scale
					ImGui::DragFloat2("Scale", &comp.scale.x, 0.1f);

					//Check if scale has beguin editing
					if (ImGui::IsItemActivated()) {
						scale_before_change = comp.scale;
					}

					//Check if scale has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_scale;

						//Change scale do action
						change_scale.do_action = [&, scale = comp.scale]() {
							comp.scale = scale;
							};

						//Change scale undo action
						change_scale.undo_action = [&, scale = scale_before_change]() {
							comp.scale = scale;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_scale));
					}
				}

				//Edit Rotation
				{
					//Rotation before change
					static float rotation_before_change;

					//Change rotation
					ImGui::DragFloat("Rotation", &comp.rotation, 0.1f, -360.f, 360.f);

					//Check if rotation has begun editing
					if (ImGui::IsItemActivated()) {
						rotation_before_change = comp.rotation;
					}

					//Check if rotation has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_rotation;

						//Change rotation do action
						change_rotation.do_action = [&, rotation = comp.rotation]() {
							comp.rotation = rotation;
							};

						//Change rotation undo action
						change_rotation.undo_action = [&, rotation = rotation_before_change]() {
							comp.rotation = rotation;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_rotation));
					}
				}
			}
		);
	}
}