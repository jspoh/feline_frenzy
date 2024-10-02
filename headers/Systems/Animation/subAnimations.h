/*****************************************************************//**
 * \file   subAnimations.h
 * \brief	Sprite animation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *********************************************************************/

#pragma once

#ifndef SUB_ANIMATIONS_HPP
#define SUB_ANIMATIONS_HPP

#include "../headers/Components/cAnimation.h"

namespace Animation {
	/*****************************************************************//**
	* Base Animation
	*********************************************************************/
	class Base {
	protected:
		//Stop Animation
		virtual void stopAnimation(Animation::cBase& base_components);

		//Resume Animation
		virtual void resumeAnimation(Animation::cBase& base_components);

		//Restart Animation ( Can Only Be Called If Animation Paused Abruptly )
		virtual void restartAnimation(Animation::cBase& base_components);

		//Set Animation Finished
		void finishAnimation(Animation::cBase& base_components);

	protected:
		//Default Constructor
		Base() = default;

		//Check If Animation Ends
		void animationEndChecker(Animation::cBase& base_components);

		//Virtual destructor
		virtual ~Base() = default;
	};

	/*****************************************************************//**
	* Spritesheet Animation
	*********************************************************************/
	class SpriteSheet : public Animation::Base {
	private:

		//Calculate number of frames
		int frameCount(Animation::cSprite const& sprite_component);

		//Sheet Iterator Function
		void iterateForward(Animation::cSprite& sprite_component);
		void iterateBackWard(Animation::cSprite& sprite_component);
	public:
		//Default constructor
		SpriteSheet() = default;

		//animate spritesheet
		void animateSprite(Animation::cBase& base_component, Animation::cSprite& sprite_component, Render::Texture& sprite_texture);

		//Default destructor
		~SpriteSheet() override = default;

		//Get event
		void executeEvent(Animation::Mode event_mode, Animation::cBase& base_components);
	};
}

#endif //!SUB_ANIMATIONS_HPP