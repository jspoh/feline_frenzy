/*****************************************************************//**
 * \file   sysGameLogic.h
 * \brief  game logic system
 *
 * \author Ho
 * \date   September 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/
#pragma once

#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "../headers/Managers/mSystem.h"
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Core/Engine.h"
#include "../headers/Components/cInput.h"
#include "../headers/Components/cScene.h"
#include "../headers/Managers/mEvents.h"
#include "../headers/Components/cAnimation.h"
#include "../headers/Components/cMove.h"

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

		std::string getSysName() override
		{
			return "Game Logic System";
		}

		//Update Inputs
		bool update() override;

		//Default Destructor
		~Manager() override = default;
	};
}

#endif //!GAME_LOGIC_HPP
