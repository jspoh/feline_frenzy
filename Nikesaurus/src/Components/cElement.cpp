/*****************************************************************//**
 * \file   cElement.cpp
 * \brief  Enemy components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cElement.h"

namespace NIKE {
	void Element::registerComponents() {
		// Register components
		NIKE_ECS_MANAGER->registerComponent<Entity>();
		NIKE_ECS_MANAGER->registerComponent<Source>();

		// Register for Player serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Entity>(
			// Serialize
			[](Entity const& comp) -> nlohmann::json {
				return {
					{ "Element" , comp.element }
				};
			},

			// Deserialize
			[](Entity& comp, nlohmann::json const& data) {
				comp.element = data.at("Element").get<Elements>();
			}
		);

#ifndef NDEBUG
		// UI Registration for Player
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Entity>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Entity& comp) {

				// For current element
				{
					ImGui::Text("Adjust element:");
					static const char* elements_names[] = { "NONE", "FIRE", "WATER", "GRASS" };

					static Elements before_select_element;
					static int previous_element = static_cast<int>(comp.element);
					int current_element = static_cast<int>(comp.element);

					if (ImGui::Combo("##Element", &current_element, elements_names, IM_ARRAYSIZE(elements_names))) {
						Elements new_element = static_cast<Elements>(current_element);
						if (new_element != comp.element) {
							// Save action
							LevelEditor::Action save_element;
							save_element.do_action = [&, element = new_element]() {
								comp.element = element;
								};

							// Undo action
							save_element.undo_action = [&, element = before_select_element]() {
								comp.element = element;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_element));

							// Update the previous value
							before_select_element = comp.element;
							// Apply the new element
							comp.element = new_element;
						}
					}
				}
			}
		);
#endif

		// Register for Source serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Source>(
			// Serialize
			[](Source const& comp) -> nlohmann::json {
				return {
					{ "Element" , comp.element }
				};
			},

			// Deserialize
			[](Source& comp, nlohmann::json const& data) {
				comp.element = data.at("Element").get<Elements>();
			}
		);
		
#ifndef NDEBUG
		// UI Registration for Source
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Source>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Source& comp) {

				// For current element
				{
					ImGui::Text("Adjust element:");
					static const char* elements_names[] = { "NONE", "FIRE", "WATER", "GRASS"};

					static Elements before_select_element;
					static int previous_element = static_cast<int>(comp.element);
					int current_element = static_cast<int>(comp.element);
					
					if (ImGui::Combo("##Element", &current_element, elements_names, IM_ARRAYSIZE(elements_names))) {
						Elements new_element = static_cast<Elements>(current_element);
						if (new_element != comp.element) {
							// Save action
							LevelEditor::Action save_element;
							save_element.do_action = [&, element = new_element]() {
								comp.element = element;
								};

							// Undo action
							save_element.undo_action = [&, element = before_select_element]() {
								comp.element = element;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_element));

							// Update the previous value
							before_select_element = comp.element;
							// Apply the new element
							comp.element = new_element;
						}
					}
				}
			}
		);
#endif
	}
}