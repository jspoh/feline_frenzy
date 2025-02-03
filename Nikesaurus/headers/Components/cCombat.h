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
	namespace Combat {
		const enum class Factions : int {
			NEUTRAL = 0,
			PLAYER,
			ENEMY,
		};

		struct Damage {
			float damage;

			Damage() : damage(1.f) {};
			Damage(float const& damage)
				: damage{ damage } {}
		};

		struct Health {
			int lives;
			float health;
			bool invulnerableFlag; 
			//bool healthBarActive;

			Health() : lives(1), health(100.f), invulnerableFlag(false) {};
				//, healthBarActive(false) 
			
			Health(int const& lives, float const& health, bool const& invulnerableFlag = false)
				: lives{ lives }, health{ health }, invulnerableFlag{ invulnerableFlag } {};
		};

		struct Faction {
			Factions faction;
			Faction() : faction(Factions::NEUTRAL) {};
			Faction(Factions const& faction) : faction(faction) {};
		};

		void registerComponents();
	}
}

#endif // !C_COMBAT_HPP