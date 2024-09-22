/*****************************************************************//**
 * \file   PhysicsSystem.h
 * \brief  Physics system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   September 2024
 *********************************************************************/

#pragma once

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "stdafx.h"
#include "Engine.h"

namespace Physics {

	class Manager : public System::Base {
	private:
		//Private Constructor
		Manager() = default;

		//Delete Copy Constructor & Copy Assignment
		Manager(Manager const& copy) = delete;
		void operator=(Manager const& copy) = delete;

	public:
		//Singleton Of Manager Class
		static Manager& getInstance() {
			static Manager instance;
			return instance;
		}

		//Init Inputs
		void init() override;

		//Update Inputs
		void update() override;

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!INPUT_HPP