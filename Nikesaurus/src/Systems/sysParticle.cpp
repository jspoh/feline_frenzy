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
	using NPSM = NIKE::ParticleSystem::Manager;

}

NPSM::Manager() {
	active_particle_systems.reserve(MAX_ACTIVE_PARTICLE_SYSTEMS);
}

NPSM::~Manager() {
	active_particle_systems.clear();
}


NPSM& NPSM::getInstance() {
	static Manager instance;
	return instance;
}

bool NPSM::addActiveParticleSystem(ParticlePresets preset, const Vector2f& start_pos) {
	if (active_particle_systems.size() >= MAX_ACTIVE_PARTICLE_SYSTEMS) {
		return false;
	}

	ParticleSystem new_particle_system;
	new_particle_system.particles.reserve(MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES);
	new_particle_system.preset = preset;
	new_particle_system.start_pos = start_pos;
	new_particle_system.is_alive = true;

	active_particle_systems.push_back(new_particle_system);

	return true;
}


void NPSM::update() {
	const float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

	for (ParticleSystem& ps : active_particle_systems) {

		if (ps.duration != -1 && ps.time_alive >= ps.duration) {
			ps.is_alive = false;
			continue;
		}

		ps.time_alive += dt;

		switch (ps.preset) {
		case ParticlePresets::BASIC: {
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
			}

			// remove dead particles
			ps.particles.erase(std::remove_if(ps.particles.begin(), ps.particles.end(), [](const Particle& p) { return !p.is_alive; }), ps.particles.end());

			// spawn new particles

			if (ps.particles.size() < MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES) {
				constexpr int NEW_PARTICLES_PER_SECOND = 1000;
				const Vector2f PARTICLE_VELOCITY_RANGE = { 10.f, 20.f };

				for (int _{}; _ < NEW_PARTICLES_PER_SECOND; _++) {
					Particle new_particle;
					new_particle.preset = ParticlePresets::BASIC;
					new_particle.pos = ps.start_pos;
					new_particle.velocity = {
						rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x,
						rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x
					};
					new_particle.acceleration = { 0.f, 0.f };
					new_particle.lifespan = -1;		// particle death not time dependent
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
	active_particle_systems.erase(std::remove_if(active_particle_systems.begin(), active_particle_systems.end(), [](const ParticleSystem& ps) { return !ps.is_alive; }), active_particle_systems.end());
}
