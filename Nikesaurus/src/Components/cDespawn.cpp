/*****************************************************************//**
 * \file   cDespawn.cpp
 * \brief  Despawning entity components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cDespawn.h"

namespace NIKE {
	void Despawn::registerComponents() {
		// Register despawn components
		NIKE_ECS_MANAGER->registerComponent<Lifetime>();

		// Register despawn for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Lifetime>(
			// Serialize
			[](Lifetime const& comp) -> nlohmann::json {
				return {
					{ "CurrentLifetime" , comp.current_lifetime },
					{ "MaxLifetime", comp.max_lifetime },
				};
			},

			// Deserialize
			[](Lifetime& comp, nlohmann::json const& data) {
				comp.current_lifetime = data.value("CurrentLifetime", 0.0f);
				comp.max_lifetime = data.value("MaxLifetime", 10.0f);
			}
		);

#ifndef NDEBUG
		// Level Editor UI registration
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Lifetime>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Lifetime& comp) {

				ImGui::Text("Edit Entity Lifetime Variables:");

				// For current lifetime
				{
					static float before_change_current_lifetime;

					ImGui::DragFloat("Current Lifetime", &comp.current_lifetime, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_current_lifetime = comp.current_lifetime;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_current_lifetime;

						// Change do action
						change_current_lifetime.do_action = [&, current_lifetime = comp.current_lifetime]() {
							comp.current_lifetime = current_lifetime;
							};

						// Change undo action
						change_current_lifetime.undo_action = [&, current_lifetime = before_change_current_lifetime]() {
							comp.current_lifetime = current_lifetime;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_current_lifetime));
					}
				}

				// For max lifetime
				{
					static float before_change_max_lifetime;

					ImGui::DragFloat("Max Lifetime", &comp.max_lifetime, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_max_lifetime = comp.max_lifetime;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_max_lifetime;

						// Change do action
						change_max_lifetime.do_action = [&, max_lifetime = comp.max_lifetime]() {
							comp.max_lifetime = max_lifetime;
							};

						// Change undo action
						change_max_lifetime.undo_action = [&, max_lifetime = before_change_max_lifetime]() {
							comp.max_lifetime = max_lifetime;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_max_lifetime));
					}
				}
			}
		);
#endif
	}
}