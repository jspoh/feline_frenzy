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

#include "Managers/ECS/mSystem.h"

namespace NIKE {
    namespace GameLogic {

	    class Manager : public System::ISystem {
	    private:
		    //Delete Copy Constructor & Copy Assignment
		    Manager(Manager const& copy) = delete;
		    void operator=(Manager const& copy) = delete;

			//Internal script management
			//sol::protected_function executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function);

			// Player Shooting
			void shootPlayerBullet(const Entity::Type& player_entity);

	    public:
		    //Default constructor
		    Manager() = default;

			//Default Destructor
			~Manager() = default;

			//Get system name
		    std::string getSysName() override
		    {
			    return "Game Logic System";
		    }

			//Init Inputs
			void init() override;

			////Register systems for lua
			//void registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system);

		    //Update Inputs
		    void update() override;
	    };
    }
}

#endif //!GAME_LOGIC_HPP
