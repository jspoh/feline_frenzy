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

			Health() : lives(1), health(100.f) {};
			Health(int const& lives, float const& health)
				: lives{ lives }, health{ health } {}
		};

		void registerComponents();
	}
}

#endif // !C_HEALTH_HPP