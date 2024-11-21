/*****************************************************************//**
 * \file   cDamage.h
 * \brief	Defines the components used for Damage
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_DAMAGE_HPP
#define C_DAMAGE_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Damage {
		struct Damage {
			float damage;

			Damage() : damage(1.f) {};
			Damage(float const& damage)
				: damage{ damage } {}
		};

		void registerComponents();
	}
}

#endif // !C_DAMAGE_HPP