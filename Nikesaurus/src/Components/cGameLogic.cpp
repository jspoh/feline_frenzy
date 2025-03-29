/*****************************************************************//**
 * \file   cGameLogic.cpp
 * \brief	Game logic components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cGameLogic.h"

namespace NIKE {

	void GameLogic::registerComponents() {

		//Register logic components
		NIKE_ECS_MANAGER->registerComponent<GameLogic::StateMachine>();
		NIKE_ECS_MANAGER->registerComponent<GameLogic::Script>();

		//Register Logic comp For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<GameLogic::Script>(
			//Serialize
			[](GameLogic::Script const& comp) -> nlohmann::json {
				return {
					{"Script_ID", comp.script_id},
					{"Update_Function", comp.update_func},
					{"Configs", comp.configs}
				};
			},

			//Deserialize
			[](GameLogic::Script& comp, nlohmann::json const& data) {
				comp.script_id = data.value("Script_ID", "");
				comp.update_func = data.value("Update_Function", "");

				if (comp.script_id.empty()) return;

				//Get main script configs
				auto script_obj = NIKE_ASSETS_SERVICE->getAsset<Lua::ScriptObj>(comp.script_id);
				auto const& configs = script_obj->configs;
				comp.configs = configs;

				//Override the configs
				if (data.contains("Configs")) {
					for (const auto& [key, value] : data.at("Configs").items()) {

						if (configs.find(key) != configs.end()) {
							comp.configs[key] = value.get<LuaValue>();
						}
					}
				}

				//Init script
				NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
			},

			// Override Serialize
			[](GameLogic::Script const& comp, GameLogic::Script const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.script_id != other_comp.script_id) {
					delta["Script_ID"] = comp.script_id;
				}
				if (comp.update_func != other_comp.update_func) {
					delta["Update_Function"] = comp.update_func;
				}
				if (comp.configs != other_comp.configs) {
					delta["Configs"] = comp.configs;
				}

				return delta;
			},

			// Override Deserialize
			[](GameLogic::Script& comp, nlohmann::json const& delta) {
				comp.script_id = delta.value("Script_ID", "");
				comp.update_func = delta.value("Update_Function", "");

				if (comp.script_id.empty()) return;

				//Get main script configs
				auto script_obj = NIKE_ASSETS_SERVICE->getAsset<Lua::ScriptObj>(comp.script_id);
				auto const& configs = script_obj->configs;
				comp.configs = configs;

				//Override the configs
				if (delta.contains("Configs")) {
					for (const auto& [key, value] : delta.at("Configs").items()) {

						if (configs.find(key) != configs.end()) {
							comp.configs[key] = value.get<LuaValue>();
						}
					}
				}

				//Init script
				NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<GameLogic::Script>();
	}


	void GameLogic::registerEditorComponents() {

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<GameLogic::Script>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, GameLogic::Script& comp) {

				//Edit script component
				ImGui::Text("Edit Script Component");

				// For script path input
				{
					// Get all loaded scripts
					const auto& get_load_scripts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Script);

					// Find the index of the currently selected script id in the list
					int script_index = -1;
					for (size_t i = 0; i < get_load_scripts.size(); ++i) {
						if (comp.script_id == get_load_scripts[i]) {
							script_index = static_cast<int>(i);
							break;
						}
					}

					//Display combo box for script selection
					ImGui::Text("Component Script: ");
					if (ImGui::Combo("##ComponentScript", &script_index, get_load_scripts.data(), static_cast<int>(get_load_scripts.size()))) {
						// Validate the selected index and get the new font ID
						if (script_index >= 0 && script_index < static_cast<int>(get_load_scripts.size())) {
							comp.script_id = get_load_scripts[script_index];
							comp.configs = NIKE_ASSETS_SERVICE->getAsset<Lua::ScriptObj>(comp.script_id)->configs;
							NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
						}
					}
				}

				//Select script update function
				{
					if (!comp.script_id.empty() && NIKE_ASSETS_SERVICE->isAssetRegistered(comp.script_id)) {
						// Get all loaded script functions
						const auto& get_script_functions = NIKE_LUA_SERVICE->getTableKeys(comp.script_instance);
						std::vector<const char*> funcs;
						//Convert to const char*
						std::for_each(get_script_functions.begin(), get_script_functions.end(), [&funcs](std::string const& ref) {
							funcs.push_back(ref.c_str());
							});

						// Find the index of the currently selected update function in the list
						int update_func_index = -1;
						for (size_t i = 0; i < funcs.size(); ++i) {
							if (comp.update_func == funcs[i]) {
								update_func_index = static_cast<int>(i);
								break;
							}
						}

						// Display combo box for script update function selection
						ImGui::Text("Script Update Function: ");
						if (ImGui::Combo("##CompScriptUpdateFunc", &update_func_index, funcs.data(), static_cast<int>(funcs.size()))) {
							// Validate the selected index and set the new update function
							if (update_func_index >= 0 && update_func_index < static_cast<int>(funcs.size())) {
								comp.update_func = funcs[update_func_index];
							}
						}
					}

					ImGui::Spacing();
					ImGui::SeparatorText("Configs");
					ImGui::Spacing();

					//Set config arguments
					{
						for (auto& config : comp.configs) {

							std::visit([&, key = config.first](auto&& v) {
								using T = std::decay_t<decltype(v)>;

								if constexpr (std::is_same_v<T, int>) {
									int temp = v;
									ImGui::Text(key.c_str());
									if (ImGui::InputInt(("##Config " + key).c_str(), &temp)) {
										config.second = temp;
										NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
									}
								}
								else if constexpr (std::is_same_v<T, float>) {
									float temp = v;
									ImGui::Text(key.c_str());
									if (ImGui::InputFloat(("##Config " + key).c_str(), &temp)) {
										config.second = temp;
										NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
									}
								}
								else if constexpr (std::is_same_v<T, std::string>) {
									//Static variables for string input management
									std::string text_input = v;
									text_input.resize(64);
									ImGui::Text(key.c_str());
									if (ImGui::InputText(("##Config " + key).c_str(), text_input.data(), text_input.capacity() + 1)) {
										text_input.resize(strlen(text_input.c_str()));
										config.second = text_input;
										NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
									}
								}
								else if constexpr (std::is_same_v<T, bool>) {
									bool temp = v;
									ImGui::Text(key.c_str());
									if (ImGui::Checkbox(("##Config " + key).c_str(), &temp)) {
										config.second = temp;
										NIKE_LUA_SERVICE->initScript(comp.script_id, comp.configs, comp.script_instance, comp.b_init);
									}
								}
								}, config.second);
						}
					}
				}
			}
		);
#endif
	}
}