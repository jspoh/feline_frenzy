/*****************************************************************//**
 * \file   subAnimations.h
 * \brief	Sprite animation
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu(100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef SUB_ANIMATIONS_HPP
#define SUB_ANIMATIONS_HPP

#include "Components/cAnimation.h"
#include "Components/cRender.h"

namespace NIKE {
	namespace Animation {

		/*****************************************************************//**
		* Base Animation
		*********************************************************************/
		class BaseAnimator {
		protected:
			//Default Constructor
			BaseAnimator() = default;

			//Check If Animation Ends
			void animationEndChecker(Animation::Base& base_components);

			//Base animator update
			void baseUpdate(Animation::Base& base_components);

			//Virtual destructor
			virtual ~BaseAnimator() = default;
		};

		/*****************************************************************//**
		* Spritesheet Animation
		*********************************************************************/
		class SpriteAnimator : public Animation::BaseAnimator {
		private:

			//Calculate number of frames
			int frameCount(Animation::Sprite const& sprite_component);

			//Sheet Iterator Function
			void iterateForward(Animation::Sprite& sprite_component);
			void iterateBackWard(Animation::Sprite& sprite_component);
		public:
			//Default constructor
			SpriteAnimator() = default;

			//animate spritesheet
			void animateSprite(Animation::Base& base_component, Animation::Sprite& sprite_component, Render::Texture& sprite_texture);

			//Default destructor
			~SpriteAnimator() override = default;
		};
	}
}

#endif //!SUB_ANIMATIONS_HPP