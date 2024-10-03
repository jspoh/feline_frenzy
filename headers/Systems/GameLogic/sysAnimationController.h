/*****************************************************************//**
 * \file   sysAnimationController.h
 * \brief  animation controller system
 *
 * \author Ho
 * \date   September 2024
 *********************************************************************/
#pragma once

#ifndef ANIMATION_CONTROLLER_HPP
#define ANIMATION_CONTROLLER_HPP

#include "../headers/Managers/mSystem.h"

namespace AnimationController {

	class Manager : public System::ISystem {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		//Default constructor
		Manager() = default;

		//Init Inputs
		void init() override;

		std::string getSysName() override
		{
			return "Animation Controller System";
		}

		//Update Inputs
		bool update() override;

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!ANIMATION_CONTROLLER_HPP
