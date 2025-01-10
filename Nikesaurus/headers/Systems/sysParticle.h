/*****************************************************************//**
 * \file   sysParticle.h
 * \brief  Particle system class
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   January 2025
 * All content Copyright 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


#pragma once


#include "Core/stdafx.h"
#include "Managers/Services/Assets/sLoader.h"
#include "Core/Engine.h"

// !TODO: probably should implement lua for particles too on top of the presets

namespace NIKE {
	namespace ParticleSystem {
		constexpr int MAX_GLOBAL_ACTIVE_PARTICLES = 10000	;
		constexpr int MAX_ACTIVE_PARTICLE_SYSTEMS = 10;
		constexpr int MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES = MAX_GLOBAL_ACTIVE_PARTICLES / MAX_ACTIVE_PARTICLE_SYSTEMS;

		enum class ParticlePresets {
			BASIC = 0,
			EXPLOSION,
			SMOKE,
			FIRE,
			XP_DROP,
			NUM_PARTICLE_PRESETS
		};

		struct Particle {
			std::shared_ptr<Assets::Model> model = nullptr;

			ParticlePresets preset{ ParticlePresets::BASIC };

			Vector2f pos{};
			Vector2f velocity{};			// per second
			Vector2f acceleration{};		// per second
			float time_alive{};				// in seconds
			float lifespan{};				// in seconds. -1 for infinite (-1 means particle death not dependent on time)
			Vector2f size{};
			Vector4f color{};				// in range [0,1]
			float rotation{};				// in degrees, anticlockwise

			bool is_alive{ true };

		};

		struct ParticleSystem {
			Vector2f start_pos{};
			ParticlePresets preset{};
			std::vector<Particle> particles{};
			
			float duration{};		// in seconds. -1 for infinite. how long particle system should last
			float time_alive{};		// in seconds. how long particle system has been alive
			bool is_alive{};
		};

		class Manager {
		private:

			std::vector<ParticleSystem> active_particle_systems;

			Manager();
			~Manager();
		public:

			static Manager& getInstance();

			/**
			 * .
			 * 
			 * \param preset
			 * \return if particle system was successfully added
			 */
			bool addActiveParticleSystem(ParticlePresets preset, const Vector2f& start_pos);

			void update();
		};
	}
}
