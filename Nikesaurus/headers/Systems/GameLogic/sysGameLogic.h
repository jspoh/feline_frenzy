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

#include "../headers/Managers/ECS/mSystem.h"

namespace NIKE {
    namespace GameLogic {

	    class Manager : public System::ISystem {
	    private:
		    //Delete Copy Constructor & Copy Assignment
		    Manager(Manager const& copy) = delete;
		    void operator=(Manager const& copy) = delete;

	    public:
		    //Default constructor
		    Manager() = default;

			//Default Destructor
			~Manager() = default;

		    std::string getSysName() override
		    {
			    return "Game Logic System";
		    }

			//Init Inputs
			void init() override;

		    //Update Inputs
		    void update() override;

            //// Utility functions for logic, should be moved in future
            //// Generates a random unit vector direction
            //Vector2f getRandomDirection();
            //Entity::Type getPlayerEntity();
	    };
    }
}

#endif //!GAME_LOGIC_HPP
