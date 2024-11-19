/*****************************************************************//**
 * \file   cRender.cpp
 * \brief	Render components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
						{ "Zoom", comp.zoom }
				};
			},

			//Deserialize
			[](Render::Cam& comp, nlohmann::json const& data) {
				comp.position.fromJson(data.at("Position"));
				comp.zoom = data.at("Zoom").get<float>();
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Cam>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Cam& comp) {
				ImGui::Text("Edit Camera variables");

				// For cam height
				{
					//Position before change
					static float before_change;

					ImGui::DragFloat("Camera Zoom", &comp.zoom, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.zoom;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_zoom;

						//Change pos do action
						change_zoom.do_action = [&, zoom = comp.zoom]() {
							comp.zoom = zoom;
							};

						//Change pos undo action
						change_zoom.undo_action = [&, zoom = before_change]() {
							comp.zoom = zoom;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_zoom));
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

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated()) {
					font_id = comp.font_id;
					text_input = comp.text;
				}

				// For Text Scale
				{
					// Before change
					static float editing_initial_scale = 0.0f;

					// Allow scale adjustment UI
					ImGui::DragFloat("Text Scale", &comp.scale, 0.1f);


					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						editing_initial_scale = comp.scale;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_text_scale;

						//Change pos do action
						change_text_scale.do_action = [&, scale = comp.scale]() {
							comp.scale = scale;
							};

						//Change pos undo action
						change_text_scale.undo_action = [&, scale = editing_initial_scale]() {
							comp.scale = scale;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_text_scale));
					}

				}

				//ImGui::Text((std::string("Text Size X: ") + std::to_string(comp.size.x)).c_str());
				//ImGui::Text((std::string("Text Size Y: ") + std::to_string(comp.size.y)).c_str());

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
					// When init only, not everytime
					if (!comp.font_id.empty() && font_id.empty())
					{
						font_id = comp.font_id;
					}

					ImGui::Text("Enter Font (wihtout the ttf):");
					if (ImGui::InputText("##FontID", font_id.data(), font_id.capacity() + 10)) {
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
							comp_panel.setPopUpSuccessMsg("Font Saved successfully");
							comp_panel.openPopUp("Success");

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
					// When init only, not everytime
					//if (!comp.text.empty() && text_input.empty())
					//{
					//	text_input = comp.text;
					//}

					ImGui::Text("Enter text:");
					if (ImGui::InputText("##TextInput", text_input.data(), text_input.capacity() + 1)) {
						text_input.resize(strlen(text_input.c_str()));
					}

					ImGui::SameLine();

					//Save font ID Button
					if (ImGui::Button("Save##TextInput")) {
						LevelEditor::Action save_text;

						// Capture the current value of comp.text 
						std::string before_change_text = comp.text;
						std::string before_change_input = text_input;

						//Save action
						save_text.do_action = [&, text = before_change_input]() {
							comp.text = text;
							text_input = text;
							};

						//Undo action
						save_text.undo_action = [&, text = before_change_text]() {
							comp.text = text;
							text_input = text;
							};

						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));
						comp_panel.setPopUpSuccessMsg("Text Saved successfully");
						comp_panel.openPopUp("Success");
					}
				}

				// For Text Origin
				{
					static const char* origin_names[] = { "CENTER", "TOP", "BOTTOM", "RIGHT", "LEFT" };
					// Hold the current selection and the previous value
					static NIKE::Render::TextOrigin before_select_origin;
					static int previous_origin = static_cast<int>(comp.origin);
					int current_origin = static_cast<int>(comp.origin);
					// Combo returns one bool check
					if (ImGui::Combo("##TextOrigin", &current_origin, origin_names, IM_ARRAYSIZE(origin_names))) {
						NIKE::Render::TextOrigin new_origin = static_cast<NIKE::Render::TextOrigin>(current_origin);
						if (new_origin != comp.origin) {
							// Save action
							LevelEditor::Action save_text;
							save_text.do_action = [&, origin = new_origin]() {
								comp.origin = origin;
								};

							// Undo action
							save_text.undo_action = [&, origin = before_select_origin]() {
								comp.origin = origin;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));

							// Update the previous value
							before_select_origin = comp.origin;
							// Apply the new origin
							comp.origin = new_origin;
						}
					}
				}
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

		// UI for shape
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Shape>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Shape& comp) {
				ImGui::Text("Edit Shape variables");

				//Static variables for string input management
				static std::string shape_model_input;

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated()) {
					shape_model_input = comp.model_id;
				}

				// For shape model
				{
					ImGui::Text("Enter shape model:");
					if (ImGui::InputText("##ShapeInput", shape_model_input.data(), shape_model_input.capacity() + 1)) {
						shape_model_input.resize(strlen(shape_model_input.c_str()));
					}

					ImGui::SameLine();

					//Save Shape model ID Button
					if (ImGui::Button("Save##ShapeModelID")) {
						if (NIKE_ASSETS_SERVICE->checkModelExist(shape_model_input))
						{
							LevelEditor::Action save_shape_model;

							//Save action
							save_shape_model.do_action = [&, shape_model = shape_model_input]() {
								comp.model_id = shape_model;
								shape_model_input = comp.model_id;
								};

							//Undo action
							save_shape_model.undo_action = [&, shape_model = comp.model_id]() {
								comp.model_id = shape_model;
								shape_model_input = comp.model_id;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_shape_model));
							comp_panel.setPopUpSuccessMsg("Shape Model Saved!");
							comp_panel.openPopUp("Success");
						}
						else
						{
							comp_panel.setPopUpErrorMsg("Shape Model Does Not Exist!");
							comp_panel.openPopUp("Error");
							shape_model_input = comp.model_id;
						}

					}
				}


				// For shape color
				{
					// Before change
					static Vector4f before_change;

					ImGui::DragFloat4("Shape Color", &comp.color.x, 0.1f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change = comp.color;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_shape_color;

						//Change pos do action
						change_shape_color.do_action = [&, color = comp.color]() {
							comp.color = color;
							};

						//Change pos undo action
						change_shape_color.undo_action = [&, color = before_change]() {
							comp.color = color;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_shape_color));
					}

					
				}

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