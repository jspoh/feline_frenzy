/*****************************************************************//**
 * \file   cHealth.h
 * \brief	Defines the components used for Health
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_HEALTH_HPP
#define C_HEALTH_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Health {
        struct Health {
            int lives;
            float health;
            bool invulnerableFlag; // New flag

            Health() : lives(1), health(100.f), invulnerableFlag(false) {};
            Health(int const& lives, float const& health, bool const& invulnerableFlag = false)
                : lives{ lives }, health{ health }, invulnerableFlag{ invulnerableFlag } {}
        };

		void registerComponents();
	}
}

#endif // !C_HEALTH_HPP