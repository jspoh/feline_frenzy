/*****************************************************************//**
 * \file   cShooting.h
 * \brief	Defines the components used for Shooting 
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
		//Script data
		struct Script {
			std::string script_id;
			std::string script_path;
			std::string function;
			bool b_loaded;

			Script() : script_id{ "" }, script_path{ "" }, function{ "" }, b_loaded{ false } {}
			Script(std::string const& script) : script_id{ "" }, script_path{ script }, function{ "" }, b_loaded{ false } {}
		};

		struct Shooting {
			Script script;			// Script info
			float cooldown;			// Time between shot
			float last_shot_time;	// Tracking elapsed time since last shot
			float offset;			// Offset for bullet spawn position

			// Set last_shot_time equal to cooldown to allow player to shoot immediately
			// !TODO: Make offset automatic based on spawning entity's scale?
			Shooting() :script(), cooldown(.5f), last_shot_time(cooldown), offset(.0f) {};
			Shooting(std::string const& script, float const& cooldown, float const& last_shot_time, float const& offset)
				: script{ script }, cooldown{ cooldown }, last_shot_time{ last_shot_time }, offset{ offset } {}
		};

		void registerComponents();
	}
}

#endif // !C_SHOOTING_HPP