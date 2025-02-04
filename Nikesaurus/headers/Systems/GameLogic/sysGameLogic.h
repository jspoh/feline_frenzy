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
#include <random>

namespace NIKE {
    namespace GameLogic {

	    class Manager : public System::ISystem {
	    private:
		    //Delete Copy Constructor & Copy Assignment
		    Manager(Manager const& copy) = delete;
		    void operator=(Manager const& copy) = delete;

			// Random Number Engine
			//static std::mt19937 gen;

			//Internal script management
			//sol::protected_function executeScript(std::string const& file_path, std::string& script_id, bool& b_loaded, std::string const& function);

			// Player Shooting
			//void shootCursor(const Entity::Type& player_entity);

			// Spawn Health Bar
			//void spawnHealthBar(const Entity::Type& entity);

			// Spawn Enemy
			void spawnEnemy(const Entity::Type& spawner);

			// Prefabs for spawnEnemy
			const std::string enemyArr[4] = { "enemy.prefab", "fireEnemy.prefab", "waterEnemy.prefab", "grassEnemy.prefab" };

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

		// Random Number Generator
		template <typename T>
		T getRandomNumber(T min, T max) {
			static std::random_device rd;
			// Define static random engine
			static std::mt19937 gen(rd());
			if constexpr (std::is_floating_point_v<T>) {
				std::uniform_real_distribution<T> distrib(min, max);
				return distrib(gen);
			}
			else if constexpr (std::is_integral_v<T>) {
				std::uniform_int_distribution<T> distrib(min, max);
				return distrib(gen);
			}
		}
    }
}

#endif //!GAME_LOGIC_HPP
