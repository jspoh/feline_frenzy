/*****************************************************************//**
 * \file   cEnemy.h
 * \brief  Defines the components used for Enemies
 *
 * \author Soh Zhi Jie Bryan, 2301238, z.soh@digipen.edu
 * \date   November 2024
 *********************************************************************/

#pragma once

#ifndef C_ENEMY_HPP
#define C_ENEMY_HPP

#include "Core/stdafx.h"

namespace NIKE {
	namespace Enemy {
		//Script data
		struct Script {
			std::string script_id;
			std::string script_path;
			std::string function;
			bool b_loaded;

			Script() : script_id{ "" }, script_path{ "assets/Scripts/enemyBullet.lua" }, function{ "enemyBullet" }, b_loaded{ false } {}
			Script(std::string const& script) : script_id{ "" }, script_path{ script }, function{ "" }, b_loaded{ false } {}
		};

		struct Attack {
			Script script;			// Script info
			std::string prefab_path;// Prefab path
			float range;			// Attack Range
			float cooldown;			// Time between shot
			float last_shot_time;	// Tracking elapsed time since last shot
			float offset;			// Offset for bullet spawn position
			int layer;				// Bullet spawn layer

			Attack() :script(), prefab_path("bullet.prefab"), range(10.f), cooldown(10.f), last_shot_time(cooldown), offset(100.f), layer(0) {};
			Attack(std::string const& script, std::string const& prefab_path, float const& range,  float const& cooldown, float const& last_shot_time, float const& offset, int const& layer)
				: script{ script }, prefab_path{ prefab_path }, range { range }, 
				cooldown{ cooldown }, last_shot_time{ last_shot_time }, offset{ offset }, layer{ layer } {}
		};

		struct Spawner {
			int enemy_limit;			// Max amount of enemies that can be spawned
			int enemies_spawned;		// Number of enemies that have already been spawned (all spawners)
			float cooldown;				// Cooldown time for spawning enemies
			float last_spawn_time;		// Last time spawned

			Spawner() : enemy_limit(0), enemies_spawned(0), cooldown(10.f), last_spawn_time(cooldown) {};
			Spawner(int const& enemy_limit, int const& enemies_spawned, float const& cooldown, float const& last_spawn_time) : enemy_limit{ enemy_limit }, enemies_spawned{ enemies_spawned }, cooldown { cooldown }, last_spawn_time{ last_spawn_time } {};
		};

		void registerComponents();
	}
}


#endif // !C_ENEMY_HPP