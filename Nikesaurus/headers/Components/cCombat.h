#pragma once
/*****************************************************************//**
 * \file   cCombat.h
 * \brief	Defines the components used for Combat
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_COMBAT_HPP
#define C_COMBAT_HPP

#include "Core/stdafx.h"

namespace NIKE {

	#define ENEMY_HEALTH 100.f
	#define ENEMY_LIVES 1
	#define HEAL_AMOUNT 10.f

	namespace Combat {
		const enum class Factions : int {
			NEUTRAL = 0,
			PLAYER,
			ENEMY,
			PLAYERBULLET,
			ENEMYBULLET,
		};

		struct Damage {
			float damage;

			Damage() : damage(1.f) {};
			Damage(float const& damage)
				: damage{ damage } {}
		};

		struct Health {
			int lives;
			float max_health;
			float health;
			bool invulnerable_flag;
			bool taken_damage;
			//bool healthBarActive;

			Health() : lives(ENEMY_LIVES), max_health(ENEMY_HEALTH), health(max_health), invulnerable_flag(false), taken_damage{false} {};
				//, healthBarActive(false) 
			
			Health(int const& lives, float const& max_health, float const& health, bool const& invulnerable_flag = false)
				: lives{ lives }, max_health {max_health}, health{ health }, invulnerable_flag{ invulnerable_flag }, taken_damage{ false } {};
		};

		struct Faction {
			Factions faction;
			Faction() : faction(Factions::NEUTRAL) {};
			Faction(Factions const& faction) : faction(faction) {};
		};

		struct HealthDrop {
			float heal_amount;

			HealthDrop() : heal_amount(HEAL_AMOUNT) {};
			HealthDrop(float const& heal_amount) : heal_amount{ heal_amount } {};
		};

		void registerComponents();

		void registerEditorComponents();
	}
}

#endif // !C_COMBAT_HPP