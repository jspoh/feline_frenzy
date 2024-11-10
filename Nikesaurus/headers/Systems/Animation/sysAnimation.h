/*****************************************************************//**
 * \file   sysAnimation.h
 * \brief	Animation manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu (100%)
 * \date   October 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include "Managers/ECS/mSystem.h"
#include "Managers/Services/sEvents.h"
#include "Systems/Animation/subAnimations.h"

namespace NIKE {
	namespace Animation {

		/*****************************************************************//**
		* Animation manager
		*********************************************************************/
		class Manager : public System::ISystem {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Sprite animator
			std::unique_ptr<Animation::SpriteAnimator> sprite_animator;

		public:
			//Default Constructor
			Manager() = default;

			//Destructor
			~Manager() = default;

			//Animation system
			std::string getSysName() override
			{
				return "Animation System";
			}

			//Override init func
			void init() override;

			//Override update func
			void update() override;
		};
	}
}

#endif //!ANIMATION_SYSTEM_HPP