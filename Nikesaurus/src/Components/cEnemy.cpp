/*****************************************************************//**
 * \file   cEnemy.cpp
 * \brief  Enemy components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cEnemy.h"

namespace NIKE {
	void Enemy::registerComponents() {
		// Register attack components
		NIKE_ECS_MANAGER->registerComponent<Attack>();

		// Register attack for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Attack>(
			// Serialize
			[](Attack const& comp) -> nlohmann::json {
				return {
					{ "Range" , comp.range },
					{ "Cooldown", comp.cooldown },
					{ "LastShotTime", comp.last_shot_time },
					{ "Offset", comp.offset },
					{ "Layer", comp.layer },
					{ "PrefabPath", comp.prefab_path},
					{ "ScriptID", comp.script.script_id },
					{ "ScriptPath", comp.script.script_path },
					{ "Function", comp.script.function },
					{ "ScriptLoaded", comp.script.b_loaded },
				};
			},

			// Deserialize
			[](Attack& comp, nlohmann::json const& data) {
				comp.range = data.at("Range").get<float>();
				comp.cooldown = data.at("Cooldown").get<float>();
				comp.last_shot_time = data.at("LastShotTime").get<float>();
				comp.offset = data.at("Offset").get<float>();
				comp.layer = data.at("Layer").get<int>();
				comp.prefab_path = data.at("PrefabPath").get<std::string>();
				comp.script.script_id = data.at("ScriptID").get<std::string>();
				comp.script.script_path = data.at("ScriptPath").get<std::string>();
				comp.script.function = data.at("Function").get<std::string>();
				comp.script.b_loaded = data.at("ScriptLoaded").get<bool>();
			}
		);

#ifndef NDEBUG
		// Level Editor UI registration
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Attack>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Attack& comp) {

				ImGui::Text("Edit Shooting Variables:");

				// For attack range
				{
					static float before_change_range;

					ImGui::DragFloat("Range", &comp.range, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_range = comp.range;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_range;

						// Change do action
						change_range.do_action = [&, range = comp.range]() {
							comp.range = range;
							};

						// Change undo action
						change_range.undo_action = [&, range = before_change_range]() {
							comp.range = range;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_range));
					}
				}

				// For shooting cooldown
				{
					static float before_change_cooldown;

					ImGui::DragFloat("Shot Cooldown", &comp.cooldown, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_cooldown = comp.cooldown;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_cooldown;

						// Change do action
						change_cooldown.do_action = [&, cooldown = comp.cooldown]() {
							comp.cooldown = cooldown;
							};

						// Change undo action
						change_cooldown.undo_action = [&, cooldown = before_change_cooldown]() {
							comp.cooldown = cooldown;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_cooldown));
					}
				}

				// Show and allow editing of the offset
				{
					static float before_change_offset;

					ImGui::DragFloat("Shot Offset", &comp.offset, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_offset = comp.offset;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_offset;

						// Change do action
						change_offset.do_action = [&, offset = comp.offset]() {
							comp.offset = offset;
							};

						// Change undo action
						change_offset.undo_action = [&, offset = before_change_offset]() {
							comp.offset = offset;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_offset));
					}
				}

				// Show and allow editing of the layer
				{
					static int before_change_layer;

					ImGui::DragInt("Layer", &comp.layer, 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_layer = comp.layer;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_layer;

						// Change do action
						change_layer.do_action = [&, layer = comp.layer]() {
							comp.layer = layer;
							};

						// Change undo action
						change_layer.undo_action = [&, layer = before_change_layer]() {
							comp.layer = layer;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_layer));
					}
				}

				// !TODO: Change this to input
				ImGui::Text("Prefab Path: %s", comp.prefab_path.c_str());
				ImGui::Text("Script Path: %s", comp.script.script_path.c_str());
				ImGui::Text("Function: %s", comp.script.function.c_str());
				ImGui::Text("Script Loaded: %s", comp.script.b_loaded ? "Yes" : "No");
				ImGui::Text("Last Shot time: %f", comp.last_shot_time);
			}
		);
#endif
	}
}