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
#include "../headers/Systems/Render/sysRender.h"
#include "../headers/Core/Engine.h"

namespace NIKE {
    namespace GameLogic {

	    class Manager : public System::ISystem {
	    private:
		    //Delete Copy Constructor & Copy Assignment
		    Manager(Manager const& copy) = delete;
		    void operator=(Manager const& copy) = delete;

            // Map to associate each entity with its state machine
            //std::unordered_map<Entity::Type, std::unique_ptr<StateMachineManager>> state_machines;

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
		    void update() override;

		    //Default Destructor
		    ~Manager() override = default;

            // Utility functions for logic, should be moved in future
            // Generates a random unit vector direction
            Vector2f getRandomDirection();
            Entity::Type getPlayerEntity();
	    };
    }
}

#endif //!GAME_LOGIC_HPP
