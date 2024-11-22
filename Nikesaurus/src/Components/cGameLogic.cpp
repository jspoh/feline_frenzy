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
					ImGui::Text("Enter Your Script:");
					if (ImGui::InputText("##ScriptInput", script_input.data(), script_input.capacity() + 1)) {
						script_input.resize(strlen(script_input.c_str()));
					}

					ImGui::SameLine();

					//Save Button
					if (ImGui::Button("Save##Script")) {
						if (NIKE_LUA_SERVICE->checkScriptFileExist(script_input))
						{
							comp.script.function = NIKE_LUA_SERVICE->extractFunctionFromScript(script_input);
							NIKE_LUA_SERVICE->executeScript(script_input, comp.script.function);
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
			}
		);
	}
}