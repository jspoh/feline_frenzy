/*****************************************************************//**
 * \file   cPhysics.cpp
 * \brief	Physics components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cPhysics.h"

namespace NIKE {
	void Physics::registerComponents() {
		//Register physics components
		NIKE_ECS_MANAGER->registerComponent<Physics::Dynamics>();
		NIKE_ECS_MANAGER->registerComponent<Physics::Collider>();

		//Register Dynamics for serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Physics::Dynamics>(
			//Serialize
			[](Physics::Dynamics const& comp) -> nlohmann::json {
				return	{
						{ "Max_Speed", comp.max_speed },
						{ "Drag", comp.drag },
						{ "Mass", comp.mass },
						{ "Velocity", comp.velocity.toJson() },
						{ "Force", comp.force.toJson() }
						};
			},

			//Deserialize
			[](Physics::Dynamics& comp, nlohmann::json const& data) {
				comp.max_speed = data.at("Max_Speed").get<float>();
				comp.drag = data.at("Drag").get<float>();
				comp.mass = data.at("Mass").get<float>();
				comp.velocity.fromJson(data.at("Velocity"));
				comp.force.fromJson(data.at("Force"));
			}
		);

		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Dynamics>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Dynamics& comp) {
				ImGui::Text("Edit Physics Dynamics:");

				// Edit Velocity
				{
					//Position before change
					static Vector2f velocity_before_change;

					//Drag position
					ImGui::DragFloat2("Velocity", &comp.velocity.x, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						velocity_before_change = comp.velocity;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_velocity;

						//Change pos do action
						change_velocity.do_action = [&, velocity = comp.velocity]() {
							comp.velocity = velocity;
							};

						//Change pos undo action
						change_velocity.undo_action = [&, velocity = velocity_before_change]() {
							comp.velocity = velocity;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_velocity));
					}
				}

				// Edit max_speed
				{
					//before change
					static float max_speed_before_change;

					//Drag 
					ImGui::DragFloat("Max Speed", &comp.max_speed, 0.1f);

					//Check if has begun editing
					if (ImGui::IsItemActivated()) {
						max_speed_before_change = comp.max_speed;
					}

					//Check if has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_max_speed;

						//Change pos do action
						change_max_speed.do_action = [&, max_speed = comp.max_speed]() {
							comp.max_speed = max_speed;
							};

						//Change pos undo action
						change_max_speed.undo_action = [&, max_speed = max_speed_before_change]() {
							comp.max_speed = max_speed;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_max_speed));
					}
				}

				// Edit Forces
				{
					//Position before change
					static Vector2f force_before_change;

					//Drag position
					ImGui::DragFloat2("Force", &comp.force.x, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						force_before_change = comp.force;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_force;

						//Change pos do action
						change_force.do_action = [&, force = comp.force]() {
							comp.force = force;
							};

						//Change pos undo action
						change_force.undo_action = [&, force = force_before_change]() {
							comp.force = force;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_force));
					}
				}

				// Edit Drag
				{
					//Position before change
					static float drag_before_change;

					//Drag position
					ImGui::DragFloat("Drag", &comp.drag, 0.1f);

					//Check if position has begun editing
					if (ImGui::IsItemActivated()) {
						drag_before_change = comp.drag;
					}

					//Check if position has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_drag;

						//Change pos do action
						change_drag.do_action = [&, drag = comp.drag]() {
							comp.drag = drag;
							};

						//Change pos undo action
						change_drag.undo_action = [&, drag = drag_before_change]() {
							comp.drag = drag;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_drag));
					}
				}

				// Edit mass
				{
					//before change
					static float mass_before_change;

					//Drag 
					ImGui::DragFloat("Mass", &comp.mass, 0.1f);

					//Check if has begun editing
					if (ImGui::IsItemActivated()) {
						mass_before_change = comp.mass;
					}

					//Check if has finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_mass;

						//Change pos do action
						change_mass.do_action = [&, mass = comp.mass]() {
							comp.mass = mass;
							};

						//Change pos undo action
						change_mass.undo_action = [&, mass = mass_before_change]() {
							comp.mass = mass;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_mass));
					}
				}
			}
		);

		//Register Collider for serializarion
		NIKE_SERIALIZE_SERVICE->registerComponent<Physics::Collider>(
			//Serialize
			[](Physics::Collider const& comp) -> nlohmann::json {
				return	{
						{ "B_Collided", comp.b_collided },
						{ "Resolution", static_cast<int>(comp.resolution) }
						};
			},

			//Deserialize
			[](Physics::Collider& comp, nlohmann::json const& data) {
				comp.b_collided = data.at("B_Collided").get<bool>();
				comp.resolution = static_cast<Resolution>(data.at("Resolution").get<int>());
			}
		);
	}
}