/*****************************************************************//**
 * \file   cCombat.cpp
 * \brief  Combat components
 * 
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cCombat.h"

namespace NIKE {
	void Combat::registerComponents() {
		// Register health components
		NIKE_ECS_MANAGER->registerComponent<Health>();

		// Register health for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Health>(
			// Serialize
			[](Health const& comp) -> nlohmann::json {
				return	{
						{ "Lives", comp.lives },
						{ "Health", comp.health },
						{ "InvulnerableFlag", comp.invulnerableFlag },
						//{ "HealthBarActive", comp.healthBarActive}
				};
			},

			// Deserialize
			[](Health& comp, nlohmann::json const& data) {
				comp.lives = data.at("Lives").get<int>();
				comp.health = data.at("Health").get<float>();
				comp.invulnerableFlag = data.value("InvulnerableFlag", false);
				//comp.healthBarActive = data.at("HealthBarActive").get<bool>();
			}
		);

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Health>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Health& comp) {

				ImGui::Text("Edit Health Variables:");

				// For health
				{
					static float before_change_health;

					ImGui::DragFloat("Health", &comp.health, 0.1f);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_health = comp.health;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_health;

						// Change do action
						change_health.do_action = [&, health = comp.health]() {
							comp.health = health;
							};

						// Change undo action
						change_health.undo_action = [&, health = before_change_health]() {
							comp.health = health;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_health));
					}
				}

				// For lives
				{
					static int before_change_lives;

					ImGui::DragInt("Lives", &comp.lives, 1, 1);

					// Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_lives = comp.lives;
					}

					// Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_lives;

						// Change do action
						change_lives.do_action = [&, lives = comp.lives]() {
							comp.lives = lives;
							};

						// Change undo action
						change_lives.undo_action = [&, lives = before_change_lives]() {
							comp.lives = lives;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_lives));
					}
				}

				// For invulnerable flag
				{
					bool prev_flag = comp.invulnerableFlag;

					if (ImGui::Checkbox("Invulnerable", &comp.invulnerableFlag)) {
						LevelEditor::Action toggle_invulnerability;

						// Change do action
						toggle_invulnerability.do_action = [&, flag = comp.invulnerableFlag]() {
							comp.invulnerableFlag = flag;
							};

						// Change undo action
						toggle_invulnerability.undo_action = [&, flag = prev_flag]() {
							comp.invulnerableFlag = flag;
							};

						// Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(toggle_invulnerability));
					}
				}
			}
		);
#endif
	
		//Register damage components
		NIKE_ECS_MANAGER->registerComponent<Damage>();

		//Register damage for serialization
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

#ifndef NDEBUG
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
#endif
	}
}