/*****************************************************************//**
 * \file   cElement.cpp
 * \brief  Enemy components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cElement.h"

namespace NIKE {

	bool Element::doesElementCounterTarget(Element::Elements attacker, Element::Elements target) {
		switch (attacker) {
			case Element::Elements::WATER: return target == Element::Elements::FIRE;   // Water counters Fire
			case Element::Elements::FIRE: return target == Element::Elements::GRASS;  // Fire counters Grass
			case Element::Elements::GRASS: return target == Element::Elements::WATER; // Grass counters Water
			default: return false;
		}
	}

	Element::Elements Element::getElementFromStatus(Element::Status status) {
		switch (status) {
			case Element::Status::BURN:   return Element::Elements::FIRE;
			case Element::Status::FREEZE: return Element::Elements::WATER;
			case Element::Status::POISON: return Element::Elements::GRASS;
			default: return Element::Elements::NONE;
		}
	}

	float Element::getElementMultiplier(Element::Elements attacker, Element::Elements defender) {
		return Element::elemental_multiplier_table[static_cast<int>(attacker)][static_cast<int>(defender)];
	}

	std::string Element::getElementString(Element::Elements& element)
	{
		auto it = elements_to_string.find(element);
		// Return empty str if no element found
		return (it != elements_to_string.end()) ? it->second : "";
	}

	void Element::registerComponents() {
		// Register components
		NIKE_ECS_MANAGER->registerComponent<Combo>();
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
				comp.element = data.value("Element", Elements::NONE);
			},

			// Override Serialize
			[](Entity const& comp, Entity const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.element != other_comp.element) {
					delta["Element"] = comp.element;
				}

				return delta;
			},

			// Override Deserialize
			[](Entity& comp, nlohmann::json const& delta) {
				if (delta.contains("Element")) {
					comp.element = delta["Element"];
				}
			}
		);

		//Entity Comp Adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Entity>();

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
				comp.element = data.value("Element", Elements::NONE);
			},

			// Override Serialize
			[](Source const& comp, Source const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.element != other_comp.element) {
					delta["Element"] = comp.element;
				}

				return delta;
			},

			// Override Deserialize
			[](Source& comp, nlohmann::json const& delta) {
				if (delta.contains("Element")) {
					comp.element = delta["Element"];
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponent<Combo>(
			// Serialize
			[](Combo const& comp) -> nlohmann::json {
				return {
					{ "LastHits", comp.last_hits }
				};
			},

			// Deserialize
			[](Combo& comp, nlohmann::json const& data) {
				if (data.contains("LastHits")) {
					comp.last_hits = data["LastHits"].get<std::deque<Elements>>();
				}
			},

			// Override Serialize
			[](Combo const& comp, Combo const& other_comp) -> nlohmann::json {
				nlohmann::json delta;
				if (comp.last_hits != other_comp.last_hits) {
					delta["LastHits"] = comp.last_hits;
				}
				return delta;
			},

			// Override Deserialize
			[](Combo& comp, nlohmann::json const& delta) {
				if (delta.contains("LastHits")) {
					comp.last_hits = delta["LastHits"].get<std::deque<Elements>>();
				}
			}
		);

		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Source>();
	}

	void Element::registerEditorComponents() {
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

		// UI Registration for Source
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Source>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Source& comp) {

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
	}
}