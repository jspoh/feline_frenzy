/*****************************************************************//**
 * \file   cHealth.cpp
 * \brief	Health components
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cHealth.h"

namespace NIKE {
	void Health::registerComponents() {
		//Register transform components
		NIKE_ECS_MANAGER->registerComponent<Health>();

		//Register transform for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Health>(
			//Serialize
			[](Health const& comp) -> nlohmann::json {
				return	{
						{ "Lives", comp.lives },
						{ "Health", comp.health },
				};
			},

			//Deserialize
			[](Health& comp, nlohmann::json const& data) {
				comp.lives = data.at("Lives").get<int>();
				comp.health = data.at("Health").get<float>();
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Health>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Health& comp) {

				ImGui::Text("Edit Health Variables:");

				// For health
				{
					static float before_change_health;

					ImGui::DragFloat("Health", &comp.health, 0.1f);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_health = comp.health;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_health;

						//Change do action
						change_health.do_action = [&, health = comp.health]() {
							comp.health = health;
							};

						//Change undo action
						change_health.undo_action = [&, health = before_change_health]() {
							comp.health = health;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_health));
					}
				}

				// For lives
				{
					static int before_change_lives;

					ImGui::DragInt("Lives", &comp.lives, 1, 1);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_change_lives = comp.lives;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_lives;

						//Change do action
						change_lives.do_action = [&, lives = comp.lives]() {
							comp.lives = lives;
							};

						//Change undo action
						change_lives.undo_action = [&, lives = before_change_lives]() {
							comp.lives = lives;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_lives));
					}
				}
			}
		);
	}
}