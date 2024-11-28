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

namespace NIKE {
	namespace Enemy {
		class Manager : public System::ISystem {
		private:
			//Delete Copy Constructor & Copy Assignment
			Manager(Manager const& copy) = delete;
			void operator=(Manager const& copy) = delete;

			// Player entity
			//Entity::Type player_entity;
			//bool player_entity_cached = false;

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

			//Update
			void update() override;
		};
	}
}
#endif //!ENEMY_HPP