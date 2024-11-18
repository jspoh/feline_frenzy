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