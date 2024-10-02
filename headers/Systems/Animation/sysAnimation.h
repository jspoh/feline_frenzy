/*****************************************************************//**
 * \file   sysAnimation.h
 * \brief	Animation manager
 *
 * \author Ho Shu Hng, 2301339, shuhng.ho@digipen.edu
 * \date   October 2024
 *********************************************************************/

#pragma once

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include "../headers/Managers/mSystem.h"

namespace Animation {

	/*****************************************************************//**
	* Animation manager
	*********************************************************************/
	class Manager : public System::ISystem {
	private:
		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		//Constructor
		Manager() = default;

		//Destructor
		~Manager() = default;


	};
}

#endif //!ANIMATION_SYSTEM_HPP