/*****************************************************************//**
 * \file   subAnimations.cpp
 * \brief	Sprite animation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/Animation/subAnimations.h"
#include "Core/Engine.h"

namespace NIKE {
	/*****************************************************************//**
	* Base Animation
	*********************************************************************/
	void Animation::BaseAnimator::animationEndChecker(Animation::Base& base_components) {
		//Incr Completed Animations ( If animations to complete == 0, animation will keep running ) 
		if (++base_components.completed_animations >= base_components.animations_to_complete && base_components.animations_to_complete) {
			base_components.animation_mode = Mode::END;
			base_components.animations_to_complete = base_components.completed_animations;
		}
	}

	void Animation::BaseAnimator::baseUpdate(Animation::Base& base_components) {
		switch (base_components.animation_mode) {
		case Mode::PLAYING:
			break;
		case Mode::PAUSE:
			break;
		case Mode::RESTART:
			base_components.timer = 0.0f;
			base_components.completed_animations = 0;
			base_components.b_reverse = false;
			base_components.animation_mode = Mode::PLAYING;
			break;
		case Mode::END:
			break;
		default:
			break;
		}
	}

	/*****************************************************************//**
	* Spritesheet Animation
	*********************************************************************/

	int Animation::SpriteAnimator::frameCount(Animation::Sprite const& sprite_component) {
		int start_row = sprite_component.start_index.y;
		int start_col = sprite_component.start_index.x;
		int end_row = sprite_component.end_index.y;
		int end_col = sprite_component.end_index.x;
		int sheet_width = sprite_component.sheet_size.x;  // Number of columns in the sprite sheet

		// Calculate number of rows between start and end indices
		int row_diff = end_row - start_row;

		// If the start and end are on the same row, it's just the column difference
		if (row_diff == 0) {
			return end_col - start_col + 1;
		}

		// Calculate number of sprites
		int num_sprites = (row_diff * sheet_width) + (end_col + 1) - start_col;

		return num_sprites;
	}

	void Animation::SpriteAnimator::iterateForward(Animation::Sprite& sprite_component) {
		//Iterate Sprites Forward
		if (sprite_component.curr_index.x < sprite_component.sheet_size.x - 1) {
			//Increment X
			sprite_component.curr_index.x++;
		}
		else {
			//Reset X To Left Side Of Sheet
			sprite_component.curr_index.x = 0;

			//Increment Y Or Reset Y To Top Of Sheet
			if (sprite_component.curr_index.y < sprite_component.sheet_size.y - 1) {
				sprite_component.curr_index.y++;
			}
			else {
				sprite_component.curr_index.y = 0;
			}
		}
	}

	void Animation::SpriteAnimator::iterateBackWard(Animation::Sprite& sprite_component) {
		//Iterate Sprites Backward
		if (sprite_component.curr_index.x > 0) {
			//Increment X
			sprite_component.curr_index.x--;
		}
		else {
			//Reset X To Left Side Of Sheet
			sprite_component.curr_index.x = sprite_component.sheet_size.x - 1;

			//Increment Y Or Reset Y To Top Of Sheet
			if (sprite_component.curr_index.y > 0) {
				sprite_component.curr_index.y--;
			}
			else {
				sprite_component.curr_index.y = sprite_component.sheet_size.y - 1;
			}
		}
	}

	void Animation::SpriteAnimator::animateSprite(Animation::Base& base_component, Animation::Sprite& sprite_component, Render::Texture& sprite_texture) {
		//Limit Frame Size
		sprite_component.sheet_size.x = sprite_texture.frame_size.x;
		sprite_component.sheet_size.y = sprite_texture.frame_size.y;

		//Limit Start Index
		sprite_component.start_index.x = std::clamp(sprite_component.start_index.x, 0, sprite_component.sheet_size.x - 1);
		sprite_component.start_index.y = std::clamp(sprite_component.start_index.y, 0, sprite_component.sheet_size.y - 1);

		//Limit End Index
		sprite_component.end_index.x = std::clamp(sprite_component.end_index.x, 0, sprite_component.sheet_size.x - 1);
		sprite_component.end_index.y = std::clamp(sprite_component.end_index.y, 0, sprite_component.sheet_size.y - 1);
		
		//Move sprite index back to starting index
		if (base_component.animation_mode == Mode::END || base_component.animation_mode == Mode::RESTART) {
			sprite_component.curr_index = sprite_component.start_index;
		}

		//Update base animator
		baseUpdate(base_component);

		//Set texture frame index
		sprite_texture.frame_index = sprite_component.curr_index;
		
		////Calculate Time Before Next Animation
		//float timePerAnimation{ base_component.animation_duration / frameCount(sprite_component) };

		//Increment timer with delta time
		base_component.timer += NIKE_WINDOWS_SERVICE->getFixedDeltaTime();

		//Wait For Next Iteration
		if (base_component.timer >= base_component.frame_duration && (base_component.animation_mode == Mode::PLAYING)) {

			//Reset Timer
			base_component.timer = 0.0f;

			// Stops the animation from running if start index == end index
			if (sprite_component.start_index == sprite_component.end_index && sprite_component.start_index == sprite_component.curr_index) {
				return;
			}

			//If End Has Been Reached
			if (sprite_component.curr_index.x >= sprite_component.end_index.x && sprite_component.curr_index.y >= sprite_component.end_index.y) {
				//Check If Animation Is Finished
				animationEndChecker(base_component);

				//PingPong Function Checking
				if (base_component.b_pingpong) {

					//Set Reverse Iterator To True
					base_component.b_reverse = true;
				}
				else {
					sprite_component.curr_index = sprite_component.start_index;
					return;
				}
			}

			//Iterate Curr Sprite
			if (!base_component.b_reverse) {
				iterateForward(sprite_component);
			}
			else {
				iterateBackWard(sprite_component);
			}

			//If CurrSprite Is At Start
			if ((sprite_component.curr_index.x <= sprite_component.start_index.x && sprite_component.curr_index.y <= sprite_component.start_index.y) && base_component.b_reverse) {
				//Check If Animation Is Finished
				animationEndChecker(base_component);

				//Set Reverse Iterator To False
				base_component.b_reverse = false;
			}
		}
	}
}
