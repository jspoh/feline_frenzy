/*****************************************************************//**
 * \file   cAnimation.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (50%)
 * \co-author Bryan Lim Li Cheng, 2301214, bryanlicheng.l@digipen.edu (50%)
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Core/Engine.h"
#include "Components/cAnimation.h"

namespace NIKE {
	void Animation::registerComponents() {

		//Register animation components
		NIKE_ECS_MANAGER->registerComponent<Animation::Base>();
		NIKE_ECS_MANAGER->registerComponent<Animation::Sprite>();

		//Register Base For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Animation::Base>(
			//Serialize
			[](Animation::Base const& comp) -> nlohmann::json {
				return	{
						{ "Mode", static_cast<int>(comp.animation_mode) },
						{ "Animation_Count", comp.animations_to_complete },
						{ "Completed_Animations", comp.completed_animations },
						{ "B_PingPong", comp.b_pingpong },
						{ "B_Reverse", comp.b_reverse },
						{ "Frame_Duration", comp.frame_duration },
						{ "Timer", comp.timer }
				};
			},

			//Deserialize
			[](Animation::Base& comp, nlohmann::json const& data) {
				comp.animation_mode = static_cast<Mode>(data.at("Mode").get<int>());
				comp.animations_to_complete = data.at("Animation_Count").get<int>();
				comp.completed_animations = data.at("Completed_Animations").get<int>();
				comp.b_pingpong = data.at("B_PingPong").get<bool>();
				comp.b_reverse = data.at("B_Reverse").get<bool>();
				comp.frame_duration = data.at("Frame_Duration").get<float>();
				comp.timer = data.at("Timer").get<float>();
			}
		);

#ifndef NDEBUG
		// Animation base comp UI
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Animation::Base>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Animation::Base& comp) {
				ImGui::Text("Edit Animation Base:");

				// For number of animations
				{
					// Before change
					static int before_animate_number;

					ImGui::Text("Number of Animations ");
					ImGui::Text("If set to 0, infinite animations");
					ImGui::DragInt("##If set to 0, infinite animations", &comp.animations_to_complete, 1, 0, UINT16_MAX, "%d", ImGuiSliderFlags_AlwaysClamp);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_animate_number = comp.animations_to_complete;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_number_animation;

						//Change pos do action
						change_number_animation.do_action = [&, number = comp.animations_to_complete]() {
							comp.animations_to_complete = number;
							};

						//Change pos undo action
						change_number_animation.undo_action = [&, number = before_animate_number]() {
							comp.animations_to_complete = number;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_number_animation));
					}
				}

				ImGui::Spacing();

				// For frame duration
				{
					// Before change
					static float before_frame_duration = 0.0f;

					ImGui::DragFloat("Frame Duration", &comp.frame_duration, .001f, 0.f, float(UINT16_MAX), "%.3f", ImGuiSliderFlags_AlwaysClamp);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_frame_duration = comp.frame_duration;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_frame_duration;

						//Change pos do action
						change_frame_duration.do_action = [&, number = comp.frame_duration]() {
							comp.frame_duration = number;
							};

						//Change pos undo action
						change_frame_duration.undo_action = [&, number = before_frame_duration]() {
							comp.frame_duration = number;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_frame_duration));
					}
				}

				ImGui::Text("Animation timer: %f", &comp.timer);
				ImGui::Text("Completed Animations: %d", comp.completed_animations);


				// For Animtation settings
				{
					ImGui::Text("Adjust Animation Mode:");
					const char* mode_names[] = { "PLAYING", "PAUSE", "RESTART", "END" };
					// Hold the current selection and the previous value
					static NIKE::Animation::Mode before_selected_mode;
					static int previous_mode = static_cast<int>(comp.animation_mode);
					int current_mode = static_cast<int>(comp.animation_mode);
					// Combo returns one bool check
					if (ImGui::Combo("##AnimationMode", &current_mode, mode_names, IM_ARRAYSIZE(mode_names))) {
						NIKE::Animation::Mode new_mode = static_cast<NIKE::Animation::Mode>(current_mode);
						if (new_mode != comp.animation_mode) {
							// Save action
							LevelEditor::Action save_mode;
							save_mode.do_action = [&, mode = new_mode]() {
								comp.animation_mode = mode;
								};

							// Undo action
							save_mode.undo_action = [&, mode = before_selected_mode]() {
								comp.animation_mode = mode;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(save_mode));

							// Update the previous value
							before_selected_mode = comp.animation_mode;
							// Apply the new origin
							comp.animation_mode = new_mode;
						}
					}
				}

				ImGui::Spacing();

				// Check box to toggle ping pong
				{
					static bool before_bool_ping_pong = comp.b_pingpong;
					if (ImGui::Checkbox("Ping Pong", &comp.b_pingpong))
					{
						if (comp.b_pingpong != before_bool_ping_pong)
						{
							// Save action
							LevelEditor::Action change_ping_pong;
							change_ping_pong.do_action = [&, ping_pong = comp.b_pingpong]() {
								comp.b_pingpong = ping_pong;
								};

							// Undo action
							change_ping_pong.undo_action = [&, ping_pong = before_bool_ping_pong]() {
								comp.b_pingpong = ping_pong;
								};

							NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_ping_pong));

							// Update the previous value
							before_bool_ping_pong = comp.b_pingpong;
						}

					}
				}
			}
		);
#endif

		//Register Sprite For Serialization
		NIKE_SERIALIZE_SERVICE->registerComponent<Animation::Sprite>(
			//Serialize
			[](Animation::Sprite const& comp) -> nlohmann::json {
				return	{
						{ "Sheet_Size", comp.sheet_size.toJson() },
						{ "Start_Index", comp.start_index.toJson() },
						{ "End_Index", comp.end_index.toJson() },
						{ "Curr_Index", comp.curr_index.toJson() }
				};
			},

			//Deserialize
			[](Animation::Sprite& comp, nlohmann::json const& data) {
				comp.sheet_size.fromJson(data.at("Sheet_Size"));
				comp.start_index.fromJson(data.at("Start_Index"));
				comp.end_index.fromJson(data.at("End_Index"));
				comp.curr_index.fromJson(data.at("Curr_Index"));
			}
		);

#ifndef NDEBUG
		// Animation base comp UI
		NIKE_LVLEDITOR_SERVICE->registerCompUIFunc<Animation::Sprite>(
			[]([[maybe_unused]] LevelEditor::ComponentsPanel& comp_panel, Animation::Sprite& comp) {

				// For animation sprite start index
				{
					static Vector2i before_start_change;

					ImGui::DragInt2("Start Index", &comp.start_index.x, 1);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_start_change = comp.start_index;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_start_index;

						//Change pos do action
						change_start_index.do_action = [&, start_index = comp.start_index]() {
							comp.start_index = start_index;
							};

						//Change pos undo action
						change_start_index.undo_action = [&, start_index = before_start_change]() {
							comp.start_index = start_index;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_start_index));
					}
				}

				// For animation sprite end index
				{
					static Vector2i before_end_change;

					ImGui::DragInt2("End Index", &comp.end_index.x, 1);

					//Check if begin editing
					if (ImGui::IsItemActivated()) {
						before_end_change = comp.end_index;
					}

					//Check if finished editing
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						LevelEditor::Action change_end_index;

						//Change pos do action
						change_end_index.do_action = [&, end_index = comp.end_index]() {
							comp.end_index = end_index;
							};

						//Change pos undo action
						change_end_index.undo_action = [&, end_index = before_end_change]() {
							comp.end_index = end_index;
							};

						//Execute action
						NIKE_LVLEDITOR_SERVICE->executeAction(std::move(change_end_index));
					}
				}

				// Some data to show for animation sprite
				ImGui::Text("Sheet Size:");
				ImGui::BulletText("X = %d", comp.sheet_size.x);
				ImGui::BulletText("Y = %d", comp.sheet_size.y);
				ImGui::Text("Current Index:");
				ImGui::BulletText("X = %d", comp.curr_index.x);
				ImGui::BulletText("Y = %d", comp.curr_index.y);
			}
		);
#endif
	}
}