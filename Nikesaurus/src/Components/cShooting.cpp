/*****************************************************************//**
 * \file   cShooting.cpp
 * \brief  Shooting components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cShooting.h"

namespace NIKE {
	void Shooting::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Shooting>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Shooting>(
			//Serialize
			[](Shooting const& comp) -> nlohmann::json {
				return	{
						{ "Cooldown", comp.cooldown },
						{ "LastShotTime", comp.last_shot_time },
				};
			},

			//Deserialize
			[](Shooting& comp, nlohmann::json const& data) {
				comp.cooldown = data.at("Cooldown").get<float>();
				comp.last_shot_time = data.at("LastShotTime").get<float>();
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Shooting>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Shooting& comp) {

				ImGui::Text("Edit Shooting Variables:");

				// For shooting cooldown
				{
					static float before_change_cooldown;

					ImGui::DragFloat("Shot Cooldown", &comp.cooldown, 0.1f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_cooldown = comp.cooldown;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_cooldown;

						//Change do action
						change_cooldown.do_action = [&, cooldown = comp.cooldown]() {
							comp.cooldown = cooldown;
							};

						//Change undo action
						change_cooldown.undo_action = [&, cooldown = before_change_cooldown]() {
							comp.cooldown = cooldown;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_cooldown));
					}
				}

				ImGui::Text("Last Shot time: %f", comp.last_shot_time);
			}
		);
	}
}