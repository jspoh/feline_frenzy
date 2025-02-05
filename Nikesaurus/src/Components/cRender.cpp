/*****************************************************************//**
 * \file   cRender.cpp
 * \brief	Render components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cRender.h"
#include "Systems/sysParticle.h"

namespace NIKE {
	void Render::registerComponents() {

		//Register render components
		NIKE_ECS_MANAGER->registerComponent<Render::Cam>();
		NIKE_ECS_MANAGER->registerComponent<Render::Text>();
		NIKE_ECS_MANAGER->registerComponent<Render::Shape>();
		NIKE_ECS_MANAGER->registerComponent<Render::Texture>();
		NIKE_ECS_MANAGER->registerComponent<Render::Hidden>();
		NIKE_ECS_MANAGER->registerComponent<Render::BuiltIn>();
		NIKE_ECS_MANAGER->registerComponent<Render::ParticleEmitter>();

		NIKE_SERIALIZE_SERVICE->registerComponent<Render::ParticleEmitter>(
			//Serialize
			[](Render::ParticleEmitter const& comp) -> nlohmann::json {
				return	{
					{ "preset", static_cast<int>(comp.preset) },
					{ "render_type", static_cast<int>(comp.render_type) },
					{ "offset", comp.offset.toJson() },
					{ "duration", comp.duration },
					//{ "ref", comp.ref }
				};
			},
			//Deserialize
			[](Render::ParticleEmitter& comp, nlohmann::json const& data) {
				const std::string particle_emitter_ref = "pe" + std::to_string(NIKE::SysParticle::Manager::getInstance().getNewPSID());

				comp.preset = static_cast<int>(data.at("preset").get<int>());
				comp.render_type = static_cast<int>(data.at("render_type").get<int>());
				comp.offset.fromJson(data.at("offset"));
				comp.duration = data.at("duration").get<float>();
				comp.ref = particle_emitter_ref;

				// add particle system
				NIKE::SysParticle::Manager::getInstance().addActiveParticleSystem(particle_emitter_ref, NIKE::SysParticle::Data::ParticlePresets(comp.preset), comp.offset, static_cast<NIKE::SysParticle::Data::ParticleRenderType>(comp.render_type), comp.duration);
			},
			// Override Serialize
			[](Render::ParticleEmitter const& comp, Render::ParticleEmitter const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.preset != other_comp.preset) {
					delta["preset"] = comp.preset;
				}
				if (comp.render_type != other_comp.render_type) {
					delta["render_type"] = comp.render_type;
				}
				if (comp.offset != other_comp.offset) {
					delta["offset"] = comp.offset.toJson();
				}
				if (comp.duration != other_comp.duration) {
					delta["duration"] = comp.duration;
				}
				//if (comp.ref != other_comp.ref) {
				//	delta["ref"] = comp.ref;
				//}
				return delta;
			},
			// Override Deserialize
			[](Render::ParticleEmitter& comp, nlohmann::json const& delta) {
				const std::string particle_emitter_ref = "pe" + std::to_string(NIKE::SysParticle::Manager::getInstance().getNewPSID());

				if (delta.contains("preset")) {
					comp.preset = delta["preset"];
				}
				if (delta.contains("render_type")) {
					comp.render_type = delta["render_type"];
				}
				if (delta.contains("offset")) {
					comp.offset.fromJson(delta["offset"]);
				}
				if (delta.contains("duration")) {
					comp.duration = delta["duration"];
				}
				if (delta.contains("ref")) {
					comp.ref = particle_emitter_ref;
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::ParticleEmitter>();

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
				comp.position.fromJson(data.value("Position", Vector2f::def_json));
				comp.zoom = data.value("Zoom", 1.0f);
			},

			// Override Serialize
			[](Render::Cam const& comp, Render::Cam const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.position != other_comp.position) {
					delta["Position"] = comp.position.toJson();
				}
				if (comp.zoom != other_comp.zoom) {
					delta["Zoom"] = comp.zoom;
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Cam& comp, nlohmann::json const& delta) {
				if (delta.contains("Position")) {
					comp.position.fromJson(delta["Position"]);
				}
				if (delta.contains("Zoom")) {
					comp.zoom = delta["Zoom"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Cam>();

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
				comp.font_id = data.value("Font_ID", "");
				comp.text = data.value("Text", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
				comp.scale = data.value("Scale", 1.0f);
				comp.size.fromJson(data.value("Size", Vector2f::def_json));
				comp.origin = data.value("Origin", TextOrigin::CENTER);
			},

			// Override Serialize
			[](Render::Text const& comp, Render::Text const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.font_id != other_comp.font_id) {
					delta["Font_ID"] = comp.font_id;
				}
				if (comp.text != other_comp.text) {
					delta["Text"] = comp.text;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}
				if (comp.scale != other_comp.scale) {
					delta["Scale"] = comp.scale;
				}
				if (comp.size != other_comp.size) {
					delta["Size"] = comp.size.toJson();
				}
				if (comp.origin != other_comp.origin) {
					delta["Origin"] = static_cast<int>(comp.origin);
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Text& comp, nlohmann::json const& delta) {
				if (delta.contains("Font_ID")) {
					comp.font_id = delta["Font_ID"];
				}
				if (delta.contains("Text")) {
					comp.text = delta["Text"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
				if (delta.contains("Scale")) {
					comp.scale = delta["Scale"];
				}
				if (delta.contains("Size")) {
					comp.size.fromJson(delta["Size"]);
				}
				if (delta.contains("Origin")) {
					comp.origin = static_cast<TextOrigin>(delta["Origin"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Text>();

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
				comp.model_id = data.value("Model_ID", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
			},

			// Override Serialize
			[](Render::Shape const& comp, Render::Shape const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.model_id != other_comp.model_id) {
					delta["Model_ID"] = comp.model_id;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Shape& comp, nlohmann::json const& delta) {
				if (delta.contains("Model_ID")) {
					comp.model_id = delta["Model_ID"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Shape>();

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
				comp.texture_id = data.value("Texture_ID", "");
				comp.color.fromJson(data.value("Color", Vector4f::def_json));
				comp.frame_size.fromJson(data.value("Frame_Size", Vector2i::def_json));
				comp.frame_index.fromJson(data.value("Frame_Index", Vector2i::def_json));
				comp.b_blend = data.value("B_Blend", false);
				comp.intensity = data.value("Intensity", 0.5f);
				comp.b_stretch = data.value("B_Stretch", false);
				comp.b_flip.fromJson(data.value("B_Flip", Vector2b::def_json));
			},

			// Override Serialize
			[](Render::Texture const& comp, Render::Texture const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.texture_id != other_comp.texture_id) {
					delta["Texture_ID"] = comp.texture_id;
				}
				if (comp.color != other_comp.color) {
					delta["Color"] = comp.color.toJson();
				}
				if (comp.frame_size != other_comp.frame_size) {
					delta["Frame_Size"] = comp.frame_size.toJson();
				}
				if (comp.frame_index != other_comp.frame_index) {
					delta["Frame_Index"] = comp.frame_index.toJson();
				}
				if (comp.b_blend != other_comp.b_blend) {
					delta["B_Blend"] = comp.b_blend;
				}
				if (comp.intensity != other_comp.intensity) {
					delta["Intensity"] = comp.intensity;
				}
				if (comp.b_stretch != other_comp.b_stretch) {
					delta["B_Stretch"] = comp.b_stretch;
				}
				if (comp.b_flip != other_comp.b_flip) {
					delta["B_Flip"] = comp.b_flip.toJson();
				}

				return delta;
			},

			// Override Deserialize
			[](Render::Texture& comp, nlohmann::json const& delta) {
				if (delta.contains("Texture_ID")) {
					comp.texture_id = delta["Texture_ID"];
				}
				if (delta.contains("Color")) {
					comp.color.fromJson(delta["Color"]);
				}
				if (delta.contains("Frame_Size")) {
					comp.frame_size.fromJson(delta["Frame_Size"]);
				}
				if (delta.contains("Frame_Index")) {
					comp.frame_index.fromJson(delta["Frame_Index"]);
				}
				if (delta.contains("B_Blend")) {
					comp.b_blend = delta["B_Blend"];
				}
				if (delta.contains("Intensity")) {
					comp.intensity = delta["Intensity"];
				}
				if (delta.contains("B_Stretch")) {
					comp.b_stretch = delta["B_Stretch"];
				}
				if (delta.contains("B_Flip")) {
					comp.b_flip.fromJson(delta["B_Flip"]);
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Render::Texture>();
	}

	void Render::registerEditorComponents() {

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::ParticleEmitter>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::ParticleEmitter& comp) {
				{
					// Before change
					static int before_change_preset;
					static Vector2f before_change_offset;

					// Preset
					ImGui::Text("Particle Preset:");

					std::string preset_options{};
					for (const auto& [preset, preset_ref] : SysParticle::Data::particle_preset_map) {
						preset_options += preset_ref + '\0';
					}
					preset_options += '\0';

					int selected_preset = static_cast<int>(comp.preset);
					static int previous_preset = selected_preset;
					if (ImGui::Combo("##Preset", &selected_preset, preset_options.c_str())) {
						// If the value changed, process the change
						if (selected_preset != previous_preset) {
							previous_preset = selected_preset;

							LevelEditor::Action change_preset;

							// Store the value before the change
							change_preset.do_action = [&, preset = selected_preset]() {
								comp.preset = preset;
								};

							// Store the undo action
							change_preset.undo_action = [&, preset = previous_preset]() {
								comp.preset = preset;
								};

							// Execute the action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_preset));
						}
					}

					// render type
					ImGui::Text("Particle Render Type:");
					std::string render_type_options{};
					for (const auto& [render_type, render_type_ref] : SysParticle::Data::particle_render_type_map) {
						render_type_options += render_type_ref + '\0';
					}
					render_type_options += '\0';

					int selected_render_type = static_cast<int>(comp.render_type);
					static int previous_render_type = selected_render_type;
					if (ImGui::Combo("##Render Type", &selected_render_type, render_type_options.c_str())) {
						// If the value changed, process the change
						if (selected_render_type != previous_render_type) {
							previous_render_type = selected_render_type;
							LevelEditor::Action change_render_type;
							// Store the value before the change
							change_render_type.do_action = [&, render_type = selected_render_type]() {
								comp.render_type = render_type;
								};
							// Store the undo action
							change_render_type.undo_action = [&, render_type = previous_render_type]() {
								comp.render_type = render_type;
								};
							// Execute the action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_render_type));
						}
					}

					// Offset
					ImGui::Text("Particle Offset:");
					ImGui::DragFloat2("##Offset", &comp.offset.x, 0.1f, -1000.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_offset = comp.offset;
					}
					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_offset;
						//Change pos do action
						change_offset.do_action = [&, offset = comp.offset]() {
							comp.offset = offset;
							};
						//Change pos undo action
						change_offset.undo_action = [&, offset = before_change_offset]() {
							comp.offset = offset;
							};
						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_offset));
					}

					// advanced options (for programmatic usage only)
					 
					ImGui::BeginDisabled(true);
					// Duration
					static bool infinite = comp.duration == -1.f;
					static bool prev_infinite = infinite;
					ImGui::Checkbox("Infinite Duration", &infinite);
					if (infinite) {
						comp.duration = -1.f;
					}
					else if (prev_infinite && !infinite) {
						comp.duration = 0.f;
					}
					prev_infinite = infinite;

					//ImGui::BeginDisabled(infinite);
					ImGui::Text("Particle Duration:");
					ImGui::DragFloat("##Duration", &comp.duration, 0.1f, 0.f, 1000.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::EndDisabled();
				}
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Cam>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Cam& comp) {
				ImGui::Text("Edit Camera variables");

				// For cam height
				{
					//Position before change
					static float before_change;

					ImGui::DragFloat("Camera Zoom", &comp.zoom, 0.1f, 0.f, float(UINT16_MAX), "%.3f", ImGuiSliderFlags_AlwaysClamp);

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

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Text>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Text& comp) {

				//Static variables for string input management
				static std::string text_input;

				//Initialization of string inputs upon collapsible shown
				if (ImGui::IsItemActivated() || comp_panel.isEntityChanged()) {
					text_input = comp.text;
				}

				ImGui::Text("Edit Text variables");

				ImGui::Spacing();

				// For Text Scale
				{
					// Before change
					static float editing_initial_scale = 0.0f;

					// Allow scale adjustment UI
					ImGui::DragFloat("Text Scale", &comp.scale, 0.1f, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

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

				ImGui::Spacing();

				//ImGui::Text((std::string("Text Size X: ") + std::to_string(comp.size.x)).c_str());
				//ImGui::Text((std::string("Text Size Y: ") + std::to_string(comp.size.y)).c_str());

				// For Text color
				{
					// Before change
					static Vector4f before_change;

					ImGui::Text("Adjust text color:");

					ImGui::ColorPicker4("Text Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);

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

				ImGui::Spacing();

				// For Text font
				{
					// Display a combo box for selecting a font
					ImGui::Text("Select Font:");

					// Hold the current and previous font selection
					static std::string previous_font_id = comp.font_id;
					std::string current_font_id = comp.font_id;

					// Get all loaded fonts
					const auto& all_loaded_fonts = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Font);

					// Find the index of the currently selected font in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_fonts.size(); ++i) {
						if (current_font_id == all_loaded_fonts[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for font selection
					if (ImGui::Combo("##SelectFont", &current_index, all_loaded_fonts.data(), static_cast<int>(all_loaded_fonts.size()))) {
						// Validate the selected index and get the new font ID
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_fonts.size())) {
							std::string new_font_id = all_loaded_fonts[current_index];
							if (new_font_id != comp.font_id) {
								// Save action
								LevelEditor::Action change_font_action;
								change_font_action.do_action = [&, font_id = new_font_id]() {
									comp.font_id = font_id;
									};

								// Undo action
								change_font_action.undo_action = [&, font_id = previous_font_id]() {
									comp.font_id = font_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_font_action));

								// Update the previous font ID
								previous_font_id = new_font_id;
							}
						}
					}
				}

				//Set Text input
				if (!comp.font_id.empty())
				{
					{
						//Set Text input
						ImGui::Text("Enter Text:");
						if (ImGui::InputText("##TextInput", text_input.data(), text_input.capacity() + 1)) {
							text_input.resize(strlen(text_input.c_str()));
						}

						ImGui::SameLine();

						//Save text input Button
						if (ImGui::Button("Save##TextInput")) {
							LevelEditor::Action save_text;

							//Save texrt action
							save_text.do_action = [&, text = text_input]() {
								comp.text = text;
								text_input = comp.text;
								};

							//Undo save text action
							save_text.undo_action = [&, text = comp.text]() {
								comp.text = text;
								text_input = comp.text;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_text));
							comp_panel.setPopUpSuccessMsg("Text Saved successfully");
							comp_panel.openPopUp("Success");
						}
					}

					ImGui::Spacing();

					// For Text Origin
					{
						ImGui::Text("Adjust text origin:");
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

			}
		);

		// UI for shape
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Shape>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Shape& comp) {
				ImGui::Text("Edit Shape variables");

				ImGui::Spacing();

				// For shape model
				{
					// Display a combo box for selecting a model
					ImGui::Text("Select Shape:");

					// Hold the current and previous model selection
					static std::string previous_model_id = comp.model_id;
					std::string current_model_id = comp.model_id;

					// Get all loaded models
					const auto& all_loaded_models = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Model);

					// Find the index of the currently selected model in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_models.size(); ++i) {
						if (current_model_id == all_loaded_models[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for model selection
					if (ImGui::Combo("##SelectModel", &current_index, all_loaded_models.data(), static_cast<int>(all_loaded_models.size()))) {
						// Validate the selected index and get the new model ID
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_models.size())) {
							std::string new_model_id = all_loaded_models[current_index];
							if (new_model_id != comp.model_id) {
								// Save action
								LevelEditor::Action change_model_action;
								change_model_action.do_action = [&, model_id = new_model_id]() {
									comp.model_id = model_id;
									};

								// Undo action
								change_model_action.undo_action = [&, model_id = previous_model_id]() {
									comp.model_id = model_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_model_action));

								// Update the previous model ID
								previous_model_id = new_model_id;
							}
						}
					}
				}

				ImGui::Spacing();

				// For shape color
				if (!comp.model_id.empty())
				{
					{
						// Before change
						static Vector4f before_change;

						ImGui::Text("Adjust shape color:");

						ImGui::ColorPicker4("Shape Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);

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
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Render::Texture>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Render::Texture& comp) {

				ImGui::Text("Edit Texture variables");

				ImGui::Spacing();

				// For texture id
				{
					// Hold the current and previous texture selection
					static std::string previous_texture = comp.texture_id;
					std::string current_texture = comp.texture_id;

					ImGui::Text("Select Texture");

					auto const& all_loaded_textures = NIKE_ASSETS_SERVICE->getAssetRefs(Assets::Types::Texture);

					// Find the index of the currently selected texture in the list
					int current_index = -1;
					for (size_t i = 0; i < all_loaded_textures.size(); ++i) {
						if (current_texture == all_loaded_textures[i]) {
							current_index = static_cast<int>(i);
							break;
						}
					}

					// Display combo box for texture selection
					if (ImGui::Combo("##SelectTexture", &current_index, all_loaded_textures.data(), static_cast<int>(all_loaded_textures.size()))) {
						// Validate the selected index and get the new texture
						if (current_index >= 0 && current_index < static_cast<int>(all_loaded_textures.size())) {
							std::string new_texture = all_loaded_textures[current_index];
							if (new_texture != comp.texture_id) {
								// Save action
								LevelEditor::Action change_font_action;
								change_font_action.do_action = [&, texture_id = new_texture]() {
									comp.texture_id = texture_id;
									};

								// Undo action
								change_font_action.undo_action = [&, texture_id = previous_texture]() {
									comp.texture_id = texture_id;
									};

								// Execute the action
								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_font_action));

								// Update the previous texture
								previous_texture = new_texture;
							}
						}
					}
				}

				ImGui::Spacing();

				if (!comp.texture_id.empty())
				{
					// For texture frame size
					{
						//Before change
						static Vector2i frame_size_before_change;

						//Change frame size
						if (ImGui::DragInt2("Frame Size", &comp.frame_size.x, 1, 1, 100)) {
							// Ensure frame size is never zero by clamping the values
							comp.frame_size.x = max(comp.frame_size.x, 1);
							comp.frame_size.y = max(comp.frame_size.y, 1);
						}

						//Check if begun editing
						if (ImGui::IsItemActivated()) {
							frame_size_before_change = comp.frame_size;
						}

						//Check if finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_frame_size;

							//Change do action
							change_frame_size.do_action = [&, frame_size = comp.frame_size]() {
								comp.frame_size = frame_size;
								};

							//Change undo action
							change_frame_size.undo_action = [&, frame_size = frame_size_before_change]() {
								comp.frame_size = frame_size;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_frame_size));
						}
					}

					ImGui::Spacing();

					// For texture frame index
					{
						// Before change
						static Vector2i frame_index_before_change;

						//Change frame index
						if (ImGui::DragInt2("Frame Index", &comp.frame_index.x, 1, 0, 100))
						{
							comp.frame_index.x = std::clamp(comp.frame_index.x, 0, comp.frame_size.x);
							comp.frame_index.y = std::clamp(comp.frame_index.y, 0, comp.frame_size.y);
						}

						//Check if begun editing
						if (ImGui::IsItemActivated()) {
							frame_index_before_change = comp.frame_index;
						}

						//Check if finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_frame_index;

							//Change do action
							change_frame_index.do_action = [&, frame_index = comp.frame_index]() {
								comp.frame_index = frame_index;
								};

							//Change undo action
							change_frame_index.undo_action = [&, frame_index = frame_index_before_change]() {
								comp.frame_index = frame_index;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_frame_index));
						}
					}

					ImGui::Spacing();

					// For texture color
					{
						// Before change
						static Vector4f texture_color_before_change;

						ImGui::Text("Adjust texture blend color:");

						ImGui::ColorPicker4("Texture Color", &comp.color.x, ImGuiColorEditFlags_AlphaBar);


						//Check if has begun editing
						if (ImGui::IsItemActivated()) {
							texture_color_before_change = comp.color;
						}

						//Check if  finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_color;

							//Change do action
							change_color.do_action = [&, color = comp.color]() {
								comp.color = color;
								};

							//Change undo action
							change_color.undo_action = [&, color = texture_color_before_change]() {
								comp.color = color;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_color));
						}
					}

					ImGui::Spacing();

					// For texture intensity
					{
						// Before change
						static float intensity_bef_change;

						ImGui::DragFloat("Texture Intensity", &comp.intensity, 0.001f, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);


						//Check if has begun editing
						if (ImGui::IsItemActivated()) {
							intensity_bef_change = comp.intensity;
						}

						//Check if  finished editing
						if (ImGui::IsItemDeactivatedAfterEdit()) {
							LevelEditor::Action change_intensity;

							//Change do action
							change_intensity.do_action = [&, intensity = comp.intensity]() {
								comp.intensity = intensity;
								};

							//Change undo action
							change_intensity.undo_action = [&, intensity = intensity_bef_change]() {
								comp.intensity = intensity;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_intensity));
						}
					}

					ImGui::Spacing();

					// Stretch checkbox
					{
						static bool before_bool_stretch = comp.b_stretch;
						if (ImGui::Checkbox("Stretch", &comp.b_stretch))
						{
							if (comp.b_stretch != before_bool_stretch)
							{
								// Save action
								LevelEditor::Action change_stretch;
								change_stretch.do_action = [&, stretch = comp.b_stretch]() {
									comp.b_stretch = stretch;
									};

								// Undo action
								change_stretch.undo_action = [&, stretch = before_bool_stretch]() {
									comp.b_stretch = stretch;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_stretch));

								// Update the previous value
								before_bool_stretch = comp.b_stretch;
							}

						}
					}

					ImGui::Spacing();

					// Blend checkbox
					{
						static bool before_bool_blend = comp.b_blend;
						if (ImGui::Checkbox("Blend", &comp.b_blend))
						{
							if (comp.b_blend != before_bool_blend)
							{
								// Save action
								LevelEditor::Action change_blend;
								change_blend.do_action = [&, blend = comp.b_blend]() {
									comp.b_blend = blend;
									};

								// Undo action
								change_blend.undo_action = [&, blend = before_bool_blend]() {
									comp.b_blend = blend;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_blend));

								// Update the previous value
								before_bool_blend = comp.b_blend;
							}

						}
					}

					ImGui::Spacing();

					// Flip x-axis checkbox
					{
						static bool before_bool_flip_x = comp.b_flip.x;
						if (ImGui::Checkbox("Flip Horizontally (x-axis) ", &comp.b_flip.x))
						{
							if (comp.b_flip.x != before_bool_flip_x)
							{
								// Save action
								LevelEditor::Action change_flip_x;
								change_flip_x.do_action = [&, flip_x = comp.b_flip.x]() {
									comp.b_flip.x = flip_x;
									};

								// Undo action
								change_flip_x.undo_action = [&, flip_x = before_bool_flip_x]() {
									comp.b_flip.x = flip_x;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_flip_x));

								// Update the previous value
								before_bool_flip_x = comp.b_flip.x;
							}

						}
					}

					ImGui::Spacing();

					// Flip y-axis checkbox
					{
						static bool before_bool_flip_y = comp.b_flip.y;
						if (ImGui::Checkbox("Flip Horizontally (y-axis) ", &comp.b_flip.y))
						{
							if (comp.b_flip.y != before_bool_flip_y)
							{
								// Save action
								LevelEditor::Action change_flip_y;
								change_flip_y.do_action = [&, flip_y = comp.b_flip.y]() {
									comp.b_flip.y = flip_y;
									};

								// Undo action
								change_flip_y.undo_action = [&, flip_y = before_bool_flip_y]() {
									comp.b_flip.y = flip_y;
									};

								NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_flip_y));

								// Update the previous value
								before_bool_flip_y = comp.b_flip.y;
							}

						}
					}

				}

			}
		);
#endif
	}
}