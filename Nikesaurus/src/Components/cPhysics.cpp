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

#ifndef NDEBUG
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
#endif

		//Register Collider for serializarion
		NIKE_SERIALIZE_SERVICE->registerComponent<Physics::Collider>(
			//Serialize
			[](Physics::Collider const& comp) -> nlohmann::json {
				return	{
						{ "B_Bind_To_Entity", comp.b_bind_to_entity },
						{ "Position", comp.transform.position.toJson()},
						{ "Scale", comp.transform.scale.toJson()},
						{ "Rotation", comp.transform.rotation},
						{ "Pos_Offset", comp.pos_offset.toJson()},
						{ "Resolution", static_cast<int>(comp.resolution) }
						};
			},

			//Deserialize
			[](Physics::Collider& comp, nlohmann::json const& data) {
				comp.b_bind_to_entity = data.at("B_Bind_To_Entity").get<bool>();
				comp.transform.position.fromJson(data.at("Position"));
				comp.transform.scale.fromJson(data.at("Scale"));
				comp.transform.rotation = data.at("Rotation").get<float>();
				comp.pos_offset.fromJson(data.at("Pos_Offset"));
				comp.resolution = static_cast<Resolution>(data.at("Resolution").get<int>());
			}
		);

#ifndef NDEBUG
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Collider>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Collider& comp) {
				// For collider response
				{
					//Collide transform
					{
						//Edit link to entity
						ImGui::Text("Bind To Entity Transform: ");
						ImGui::SameLine();
						ImGui::SmallButton(comp.b_bind_to_entity ? "Unbind##BindTranform" : "Bind##BindTranform");

						//Check if button has been activated
						if (ImGui::IsItemActivated()) {
							LevelEditor::Action set_edit_bind;

							//Do bind transform
							set_edit_bind.do_action = [&, bind = !comp.b_bind_to_entity]() {
								comp.b_bind_to_entity = bind;
								};

							//Undo bind transform
							set_edit_bind.undo_action = [&, bind = comp.b_bind_to_entity]() {
								comp.b_bind_to_entity = bind;
								};

							//Execute action
							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(set_edit_bind));
						}

						//Check for bind to entity state
						if (comp.b_bind_to_entity) {
							ImGui::Text("Collider Transform:");
							ImGui::Text("Position: X %.2f, Y %.2f", comp.transform.position.x, comp.transform.position.y);
							ImGui::Text("Scale: X %.2f, Y %.2f", comp.transform.scale.x, comp.transform.scale.y);
							ImGui::Text("Rotation: %.2f", comp.transform.rotation);
						}
						else {
							//Transform text
							ImGui::Text("Edit Collider:");

							//Edit Offset
							{
								//Offset before change
								static Vector2f offset_before_change;

								//Drag offset
								ImGui::DragFloat2("Offset##Collider", &comp.pos_offset.x, 0.1f);

								//Check if offset has begun editing
								if (ImGui::IsItemActivated()) {
									offset_before_change = comp.pos_offset;
								}

								//Check if offset has finished editing
								if (ImGui::IsItemDeactivatedAfterEdit()) {

									//Apply action
									LevelEditor::Action change_offset;

									//Change offset do action
									change_offset.do_action = [&, offset = comp.pos_offset]() {
										comp.pos_offset = offset;
										};

									//Change offset undo action
									change_offset.undo_action = [&, offset = offset_before_change]() {
										comp.transform.position = offset;
										};

									//Execute action
									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_offset));
								}
							}

							//Edit Scale
							{
								//Scale before change
								static Vector2f scale_before_change;

								//Change scale
								ImGui::DragFloat2("Scale##Collider", &comp.transform.scale.x, 0.1f, EPSILON, (float)UINT16_MAX);

								//Check if scale has beguin editing
								if (ImGui::IsItemActivated()) {
									scale_before_change = comp.transform.scale;
								}

								//Check if scale has finished editing
								if (ImGui::IsItemDeactivatedAfterEdit()) {
									LevelEditor::Action change_scale;

									//Change scale do action
									change_scale.do_action = [&, scale = comp.transform.scale]() {
										comp.transform.scale = scale;
										};

									//Change scale undo action
									change_scale.undo_action = [&, scale = scale_before_change]() {
										comp.transform.scale = scale;
										};

									//Execute action
									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_scale));
								}
							}

							//Edit Rotation
							{
								//Rotation before change
								static float rotation_before_change;

								//Change rotation
								ImGui::DragFloat("Rotation##Collider", &comp.transform.rotation, 0.1f, -360.f, 360.f);

								//Check if rotation has begun editing
								if (ImGui::IsItemActivated()) {
									rotation_before_change = comp.transform.rotation;
								}

								//Check if rotation has finished editing
								if (ImGui::IsItemDeactivatedAfterEdit()) {
									LevelEditor::Action change_rotation;

									//Change rotation do action
									change_rotation.do_action = [&, rotation = comp.transform.rotation]() {
										comp.transform.rotation = rotation;
										};

									//Change rotation undo action
									change_rotation.undo_action = [&, rotation = rotation_before_change]() {
										comp.transform.rotation = rotation;
										};

									//Execute action
									NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_rotation));
								}
							}
						}
					}

					ImGui::Separator();

					//Collision State
					ImGui::Text("Colliding: %s", comp.b_collided ? "True" : "False");

					//Collider resolution
					ImGui::Text("Choose Collider Resolution:");
					static const char* resolution_names[] = { "NONE", "SLIDE", "BOUNCE", "DESTROY" };
					// Hold the current selection and the previous value
					static NIKE::Physics::Resolution before_select_resolution;
					static int previous_resolution = static_cast<int>(comp.resolution);
					int current_resolution = static_cast<int>(comp.resolution);
					// Combo returns one bool check
					if (ImGui::Combo("##ColliderChoice", &current_resolution, resolution_names, IM_ARRAYSIZE(resolution_names))) {
						NIKE::Physics::Resolution new_resolution = static_cast<NIKE::Physics::Resolution>(current_resolution);
						if (new_resolution != comp.resolution) {
							// Save action
							LevelEditor::Action save_resolution;
							save_resolution.do_action = [&, resolution = new_resolution]() {
								comp.resolution = resolution;
								};

							// Undo action
							save_resolution.undo_action = [&, resolution = before_select_resolution]() {
								comp.resolution = resolution;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_resolution));

							// Update the previous value
							before_select_resolution = comp.resolution;
							// Apply the new origin
							comp.resolution = new_resolution;
						}
					}
				}
			});
#endif
	}
}