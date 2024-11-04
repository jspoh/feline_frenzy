/*****************************************************************//**
 * \file   cAnimation.h
 * \brief	Animation components
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *  * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef C_ANIMATION_HPP
#define C_ANIMATION_HPP

#include "Core/stdafx.h"
#include "Managers/Services/sEvents.h"

namespace NIKE {
	namespace Animation {

		//Animation Modes
		enum class Mode {
			PAUSE = 0,
			RESUME,
			RESTART,
			END
		};

		//Animation Event
		struct AnimationEvent : public Events::IEvent {
		public:
			Mode animation_action;
			std::string animator_id;

			//Constructor
			AnimationEvent(Mode action, std::string const& id)
				: animation_action{ action }, animator_id{ id } {}
		};

		//Sprite event
		struct SpriteEvent {
			Vector2i new_start_index;
			Vector2i new_end_index;
			std::string animator_id;
			int num_animations;

			//Boolean control
			bool animation_ongoing;

			//Constructor
			SpriteEvent(Vector2i const& new_start, Vector2i const& new_end, std::string const& id, int num_animations)
				: new_start_index{ new_start }, new_end_index{ new_end }, animator_id{ id }, num_animations{ num_animations }, animation_ongoing{ false } {}
		};

		struct cBase {
			//Animation Tracker ( Infinite number of animations if set to 0 )
			int animations_to_complete;

			//Completed animations
			int completed_animations;

			//Animation state
			bool b_animation_stop;

			//Animation state
			bool b_animation_finished;

			//PingPong animation
			bool b_pingpong;

			//Reverse animation
			bool b_reverse;

			//Animation Speed
			float animation_speed;

			//Timer
			Utility::Clock timer;

			//Animator identifier
			std::string animator_id;

			//Constructor
			cBase() : animator_id{""}, animations_to_complete{0}, completed_animations{0}, b_animation_stop{false},
				b_animation_finished{ false }, b_pingpong{ false }, b_reverse{ false }, animation_speed{ 0.0f }, timer() {}

			//Arguement constructor
			cBase(std::string const& animator_id, int animations_to_complete, float animation_speed, bool pingpong_mode = false)
				: animator_id{ animator_id }, animations_to_complete{ animations_to_complete }, completed_animations{ 0 }, b_animation_stop{ false },
				b_animation_finished{ false }, b_pingpong{ pingpong_mode }, b_reverse{ false }, animation_speed{ animation_speed }, timer() {}
		};

		struct cSprite {

			//Sprite sheet Size ( cols x rows )
			Vector2i sheet_size;

			//Start index
			Vector2i start_index;

			//End index
			Vector2i end_index;

			//Index Tracker
			Vector2i curr_index;

			//Constructor
			cSprite() : sheet_size(), start_index(), end_index(), curr_index() {}

			//Arguement constructor
			cSprite(Vector2i const& sheet_size, Vector2i const& start_index, Vector2i const& end_index)
				: sheet_size{ sheet_size }, start_index{ start_index }, end_index{ end_index }, curr_index{ start_index } {}
		};
	}
}

#endif //!C_ANIMATION_HPP