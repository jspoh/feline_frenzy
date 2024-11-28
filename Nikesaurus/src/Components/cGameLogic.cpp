/*****************************************************************//**
 * \file   cGameLogic.cpp
 * \brief	Game logic components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cGameLogic.h"

namespace NIKE {

	nlohmann::json ScriptSerialize(GameLogic::Script const& comp) {
		return	{
				{ "Script_Path", comp.script_path },
				{ "Function", comp.function },
		};
	}

	void ScriptDeserialize(GameLogic::Script& comp, nlohmann::json const& data) {
		comp.script_path = data.at("Script_Path").get<std::string>();
		comp.function = data.at("Function").get<std::string>();
	}

	void GameLogic::registerComponents() {

		//Register logic components
		NIKE_ECS_MANAGER->registerComponent<GameLogic::Movement>();
		NIKE_ECS_MANAGER->registerComponent<GameLogic::StateMachine>();

		//Register Player For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<GameLogic::Movement>(
			//Serialize
			[](GameLogic::Movement const& comp) -> nlohmann::json {
				return	ScriptSerialize(comp.script);
			},

			//Deserialize
			[](GameLogic::Movement& comp, nlohmann::json const& data) {
				ScriptDeserialize(comp.script, data);
			}
		);


		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<GameLogic::Movement>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, GameLogic::Movement& comp) {

				ImGui::Text("Edit Movement Logic Component");

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
					//if (ImGui::Combo("##SelectScript", &current_index, all_loaded_scripts.data(), static_cast<int>(all_loaded_scripts.size()))) {
					//	// Validate the selected index and get the new script
					//	if (current_index >= 0 && current_index < static_cast<int>(all_loaded_scripts.size())) {
					//		std::string new_script = all_loaded_scripts[current_index];
					//		if (new_script != comp.script.script_id) {
					//			// Save action
					//			LevelEditor::Action change_script_action;
					//			change_script_action.do_action = [&, script = new_script]() {
					//				comp.script.script_id = script;
					//				comp.script.script_path = NIKE_LUA_SERVICE->getScriptPath(script).string();
					//				comp.script.function = NIKE_LUA_SERVICE->extractFunctionFromScript(script);
					//				};

					//			// Undo action
					//			change_script_action.undo_action = [&, script = previous_script_id]() {
					//				comp.script.script_id = script;
					//				comp.script.script_path = NIKE_LUA_SERVICE->getScriptPath(script).string();
					//				comp.script.function = NIKE_LUA_SERVICE->extractFunctionFromScript(script);
					//				};

					//			// Execute the action
					//			NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_script_action));

					//			// Update the previous script id
					//			previous_script_id = new_script;
					//		}
					//	}
					//}
				}

				// Show the extracted function input
				ImGui::Text("Function: %s", comp.script.function.c_str());
			}
		);
	}
}