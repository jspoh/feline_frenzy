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
#include "Components/cElement.h"
#include "Components/cCombat.h"
#include "Components/cPhysics.h"
#include <random>

namespace NIKE {
    namespace GameLogic {

	    class Manager : public System::ISystem {
	    private:
		    //Delete Copy Constructor & Copy Assignment
		    Manager(Manager const& copy) = delete;
		    void operator=(Manager const& copy) = delete;

			// Combat BGM (BGMC) control
			void updateBGMCVolume();

			// Win overlay
			void gameOverlay(const std::string& background_texture, const std::string& play_again, const std::string& quit_game_text);

			// Portal interactions
			void handlePortalInteractions(const std::set<Entity::Type>& vents_entities, bool& is_spawn_portal);

			// Spawn Enemy
			void spawnEnemy(const Entity::Type& spawner);

			// Prefabs for spawnEnemy (enemy)
			const std::string enemyArr[4] = { "enemy.prefab", "fireEnemy.prefab", "waterEnemy.prefab", "grassEnemy.prefab" };

			// Prefabs for spawnEnemy (boss)
			const std::string enemyBossArr[3] = { "bossFire.prefab", "bossWater.prefab", "bossGrass.prefab"};

			// Status Effect Update
			void updateStatusEffects(Entity::Type entity);

			// Apply status effects
			void applyStatusEffect(Element::Combo& e_combo, Combat::Health& e_health, Physics::Dynamics& e_dynamic);

			// Remove status effect
			void removeStatusEffect(Element::Combo& e_combo, Physics::Dynamics& e_dynamic);

			// Lifesteal Status
			void applyLifesteal(float& health, float lifesteal_amount);

			// Burn Status
			void applyBurn(float& health, float burn_damage);

			// Freeze Status
			void applyFreeze(float& max_speed, float freeze_speed, float& temp_max_speed);

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
