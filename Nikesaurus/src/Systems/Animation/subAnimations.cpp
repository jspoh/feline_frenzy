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

namespace NIKE {
	/*****************************************************************//**
	* Base Animation
	*********************************************************************/
	void Animation::Base::animationEndChecker(Animation::cBase& base_components) {
		//Incr Completed Animations ( If animations to complete == 0, animation will keep running ) 
		if (++base_components.completed_animations >= base_components.animations_to_complete && base_components.animations_to_complete) {
			base_components.b_animation_stop = true;
			base_components.b_animation_finished = true;
			base_components.animations_to_complete = base_components.completed_animations;
		}
	}

	void Animation::Base::stopAnimation(Animation::cBase& base_components) {
		base_components.b_animation_stop = true;
	}

	void Animation::Base::resumeAnimation(Animation::cBase& base_components) {
		if (!base_components.b_animation_finished) {
			base_components.b_animation_stop = false;
		}
	}

	void Animation::Base::restartAnimation(Animation::cBase& base_components) {
		base_components.b_animation_stop = false;
		base_components.b_animation_finished = false;
		base_components.completed_animations = 0;

		base_components.b_reverse = false;
	}

	void Animation::Base::finishAnimation(Animation::cBase& base_components) {
		base_components.b_animation_finished = true;
		base_components.b_animation_stop = true;
	}

	/*****************************************************************//**
	* Spritesheet Animation
	*********************************************************************/

	int Animation::SpriteSheet::frameCount(Animation::cSprite const& sprite_component) {
		int start_row = (int)sprite_component.start_index.y;
		int start_col = (int)sprite_component.start_index.x;
		int end_row = (int)sprite_component.end_index.y;
		int end_col = (int)sprite_component.end_index.x;
		int sheet_width = (int)sprite_component.sheet_size.x;  // Number of columns in the sprite sheet

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

	void Animation::SpriteSheet::iterateForward(Animation::cSprite& sprite_component) {
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

	void Animation::SpriteSheet::iterateBackWard(Animation::cSprite& sprite_component) {
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

	void Animation::SpriteSheet::animateSprite(Animation::cBase& base_component, Animation::cSprite& sprite_component, Render::Texture& sprite_texture) {
		//Calculate Time Before Next Animation
		float timePerAnimation{ base_component.animation_speed / frameCount(sprite_component) };

		//Wait For Next Iteration
		if (base_component.timer.getElapsedTime() >= timePerAnimation && !base_component.b_animation_stop) {

			//Reset Timer
			base_component.timer.restartClock();

			//If End Has Been Reached
			if (sprite_component.curr_index == sprite_component.end_index) {
				//Check If Animation Is Finished
				animationEndChecker(base_component);

				//PingPong Function Checking
				if (base_component.b_pingpong) {

					//Set Reverse Iterator To True
					base_component.b_reverse = true;
				}
				else {
					sprite_component.curr_index = sprite_component.start_index;
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
			if (sprite_component.curr_index == sprite_component.start_index && base_component.b_reverse) {
				//Check If Animation Is Finished
				animationEndChecker(base_component);

				//Set Reverse Iterator To False
				base_component.b_reverse = false;
			}
		}

		//Move sprite index back to starting index
		if (base_component.b_animation_finished) {
			sprite_component.curr_index = sprite_component.start_index;
		}

		sprite_texture.frame_index = sprite_component.curr_index;
	}

	void Animation::SpriteSheet::executeEvent(Animation::Mode event_mode, Animation::cBase& base_components) {
		switch (event_mode) {
		case Animation::Mode::PAUSE:
			stopAnimation(base_components);
			break;
		case Animation::Mode::RESUME:
			resumeAnimation(base_components);
			break;
		case Animation::Mode::RESTART:
			restartAnimation(base_components);
			break;
		case Animation::Mode::END:
			finishAnimation(base_components);
			break;
		default:
			break;
		}
	}
}
