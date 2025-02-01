/*****************************************************************//**
 * \file   sysParticle.cpp
 * \brief  Particle system class
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
 * \date   January 2025
 * All content Copyright 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

#include "Core/stdafx.h"
#include "Systems/sysParticle.h"

namespace {
	using NSPM = NIKE::SysParticle::Manager;

}

NSPM::Manager() {
	active_particle_systems.reserve(MAX_ACTIVE_PARTICLE_SYSTEMS);

	// create vao and vbo for CLUSTER particle preset
	vao_map[ParticlePresets::CLUSTER] = -1;
	vbo_map[ParticlePresets::CLUSTER] = -1;
	NIKE::Assets::RenderLoader::RenderLoader::createClusterParticleBuffers(vao_map[ParticlePresets::CLUSTER], vbo_map[ParticlePresets::CLUSTER]);
}

NSPM::~Manager() {
	active_particle_systems.clear();
}


NSPM& NSPM::getInstance() {
	static Manager instance;
	return instance;
}

bool NSPM::addActiveParticleSystem(ParticlePresets preset, const Vector2f& origin, float duration) {
	if (active_particle_systems.size() >= MAX_ACTIVE_PARTICLE_SYSTEMS) {
		return false;
	}

	ParticleSystem new_particle_system;
	new_particle_system.particles.reserve(MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES);
	new_particle_system.preset = preset;
	new_particle_system.origin = origin;
	new_particle_system.is_alive = true;
	new_particle_system.duration = duration;
	new_particle_system.time_alive = 0.f;

	active_particle_systems[preset].push_back(new_particle_system);

	return true;
}


void NSPM::update() {
	const float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

	for (auto& [type, pss] : active_particle_systems) {
		for (auto& ps : pss) {
			// Update particle system
			if (ps.duration != -1 && ps.time_alive >= ps.duration) {
				ps.is_alive = false;
				continue;
			}

			ps.time_alive += dt;

			switch (ps.preset) {
			case ParticlePresets::BASE: {
				break;
			}
			case ParticlePresets::CLUSTER: {
				for (auto& p : ps.particles) {
					// Update particle
					p.pos += p.velocity * dt;
					p.velocity += p.acceleration * dt;
					p.time_alive += dt;

					// particle death

					if (p.size.x <= 0 || p.size.y <= 0) {
						p.is_alive = false;
					}

					if (p.color.a <= 0) {
						p.is_alive = false;
					}

					if (p.lifespan != -1 && p.time_alive > p.lifespan) {
						p.is_alive = false;
					}
				}

				// remove dead particles
				ps.particles.erase(std::remove_if(ps.particles.begin(), ps.particles.end(), [](const Particle& p) { return !p.is_alive; }), ps.particles.end());

				// spawn new particles

				if (ps.particles.size() < MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES) {
					constexpr int NEW_PARTICLES_PER_SECOND = 10;
					const Vector2f PARTICLE_VELOCITY_RANGE = { 5.f, 10.f };

					for (int _{}; _ < NEW_PARTICLES_PER_SECOND; _++) {
						Particle new_particle;
						new_particle.preset = ParticlePresets::CLUSTER;
						new_particle.pos = ps.origin;
						new_particle.velocity = {
							rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x,
							rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x
						};
						new_particle.acceleration = { 0.f, 0.f };
						new_particle.lifespan = 5.f;
						new_particle.size = { 1.f, 1.f };
						new_particle.color = {
							rand() % 255 / 255.f,
							rand() % 255 / 255.f,
							rand() % 255 / 255.f,
							1.f
						};
						new_particle.rotation = 0.f;
						new_particle.is_alive = true;

						ps.particles.push_back(new_particle);
					}
				}
				break;
			}
			}
		}

		// remove dead particle systems
		pss.erase(std::remove_if(pss.begin(), pss.end(), [](const ParticleSystem& ps) { return !ps.is_alive; }), pss.end());
	}
}

std::vector<NIKE::SysParticle::ParticleSystem>NSPM::getActiveParticleSystems(ParticlePresets preset) const {
	if (active_particle_systems.find(preset) == active_particle_systems.end()) {
		return {};
	}

	return active_particle_systems.at(preset);
}

unsigned int NSPM::getVAO(ParticlePresets preset) const {
	return vao_map.at(preset);
}

unsigned int NSPM::getVBO(ParticlePresets preset) const {
	return vbo_map.at(preset);
}
