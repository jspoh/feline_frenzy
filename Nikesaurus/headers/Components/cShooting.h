#pragma once
/*****************************************************************//**
 * \file   cShooting.h
 * \brief
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_SHOOTING_HPP
#define C_SHOOTING_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Shooting {
		struct Shooting {
			float cooldown;			//Time between shot
			float last_shot_time;	// Tracking elapsed time since last shot

			Shooting() : cooldown(10.f), last_shot_time(0.f) {};
			Shooting(float const& cooldown, float const& last_shot_time)
				: cooldown{ cooldown }, last_shot_time{ last_shot_time } {}
		};

		void registerComponents();
	}
}

#endif // !C_SHOOTING_HPP