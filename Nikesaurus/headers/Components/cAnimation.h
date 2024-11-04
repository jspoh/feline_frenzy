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

namespace NIKE {
	namespace Animation {

		//Animation Modes
		enum class Mode {
			PLAYING = 0,
			PAUSE,
			RESTART,
			END
		};

		struct Base {
			//Animation Mode
			Mode animation_mode;

			//Animation Tracker ( Infinite number of animations if set to 0 )
			int animations_to_complete;

			//Completed animations
			int completed_animations;

			//PingPong animation
			bool b_pingpong;

			//Reverse animation
			bool b_reverse;

			//Animation frame duration
			float frame_duration;

			//Animation timer
			float timer;

			//Constructor
			Base() : animation_mode{ Mode::PLAYING }, animations_to_complete{ 0 }, completed_animations{ 0 },
				 b_pingpong{ false }, b_reverse{ false }, frame_duration{ 0.0f }, timer() {}

			//Arguement constructor
			Base(int animations_to_complete, float frame_duration, bool pingpong_mode = false)
				: animation_mode{ Mode::PLAYING }, animations_to_complete{ animations_to_complete }, completed_animations{ 0 },
				b_pingpong{ pingpong_mode }, b_reverse{ false }, frame_duration{ frame_duration }, timer() {}
		};

		struct Sprite {

			//Sprite sheet Size ( cols x rows )
			Vector2i sheet_size;

			//Start index
			Vector2i start_index;

			//End index
			Vector2i end_index;

			//Index Tracker
			Vector2i curr_index;

			//Constructor
			Sprite() : sheet_size(), start_index(), end_index(), curr_index() {}

			//Arguement constructor
			Sprite(Vector2i const& sheet_size, Vector2i const& start_index, Vector2i const& end_index)
				: sheet_size{ sheet_size }, start_index{ start_index }, end_index{ end_index }, curr_index{ start_index } {}
		};

		void registerComponents();

		nlohmann::json serializeComponents(Entity::Type entity);
	}
}

#endif //!C_ANIMATION_HPP