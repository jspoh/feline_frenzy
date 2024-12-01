/*****************************************************************//**
 * \file   cDamage.cpp
 * \brief	Damage components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cDamage.h"

namespace NIKE {
	void Damage::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Damage>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Damage>(
			//Serialize
			[](Damage const& comp) -> nlohmann::json {
				return	{
						{ "Damage", comp.damage },
				};
			},

			//Deserialize
			[](Damage& comp, nlohmann::json const& data) {
				comp.damage = data.at("Damage").get<float>();
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Damage>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Damage& comp) {

				ImGui::Text("Edit Damage Variables");

				// For shooting damage
				{
					static float before_change_damage;

					ImGui::DragFloat("Shot damage", &comp.damage, 0.1f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_damage = comp.damage;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_damage;

						//Change do action
						change_damage.do_action = [&, damage = comp.damage]() {
							comp.damage = damage;
							};

						//Change undo action
						change_damage.undo_action = [&, damage = before_change_damage]() {
							comp.damage = damage;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_damage));
					}
				}
			}
		);
	}
}