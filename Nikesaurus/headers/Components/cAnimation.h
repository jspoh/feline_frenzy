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
#include "Managers/mEvents.h"

namespace NIKE {
	namespace Animation {

		//Temporary Disable DLL Export Warning
		#pragma warning(disable: 4251)

		//Animation Modes
		enum class NIKE_API Mode {
			PAUSE = 0,
			RESUME,
			RESTART,
			END
		};

		//Animation Event
		struct NIKE_API AnimationEvent : public Events::IEvent {
		public:
			Mode animation_action;
			std::string animator_id;

			//Constructor
			AnimationEvent(Mode action, std::string const& id)
				: animation_action{ action }, animator_id{ id } {}
		};

		//Sprite event
		struct NIKE_API SpriteEvent {
			Vector2 new_start_index;
			Vector2 new_end_index;
			std::string animator_id;
			int num_animations;
			int mouse_type; // -1 if using mouse
			int key_type; // -1 if using key

			//Boolean control
			bool animation_ongoing;

			//Constructor
			SpriteEvent(Vector2 const& new_start, Vector2 const& new_end, std::string const& id, int num_animations, int mouse_type, int key_type)
				: new_start_index{ new_start }, new_end_index{ new_end }, animator_id{ id }, num_animations{ num_animations }, mouse_type{ mouse_type }, key_type{ key_type }, animation_ongoing{ false } {}
		};

		struct NIKE_API cBase {
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
			cBase(std::string const& animator_id, int animations_to_complete, float animation_speed, bool pingpong_mode = false)
				: animator_id{ animator_id }, animations_to_complete{ animations_to_complete }, completed_animations{ 0 }, b_animation_stop{ false },
				b_animation_finished{ false }, b_pingpong{ pingpong_mode }, b_reverse{ false }, animation_speed{ animation_speed }, timer() {}
		};

		struct NIKE_API cSprite {

			//Sprite sheet Size ( cols x rows )
			Vector2 sheet_size;

			//Index Tracker
			Vector2 curr_index;

			//Start index
			Vector2 start_index;

			//End index
			Vector2 end_index;

			//Constructor
			cSprite(Vector2&& sheet_size, Vector2&& start_index, Vector2&& end_index)
				: sheet_size{ sheet_size }, curr_index{ start_index }, start_index{ start_index }, end_index{ end_index } {}
		};

		//Re-enable DLL Export warning
		#pragma warning(default: 4251)
	}
}

#endif //!C_ANIMATION_HPP