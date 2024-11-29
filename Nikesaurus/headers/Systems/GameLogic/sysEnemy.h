/*****************************************************************//**
 * \file   sysEnemy.h
 * \brief  Enemy system for engine
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu (100%)
 * \date   November 2024
 * All content © 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#pragma once

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Managers/ECS/mSystem.h"
#include "Components/cPhysics.h"
#include "Components/cEnemy.h"
#include "Managers/Services/Lua/sLua.h"

namespace NIKE {
	namespace Enemy {
		class Manager : public System::ISystem {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			//Internal lua system
			//std::unique_ptr<Lua::System> NIKE_LUA_SERVICE;

			//Internal script management
			sol::protected_function executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function);

			// Check if player is within range
			bool withinRange(const Entity::Type enemy, const Entity::Type player);

			// Shoot bullet
			void shootBullet(const Entity::Type enemy, const Entity::Type player);

		public:
			//Default Constructor
			Manager() = default;

			//Default Destructor
			~Manager() = default;

			//Init
			void init() override;

			//System name
			std::string getSysName() override
			{
				return "Enemy System";
			}

			//Register systems for lua
			void registerLuaSystem(std::shared_ptr<Lua::ILuaBind> system);

			//Update
			void update() override;
		};
	}
}
#endif //!ENEMY_HPP