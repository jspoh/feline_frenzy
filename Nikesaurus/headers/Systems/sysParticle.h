/*****************************************************************//**
 * \file   sysParticle.h
 * \brief  Particle system class
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   January 2025
 * All content Copyright 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


#pragma once

#ifndef PARTICLE_HPP
#define PARTICLE_HPP


#include "Core/stdafx.h"
#include "Managers/Services/Assets/sLoader.h"
#include "Core/Engine.h"

 // !TODO: probably should implement lua for particles too on top of the presets

namespace NIKE {
	namespace SysParticle {
		constexpr int MAX_GLOBAL_ACTIVE_PARTICLES = 10000;
		constexpr int MAX_ACTIVE_PARTICLE_SYSTEMS = 10;
		constexpr int MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES = MAX_GLOBAL_ACTIVE_PARTICLES / MAX_ACTIVE_PARTICLE_SYSTEMS;

		class Data {
		private:
			Data() = default;
			~Data() = default;
		public:
			enum class ParticlePresets {
				BASE = 0,
				CLUSTER,
				NUM_PARTICLE_PRESETS
			};

			static inline std::unordered_map<ParticlePresets, std::string> particle_preset_map{
				{ ParticlePresets::BASE, "base" },
				{ ParticlePresets::CLUSTER, "cluster" }
			};
		};


		struct Particle {
			std::shared_ptr<Assets::Model> model = nullptr;

			Data::ParticlePresets preset{ Data::ParticlePresets::BASE };

			bool is_alive{ true };

			Vector2f pos{};
			Vector4f color{};				// in range [0,1]
			Vector2f size{};
			float velocity{};			// per second
			Vector2f vector{};
			float acceleration{};		// per second
			float time_alive{};				// in seconds
			float lifespan{};				// in seconds. -1 for infinite (-1 means particle death not dependent on time)
			float rotation{};				// in degrees, anticlockwise

		};

		struct ParticleSystem {
			Vector2f origin{};
			Data::ParticlePresets preset{};
			std::vector<Particle> particles{};

			float duration{};		// in seconds. -1 for infinite. how long particle system should last
			float time_alive{};		// in seconds. how long particle system has been alive
			bool is_alive{};
		};

		class Manager {
		private:

			std::unordered_map<std::string, ParticleSystem>  active_particle_systems;

			std::unordered_map<Data::ParticlePresets, unsigned int> vao_map;
			std::unordered_map<Data::ParticlePresets, unsigned int> vbo_map;

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
			bool addActiveParticleSystem(const std::string& ref, Data::ParticlePresets preset, const Vector2f& start_pos, float duration = -1.f);

			void update();

			std::vector<ParticleSystem> getActiveParticleSystems() const;

			void setParticleSystemOrigin(const std::string& ref, const Vector2f& origin);

			unsigned int getVAO(Data::ParticlePresets preset) const;
			unsigned int getVBO(Data::ParticlePresets preset) const;
		};
	}


}

#endif // !PARTICLE_HPP
