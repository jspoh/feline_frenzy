/*****************************************************************//**
 * \file   cAnimation.cpp
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
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
	}
}