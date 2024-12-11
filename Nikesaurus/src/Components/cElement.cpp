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
		NIKE_ECS_MANAGER->registerComponent<Player>();
		NIKE_ECS_MANAGER->registerComponent<Source>();

		// Register for Player serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Player>(
			// Serialize
			[](Player const& comp) -> nlohmann::json {
				return {
					{ "CurrentElement" , comp.current_element }
				};
			},

			// Deserialize
			[](Player& comp, nlohmann::json const& data) {
				comp.current_element = data.at("CurrentElement").get<Elements>();
			}
		);

#ifndef NDEBUG
		// UI Registration for Player
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Player>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Player& comp) {

				ImGui::Text("Edit Element:");

				// For current element
				{
					static Elements before_change_element;

					ImGui::DragInt("Current Element", reinterpret_cast<int*>(&comp.current_element), 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_element = comp.current_element;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						// Within valid range check
						// !TODO: Consider using a dropdown box instead?
						if (static_cast<int>(comp.current_element) < static_cast<int>(Elements::NONE) ||
							static_cast<int>(comp.current_element) > static_cast<int>(Elements::GRASS)) {
							comp.current_element = Elements::NONE; 
						}

						LevelEditor::Action change_element;

						// Change do action
						change_element.do_action = [&, current_element = comp.current_element]() {
							comp.current_element = current_element;
							};

						// Change undo action
						change_element.undo_action = [&, current_element = before_change_element]() {
							comp.current_element = current_element;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_element));
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

				ImGui::Text("Edit Element:");

				// For current element
				{
					static Elements before_change_element;

					ImGui::DragInt("Element", reinterpret_cast<int*>(&comp.element), 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_element = comp.element;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						// Within valid range check
						// !TODO: Consider using a dropdown box instead?
						if (static_cast<int>(comp.element) < static_cast<int>(Elements::NONE) ||
							static_cast<int>(comp.element) > static_cast<int>(Elements::GRASS)) {
							comp.element = Elements::NONE;
						}

						LevelEditor::Action change_element;

						// Change do action
						change_element.do_action = [&, current_element = comp.element]() {
							comp.element = current_element;
							};

						// Change undo action
						change_element.undo_action = [&, current_element = before_change_element]() {
							comp.element = current_element;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_element));
					}
				}
			}
		);
#endif
	}
}