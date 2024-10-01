/*****************************************************************//**
 * \file   sysGameLogic.h
 * \brief  game logic system
 *
 * \author Ho
 * \date   September 2024
 *********************************************************************/
#pragma once

#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "../headers/Managers/mSystem.h"

namespace GameLogic {

	//Object type
	struct ObjectSpawner{};

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

		//Update Inputs
		void update() override;

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!GAME_LOGIC_HPP
