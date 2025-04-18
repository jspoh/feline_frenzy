/*****************************************************************//**
 * \file   cTransform.cpp
 * \brief  Transform components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 *  * All content 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
						{ "Screen Position", comp.use_screen_pos },
						{ "Scale", comp.scale.toJson() },
						{ "Rotation", comp.rotation }
				};
			},

			//Deserialize
			[](Transform& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.value("Position", Vector2f::def_json));
				comp.scale.fromJson(data.value("Scale", Vector2f::def_json));
				comp.rotation = data.value("Rotation", 0.0f);
				comp.use_screen_pos = data.value("Screen Position", false);
			},

			//Override Serialize
			[](Transform const& comp, Transform const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.position != other_comp.position) {
					delta["Position"] = comp.position.toJson();
				}
				if (comp.use_screen_pos != other_comp.use_screen_pos) {
					delta["Screen Position"] = comp.use_screen_pos;
				}
				if (comp.scale != other_comp.scale) {
					delta["Scale"] = comp.scale.toJson();
				}
				if (comp.rotation != other_comp.rotation) {
					delta["Rotation"] = comp.rotation;
				}

				return delta;
			},

			//Override Deserialize
			[](Transform& comp, nlohmann::json const& delta) {
				if (delta.contains("Position")) {
					comp.position.fromJson(delta.value("Position", Vector2f::def_json));
				}
				if (delta.contains("Scale")) {
					comp.scale.fromJson(delta.value("Scale", Vector2f::def_json));
				}
				if (delta.contains("Rotation")) {
					comp.rotation = delta.value("Rotation", 0.0f);
				}
				if (delta.contains("Screen Position")) {
					comp.use_screen_pos = delta.value("Screen Position", false);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Transform>();
	}

	void Transform::registerEditorComponents() {
#ifndef NDEBUG
		//Register transform for level editor UI
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Transform>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Transform& comp) {

				//Transform text
				ImGui::Text("Edit Transform:");

				//Edit Position
				{
					//Position before change
					static Vector2f pos_before_change;

					//Drag position
					ImGui::DragFloat2("Position##Transform", &comp.position.x, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						pos_before_change = comp.position;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {

						//Check for grid snapping
						if (comp_panel.checkGridSnapping()) {
							//Get snapped to cell position
							auto cursor_cell = NIKE_MAP_SERVICE->getCellAtPosition(comp.position);
							if (cursor_cell.has_value()) {

								//Snap to cell
								comp.position = cursor_cell.value().get().position;
							}
						}

						//Apply action
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
					ImGui::DragFloat2("Scale##Transform", &comp.scale.x, 0.1f, 0.f, (float)UINT16_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

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
					ImGui::DragFloat("Rotation##Transform", &comp.rotation, 0.1f, -360.f, 360.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

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

				ImGui::Checkbox("Fixed To Screen Position##", &comp.use_screen_pos);
			}
		);
#endif
	}
}