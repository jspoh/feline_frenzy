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

#include "../headers/Managers/mSystem.h"
#include "../headers/Managers/mEvents.h"
#include "../headers/Systems/Animation/subAnimations.h"

namespace Animation {

	/*****************************************************************//**
	* Animation manager
	*********************************************************************/
	class Manager : public System::ISystem, public Events::IEventListener<Animation::AnimationEvent> {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

		//Sprite animator
		std::unique_ptr<Animation::SpriteSheet> sprite_animator;

	public:
		//Default Constructor
		Manager() = default;

		//Singleton Of Manager Class
		static std::shared_ptr<Manager> getInstance() {
			static std::shared_ptr<Manager> instance{ std::make_shared<Manager>() };
			return instance;
		}

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

		//Execute event
		void executeEvent(std::shared_ptr<Animation::AnimationEvent> event) override;
	};
}

#endif //!ANIMATION_SYSTEM_HPP