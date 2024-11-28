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
					static float before_change_layer;

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
					ImGui::Text("Enter Your Script:");
					if (ImGui::InputText("##ScriptInput", script_input.data(), script_input.capacity() + 1)) {
						script_input.resize(strlen(script_input.c_str()));
					}

					ImGui::SameLine();

					//Save Button
					if (ImGui::Button("Save##Script")) {
						std::filesystem::path script_path = NIKE_LUA_SERVICE->getScriptPath(script_input);
						if (!script_path.empty() && NIKE_LUA_SERVICE->checkScriptFileExist(script_input))
						{
							comp.script.script_path = script_path.string();
							comp.script.function = NIKE_LUA_SERVICE->extractFunctionFromScript(script_input);
							LevelEditor::Action save_script_id;

							//Save script id action
							save_script_id.do_action = [&, script = script_input]() {
								comp.script.script_id = script;
								script_input = comp.script.script_id;
								};

							//Undo save script id action
							save_script_id.undo_action = [&, script = comp.script.script_id]() {
								comp.script.script_id = script;
								script_input = comp.script.script_id;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_script_id));
							comp_panel.setPopUpSuccessMsg("Script Saved successfully");
							comp_panel.openPopUp("Success");

						}
						else {
							comp_panel.setPopUpErrorMsg("Script Does Not Exist!");
							comp_panel.openPopUp("Error");
							script_input = comp.script.script_id;
						}
					}
				}

				// Show the extracted function input
				ImGui::Text("Function: %s", comp.script.function.c_str());

				// !TODO: Change this to input
				ImGui::Text("Script Path: %s", comp.script.script_path.c_str());
				ImGui::Text("Function: %s", comp.script.function.c_str());
				ImGui::Text("Script Loaded: %s", comp.script.b_loaded ? "Yes" : "No");
				ImGui::Text("Last Shot time: %f", comp.last_shot_time);
			}
		);
	}
}