/*****************************************************************//**
 * \file   cCombat.cpp
 * \brief  Combat components
 * 
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
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
				comp.lives = data.value("Lives", 1);
				comp.health = data.value("Health", 100.0f);
				comp.invulnerableFlag = data.value("InvulnerableFlag", false);
			},

			// Override Serialize
			[](Health const& comp, Health const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.lives != other_comp.lives) {
					delta["Lives"] = comp.lives;
				}
				if (comp.health != other_comp.health) {
					delta["Health"] = comp.health;
				}
				if (comp.invulnerableFlag != other_comp.invulnerableFlag) {
					delta["InvulnerableFlag"] = comp.invulnerableFlag;
				}

				return delta;
			},

			// Override Deserialize
			[](Health& comp, nlohmann::json const& delta) {
				if (delta.contains("Lives")) {
					comp.lives = delta["Lives"];
				}
				if (delta.contains("Health")) {
					comp.health = delta["Health"];
				}
				if (delta.contains("InvulnerableFlag")) {
					comp.invulnerableFlag = delta["InvulnerableFlag"];
				}
			}
		);

		//Health Comp Adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Health>();
	
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
				comp.damage = data.value("Damage", 1.0f);
			},

			// Override Serialize
			[](Damage const& comp, Damage const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.damage != other_comp.damage) {
					delta["Damage"] = comp.damage;
				}

				return delta;
			},

			// Override Deserialize for Damage
			[](Damage& comp, nlohmann::json const& delta) {
				if (delta.contains("Damage")) {
					comp.damage = delta["Damage"];
				}
			}
		);

		//Damage Comp Adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Damage>();
	}

	void Combat::registerEditorComponents() {
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
		// Register faction components
		NIKE_ECS_MANAGER->registerComponent<Faction>();

		// Register faction for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Faction>(
			// 🔹 Serialize
			[](Faction const& comp) -> nlohmann::json {
				return {
					{ "Faction", comp.faction }
				};
			},

			// 🔹 Deserialize
			[](Faction& comp, nlohmann::json const& data) {
				comp.faction = data.value("Faction", Factions::NEUTRAL);
			},

			// 🔹 Override Serialize (for delta updates)
			[](Faction const& comp, Faction const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.faction != other_comp.faction) {
					delta["Faction"] = comp.faction;
				}

				return delta;
			},

			// 🔹 Override Deserialize (for delta updates)
			[](Faction& comp, nlohmann::json const& delta) {
				if (delta.contains("Faction")) {
					comp.faction = delta["Faction"];
				}
			}
		);

		// Faction Component Adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Faction>();

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Faction>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Faction& comp) {

				ImGui::Text("Edit Faction Variables");

				static const char* faction_names[] = { "NEUTRAL", "PLAYER", "ENEMY" };

				// Store previous faction for undo/redo support
				static Factions before_select_faction = comp.faction;
				static int previous_faction = static_cast<int>(comp.faction);
				int current_faction = static_cast<int>(comp.faction);

				if (ImGui::Combo("##Faction", &current_faction, faction_names, IM_ARRAYSIZE(faction_names))) {
					Factions new_faction = static_cast<Factions>(current_faction);
					if (new_faction != comp.faction) {

						// Save action for undo/redo
						LevelEditor::Action change_faction;
						change_faction.do_action = [&, faction = new_faction]() {
							comp.faction = faction;
							};

						change_faction.undo_action = [&, faction = before_select_faction]() {
							comp.faction = faction;
							};

						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_faction));

						// Update previous values for next undo/redo
						before_select_faction = comp.faction;
						previous_faction = static_cast<int>(comp.faction);
					}
				}
			}
		);
#endif


	}
}