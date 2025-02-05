/*****************************************************************//**
 * \file   sysParticle.h
 * \brief  Particle system class
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   January 2025
 * All content Copyright 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


#pragma once

#ifndef _SYS_PARTICLE_H_
#define _SYS_PARTICLE_H_


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
				FIRE,
				NUM_PARTICLE_PRESETS
			};

			static inline std::unordered_map<ParticlePresets, std::string> particle_preset_map{
				{ ParticlePresets::BASE, "base" },
				{ ParticlePresets::CLUSTER, "cluster" },
				{ ParticlePresets::FIRE, "fire"}
			};

			static ParticlePresets getParticlePreset(const std::string& ref) {
				for (const auto& [preset, preset_ref] : particle_preset_map) {
					if (preset_ref == ref) {
						return preset;
					}
				}

				throw std::runtime_error("Invalid particle preset ref");
			}

			enum class ParticleRenderType {
				QUAD = 0,
				CIRCLE,
				TEXTURE,
				NUM_PARTICLE_RENDER_TYPES
			};

			static inline std::unordered_map<ParticleRenderType, std::string> particle_render_type_map{
				{ ParticleRenderType::QUAD, "quad" },
				{ ParticleRenderType::CIRCLE, "circle" },
				{ ParticleRenderType::TEXTURE, "texture" }
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
				float lifespan{};				// in seconds. -1 for infinite (-1 means particle EnemyDeathState not dependent on time)
				float rotation{};				// in degrees, anticlockwise

			};

			struct ParticleSystem {
				Vector2f origin{};
				bool using_world_pos{ true };
				Data::ParticlePresets preset{};
				std::vector<Particle> particles{};
				Data::ParticleRenderType render_type{};

				float duration{};		// in seconds. -1 for infinite. how long particle system should last
				float time_alive{};		// in seconds. how long particle system has been alive
				bool is_alive{};
			};

			class Manager {
			private:
				int next_ps_id{};
				std::unordered_map<std::string, ParticleSystem>  active_particle_systems;

				std::unordered_map<Data::ParticlePresets, unsigned int> vao_map;
				std::unordered_map<Data::ParticlePresets, unsigned int> vbo_map;

				Manager();
				~Manager();
			public:

				int getNewPSID();

				static Manager& getInstance();

				/**
				 *
				 *
				 * \param preset
				 * \return if particle system was successfully added
				 */
				bool addActiveParticleSystem(const std::string& ref, Data::ParticlePresets preset, const Vector2f& start_pos, Data::ParticleRenderType particle_render_type, float duration = -1.f, bool using_world_pos = true);

				bool removeActiveParticleSystem(const std::string& ref);
				
				void update();

				std::vector<ParticleSystem> getActiveParticleSystems() const;

				void setParticleSystemOrigin(const std::string& ref, const Vector2f& origin);
				void setParticleSystemPreset(const std::string& ref, Data::ParticlePresets preset);
				void setParticleSystemDuration(const std::string& ref, float duration);

				// get modifiable particle system
				ParticleSystem& getParticleSystem(const std::string& ref);

				unsigned int getVAO(Data::ParticlePresets preset) const;
				unsigned int getVBO(Data::ParticlePresets preset) const;
			};
		}


	}

#endif // !_SYS_PARTICLE_H_
