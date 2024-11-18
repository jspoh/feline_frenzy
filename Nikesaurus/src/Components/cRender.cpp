/*****************************************************************//**
 * \file   cRender.cpp
 * \brief	Render components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cRender.h"

namespace NIKE {
	void Render::registerComponents() {

		//Register render components
		NIKE_ECS_MANAGER->registerComponent<Render::Cam>();
		NIKE_ECS_MANAGER->registerComponent<Render::Text>();
		NIKE_ECS_MANAGER->registerComponent<Render::Shape>();
		NIKE_ECS_MANAGER->registerComponent<Render::Texture>();

		//Register cam for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Cam>(
			//Serialize
			[](Render::Cam const& comp) -> nlohmann::json {
				return	{
						{ "Position", comp.position.toJson() },
						{ "Height", comp.height },
				};
			},

			//Deserialize
			[](Render::Cam& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.at("Position"));
				comp.height = data.at("Height").get<float>();
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Cam>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Cam& comp) {
				ImGui::Text("Edit Camera variables");

				// For cam height
				{
					//Position before change
					static float before_change;

					//Drag position
					if (comp.height <= 0.0f) {
						comp.height = static_cast<float>(NIKE_WINDOWS_SERVICE->getWindow()->getWindowSize().y);
					}
					ImGui::DragFloat("Camera Height", &comp.height, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.height;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_height;

						//Change pos do action
						change_height.do_action = [&, height = comp.height]() {
							comp.height = height;
							};

						//Change pos undo action
						change_height.undo_action = [&, height = before_change]() {
							comp.height = height;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_height));
					}

				}
			}
		);

		//Register text for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Text>(
			//Serialize
			[](Render::Text const& comp) -> nlohmann::json {
				return	{
						{ "Font_ID", comp.font_id },
						{ "Text", comp.text },
						{ "Color", comp.color.toJson() },
						{ "Scale", comp.scale },
						{ "Size", comp.size.toJson() },
						{ "Origin", static_cast<int>(comp.origin) }
				};
			},

			//Deserialize
			[](Render::Text& comp, nlohmann::json const& data) {
				comp.font_id = data.at("Font_ID").get<std::string>();
				comp.text = data.at("Text").get<std::string>();
				comp.color.fromJson(data.at("Color"));
				comp.scale = data.at("Scale").get<float>();
				comp.size.fromJson(data.at("Size"));
				comp.origin = static_cast<TextOrigin>(data.at("Origin").get<int>());
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Text>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Text& comp) {
				ImGui::Text("Edit Text variables");

				//Static variables for string input management
				static std::string font_id;
				static std::string text_input;

				// Display a dropdown to select resolution
				// For now this hard code it 
				static const char* origin_names[] = { "CENTER", "BOTTOM", "TOP", "LEFT", "RIGHT" };

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated()) {
					font_id = comp.font_id;
					text_input = comp.text;
				}

				// For Text Scale
				{
					// Before change
					static float before_change;

					ImGui::DragFloat("Text Scale", &comp.scale, 0.1f, EPSILON, 500.f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.scale;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_text_scale;

						//Change pos do action
						change_text_scale.do_action = [&, scale = comp.scale]() {
							comp.scale = scale;
							};

						//Change pos undo action
						change_text_scale.undo_action = [&, scale = before_change]() {
							comp.scale = scale;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_text_scale));
					}

				}

				ImGui::Text((std::string("Text Size X: ") + std::to_string(comp.size.x)).c_str());
				ImGui::Text((std::string("Text Size Y: ") + std::to_string(comp.size.y)).c_str());

				// For Text color
				{
					// Before change
					static Vector4f before_change;

					ImGui::DragFloat4("Text Color", &comp.color.x, 0.1f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.color;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_color;

						//Change pos do action
						change_color.do_action = [&, color = comp.color]() {
							comp.color = color;
							};

						//Change pos undo action
						change_color.undo_action = [&, color = before_change]() {
							comp.color = color;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_color));
					}

				}

				// For Text font
				{
					ImGui::Text("Enter Font (wihtout the ttf):");
					if (ImGui::InputText("##FontID", font_id.data(), font_id.capacity() + 1)) {
						font_id.resize(strlen(font_id.c_str()));
					}

					ImGui::SameLine();

					//Save font ID Button
					if (ImGui::Button("Save##FontID")) {
						if (NIKE_ASSETS_SERVICE->checkFontExist(font_id))
						{
							LevelEditor::Action save_font_id;

							//Save font id action
							save_font_id.do_action = [&, id = font_id]() {
								comp.font_id = id;
								font_id = comp.font_id;
								};

							//Undo save font id action
							save_font_id.undo_action = [&, id = comp.font_id]() {
								comp.font_id = id;
								font_id = comp.font_id;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_font_id));
						}
						else {
							comp_panel.setPopUpErrorMsg("Font Does Not Exist!");
							comp_panel.openPopUp("Error");
							font_id = comp.font_id;
						}
					}

				}


				// For Text input
				{
					ImGui::Text("Enter text:");
					if (ImGui::InputText("##TextInput", text_input.data(), text_input.capacity() + 1)) {
						text_input.resize(strlen(text_input.c_str()));
					}

					ImGui::SameLine();

					//Save font ID Button
					if (ImGui::Button("Save##TextInput")) {
						LevelEditor::Action save_text;

						// Capture the current value of comp.text and text_input
						std::string before_change_text = comp.text;
						std::string before_change_input = text_input;

						//Save action
						save_text.do_action = [&, text = text_input]() {
							comp.text = text;
							text_input = comp.text;
							};

						//Undo action
						save_text.undo_action = [&, text = comp.text]() {
							comp.text = text;
							text_input = comp.text;
							};

						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));
					}
				}

				// For Text Origin
				{
					// Variable to hold the selected resolution
					static NIKE::Render::TextOrigin before_select_origin;
					static bool is_editing = false;

					int current_origin = static_cast<int>(comp.origin);
					static int previous_origin = static_cast<int>(comp.origin); // Track the previous selected origin

					// Combo box for selecting text origin
					if (ImGui::Combo("##TextOrigin", &current_origin, origin_names, IM_ARRAYSIZE(origin_names))) {
						comp.origin = static_cast<NIKE::Render::TextOrigin>(current_origin);
						//cout << "Combo selection changed to: " << origin_names[current_origin] << endl;
					}
					
					//Check if begin editing
					if (!is_editing && ImGui::IsItemActivated()) {
						//cout << "Combo activated. Before: " << origin_names[static_cast<int>(before_select_origin)] << endl;
						before_select_origin = comp.origin;
						previous_origin = static_cast<int>(comp.origin);
						is_editing = true;
					}

					//Check if finished editing
					if (is_editing && !ImGui::IsItemActive()) {
						// Reset the editing flag after the combo box is deactivated
						is_editing = false;
						// Action only occurs if the origin has actually changed
						LevelEditor::Action change_origin;
						//cout << "Combo deactivated. After: " << origin_names[current_origin] << endl;

						// Action to apply the new value
						change_origin.do_action = [&, origin = comp.origin]() {
							comp.origin = origin;
							//cout << "Redo action: Changed to " << origin_names[static_cast<int>(origin)] << endl;
							};

						// Action to revert to the previous value
						change_origin.undo_action = [&, origin = before_select_origin]() {
							comp.origin = origin;
							//cout << "Undo action: Reverted to " << origin_names[static_cast<int>(origin)] << endl;
							};

						// Execute the action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_origin));
					}

				}

				// Display the selected text origin
				// ImGui::Text("Current Text Origin: %s", origin_names[current_origin]);
			}
		);

		//Register shape for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Shape>(
			//Serialize
			[](Render::Shape const& comp) -> nlohmann::json {
				return	{
						{ "Model_ID", comp.model_id },
						{ "Color", comp.color.toJson() }
				};
			},

			//Deserialize
			[](Render::Shape& comp, nlohmann::json const& data) {
				comp.model_id = data.at("Model_ID").get<std::string>();
				comp.color.fromJson(data.at("Color"));
			}
		);

		//Register shape for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Render::Texture>(
			//Serialize
			[](Render::Texture const& comp) -> nlohmann::json {
				return	{
						{ "Texture_ID", comp.texture_id },
						{ "Color", comp.color.toJson() },
						{ "Frame_Size", comp.frame_size.toJson() },
						{ "Frame_Index", comp.frame_index.toJson() },
						{ "B_Blend", comp.b_blend },
						{ "Intensity", comp.intensity },
						{ "B_Stretch", comp.b_stretch },
						{ "B_Flip", comp.b_flip.toJson() }
				};
			},

			//Deserialize
			[](Render::Texture& comp, nlohmann::json const& data) {
				comp.texture_id = data.at("Texture_ID").get<std::string>();
				comp.color.fromJson(data.at("Color"));
				comp.frame_size.fromJson(data.at("Frame_Size"));
				comp.frame_index.fromJson(data.at("Frame_Index"));
				comp.b_blend = data.at("B_Blend").get<bool>();
				comp.intensity = data.at("Intensity").get<float>();
				comp.b_stretch = data.at("B_Stretch").get<bool>();
				comp.b_flip.fromJson(data.at("B_Flip"));
			}
		);
	}
}