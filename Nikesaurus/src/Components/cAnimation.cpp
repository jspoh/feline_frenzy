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
				comp.animation_mode = static_cast<Mode>(data.value("Mode", 0));
				comp.animations_to_complete = data.value("Animation_Count", 0);
				comp.completed_animations = data.value("Completed_Animations", 0);
				comp.b_pingpong = data.value("B_PingPong", false);
				comp.b_reverse = data.value("B_Reverse", false);
				comp.frame_duration = data.value("Frame_Duration", 0.0f);
				comp.timer = data.value("Timer", 0.0f);
			},

			// Override Serialize
			[](Animation::Base const& comp, Animation::Base const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.animation_mode != other_comp.animation_mode) {
					delta["Mode"] = static_cast<int>(comp.animation_mode);
				}
				if (comp.animations_to_complete != other_comp.animations_to_complete) {
					delta["Animation_Count"] = comp.animations_to_complete;
				}
				if (comp.completed_animations != other_comp.completed_animations) {
					delta["Completed_Animations"] = comp.completed_animations;
				}
				if (comp.b_pingpong != other_comp.b_pingpong) {
					delta["B_PingPong"] = comp.b_pingpong;
				}
				if (comp.b_reverse != other_comp.b_reverse) {
					delta["B_Reverse"] = comp.b_reverse;
				}
				if (comp.frame_duration != other_comp.frame_duration) {
					delta["Frame_Duration"] = comp.frame_duration;
				}
				if (comp.timer != other_comp.timer) {
					delta["Timer"] = comp.timer;
				}

				return delta;
			},

			// Override Deserialize
			[](Animation::Base& comp, nlohmann::json const& delta) {
				if (delta.contains("Mode")) {
					comp.animation_mode = static_cast<Mode>(delta["Mode"]);
				}
				if (delta.contains("Animation_Count")) {
					comp.animations_to_complete = delta["Animation_Count"];
				}
				if (delta.contains("Completed_Animations")) {
					comp.completed_animations = delta["Completed_Animations"];
				}
				if (delta.contains("B_PingPong")) {
					comp.b_pingpong = delta["B_PingPong"];
				}
				if (delta.contains("B_Reverse")) {
					comp.b_reverse = delta["B_Reverse"];
				}
				if (delta.contains("Frame_Duration")) {
					comp.frame_duration = delta["Frame_Duration"];
				}
				if (delta.contains("Timer")) {
					comp.timer = delta["Timer"];
				}
			}
		);

		//Animation component adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Animation::Base>();

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
				comp.sheet_size.fromJson(data.value("Sheet_Size", Vector2f::def_json));
				comp.start_index.fromJson(data.value("Start_Index", Vector2f::def_json));
				comp.end_index.fromJson(data.value("End_Index", Vector2f::def_json));
				comp.curr_index.fromJson(data.value("Curr_Index", Vector2f::def_json));
			}, 

			// Override Serialize
			[](Animation::Sprite const& comp, Animation::Sprite const& other_comp) -> nlohmann::json {
				nlohmann::json delta;

				if (comp.sheet_size != other_comp.sheet_size) {
					delta["Sheet_Size"] = comp.sheet_size.toJson();
				}
				if (comp.start_index != other_comp.start_index) {
					delta["Start_Index"] = comp.start_index.toJson();
				}
				if (comp.end_index != other_comp.end_index) {
					delta["End_Index"] = comp.end_index.toJson();
				}
				if (comp.curr_index != other_comp.curr_index) {
					delta["Curr_Index"] = comp.curr_index.toJson();
				}

				return delta;
			},

			// Override Deserialize
			[](Animation::Sprite& comp, nlohmann::json const& delta) {
				if (delta.contains("Sheet_Size")) {
					comp.sheet_size.fromJson(delta["Sheet_Size"]);
				}
				if (delta.contains("Start_Index")) {
					comp.start_index.fromJson(delta["Start_Index"]);
				}
				if (delta.contains("End_Index")) {
					comp.end_index.fromJson(delta["End_Index"]);
				}
				if (delta.contains("Curr_Index")) {
					comp.curr_index.fromJson(delta["Curr_Index"]);
				}
			}
		);

		//Animation Sprite Component adding
		NIKE_SERIALIZE_SERVICE->registerComponentAdding<Animation::Sprite>();
	}

	void Animation::registerEditorComponents() {

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