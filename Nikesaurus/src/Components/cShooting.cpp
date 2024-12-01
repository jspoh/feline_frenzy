/*****************************************************************//**
 * \file   cShooting.cpp
 * \brief  Shooting components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cShooting.h"

namespace NIKE {
	void Shooting::registerComponents() {
		// Register transform components
		NIKE_ECS_MANAGER->registerComponent<Shooting>();

		// Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Shooting>(
			// Serialize
			[](Shooting const& comp) -> nlohmann::json {
				return {
					{ "Cooldown", comp.cooldown },
					{ "LastShotTime", comp.last_shot_time },
					{ "Offset", comp.offset },
					{ "Layer", comp.layer },
					{ "ScriptID", comp.script.script_id }, 
					{ "ScriptPath", comp.script.script_path },
					{ "Function", comp.script.function },
					{ "ScriptLoaded", comp.script.b_loaded }
				};
			},

			// Deserialize
			[](Shooting& comp, nlohmann::json const& data) {
				comp.cooldown = data.at("Cooldown").get<float>();
				comp.last_shot_time = data.at("LastShotTime").get<float>();
				comp.offset = data.at("Offset").get<float>();
				comp.layer = data.at("Layer").get<int>();
				comp.script.script_id = data.at("ScriptID").get<std::string>();
				comp.script.script_path = data.at("ScriptPath").get<std::string>();
				comp.script.function = data.at("Function").get<std::string>();
				comp.script.b_loaded = data.at("ScriptLoaded").get<bool>();
			}
		);

#ifndef NDEBUG
		// Level Editor UI registration
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Shooting>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Shooting& comp) {

				ImGui::Text("Edit Shooting Variables:");

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

				// Static vars for string input mangement
				static std::string script_input;

				if (ImGui::IsItemActivated() || comp_panel.isEntityChanged()) {
					script_input = comp.script.script_id;
				}


				// For script path input
				{
					// Display a combo box for selecting script
					ImGui::Text("Select Script:");

					// Hold the current and previous script path
					static std::string previous_script_id = comp.script.script_id;
					std::string current_script_id = comp.script.script_id;

					// Get all loaded scripts
					const auto& all_loaded_scripts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Script);

					// Find the index of the currently selected script in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_scripts.size(); ++i) {
						if (current_script_id == all_loaded_scripts[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for script selection
					if (ImGui::Combo("##SelectScript", &current_index, all_loaded_scripts.data(), static_cast<int>(all_loaded_scripts.size()))) {
						// Validate the selected index and get the new script
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_scripts.size())) {
							std::string new_script = all_loaded_scripts[current_index];

							if (new_script != comp.script.script_id) {
								// Save action
								LevelEditor::Action change_script_action;
								change_script_action.do_action = [&, script = new_script]() {
									NIKE_LUA_SERVICE->loadScript(new_script);
									comp.script.script_id = script;
									comp.script.script_path = NIKE_ASSETS_SERVICE->getAssetPath(script).string();
									comp.script.function = NIKE_LUA_SERVICE->getScriptFunctions(script)[0];	// TEMP FIX
									};

								// Undo action
								change_script_action.undo_action = [&, script = previous_script_id]() {
									NIKE_LUA_SERVICE->loadScript(new_script);
									comp.script.script_id = script;
									comp.script.script_path = NIKE_ASSETS_SERVICE->getAssetPath(script).string();
									comp.script.function = NIKE_LUA_SERVICE->getScriptFunctions(script)[0];	// TEMP FIX
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_script_action));

								// Update the previous script id
								previous_script_id = new_script;
							}
						}
					}
				}

				// Show the extracted function input
				ImGui::Text("Function: %s", comp.script.function.c_str());

				// !TODO: Change this to input
				ImGui::Text("Prefab: %s", comp.prefab_path.c_str());
				ImGui::Text("Script Path: %s", comp.script.script_path.c_str());
				ImGui::Text("Function: %s", comp.script.function.c_str());
				ImGui::Text("Script Loaded: %s", comp.script.b_loaded ? "Yes" : "No");
				ImGui::Text("Last Shot time: %f", comp.last_shot_time);
			}
		);
#endif
	}
}