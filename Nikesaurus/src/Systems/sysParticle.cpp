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


using namespace NIKE::SysParticle;
using NSPM = NIKE::SysParticle::Manager;

NSPM::Manager() {
	active_particle_systems.reserve(MAX_ACTIVE_PARTICLE_SYSTEMS);

	// create vao and vbo for CLUSTER particle preset
	vao_map[Data::ParticlePresets::CLUSTER] = 0;
	vbo_map[Data::ParticlePresets::CLUSTER] = 0;
	NIKE::Assets::RenderLoader::RenderLoader::createClusterParticleBuffers(vao_map[Data::ParticlePresets::CLUSTER], vbo_map[Data::ParticlePresets::CLUSTER]);
}

NSPM::~Manager() {
	active_particle_systems.clear();
}


NSPM& NSPM::getInstance() {
	static Manager instance;
	return instance;
}

bool NSPM::addActiveParticleSystem(const std::string& ref, Data::ParticlePresets preset, const Vector2f& origin, float duration) {
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


	active_particle_systems[ref] = new_particle_system;

	return true;
}


void NSPM::update() {
	// !TODO: jspoh reorg update system

	const float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

	for (auto& [ref, ps] : active_particle_systems) {
		// Update particle system
		if (ps.duration != -1 && ps.time_alive >= ps.duration) {
			ps.is_alive = false;
			continue;
		}

		ps.time_alive += dt;

		switch (ps.preset) {
		case Data::ParticlePresets::BASE: {
			break;
		}
		case Data::ParticlePresets::CLUSTER: {
			static constexpr float LIFESPAN = 5.f;
			static const Vector2f ACCELERATION = { 0.f, 0.f };
			constexpr int NEW_PARTICLES_PER_SECOND = 10;
			const Vector2f PARTICLE_VELOCITY_RANGE = { 1.f, 10.f };

			for (auto& p : ps.particles) {
				// Update particle
				p.pos += p.velocity * dt;
				p.velocity += p.acceleration * dt;
				p.time_alive += dt;

				if (p.lifespan != -1.f) {
					// fade out
					p.color.a -= dt / p.lifespan;

					// darken
					p.color.r -= dt / p.lifespan;
					p.color.g -= dt / p.lifespan;
					p.color.b -= dt / p.lifespan;
				}

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

			static float time_since_last_spawn = 0.f;
			time_since_last_spawn += dt;

			int particles_to_spawn = static_cast<int>(time_since_last_spawn * NEW_PARTICLES_PER_SECOND);

			if (ps.particles.size() > MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES) {
				particles_to_spawn = 0;
				time_since_last_spawn = 0.f;
			}

			if (particles_to_spawn > 0) {
				// reset state
				time_since_last_spawn = 0.f;

				for (int _{}; _ < particles_to_spawn; _++) {
					Particle new_particle;
					new_particle.preset = Data::ParticlePresets::CLUSTER;
					new_particle.pos = ps.origin;
					new_particle.velocity = {
						rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x,
						rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x) + PARTICLE_VELOCITY_RANGE.x
					};
					new_particle.velocity.x = rand() % 2 ? new_particle.velocity.x : -new_particle.velocity.x;
					new_particle.velocity.y = rand() % 2 ? new_particle.velocity.y : -new_particle.velocity.y;
					new_particle.acceleration = ACCELERATION;
					new_particle.lifespan = LIFESPAN;
					new_particle.color = {
						rand() % 255 / 255.f,
						rand() % 255 / 255.f,
						rand() % 255 / 255.f,
						1.f
					};
					new_particle.rotation = 0.f;
					new_particle.is_alive = true;
					new_particle.size = { 5.f, 5.f };

					ps.particles.push_back(new_particle);
				}
			

			}
			break;
		}
		}
	}

	// remove dead particle systems
	//active_particle_systems.erase(std::remove_if(active_particle_systems.begin(), active_particle_systems.end(), [](const auto& pair) { return pair.second.is_alive; }), active_particle_systems.end());
}

std::vector<ParticleSystem>NSPM::getActiveParticleSystems() const {
	std::vector<ParticleSystem> v;
	v.reserve(active_particle_systems.size());
	for (const auto& [ref, ps] : active_particle_systems) {
		v.push_back(ps);
	}
	return v;
}

void NSPM::setParticleSystemOrigin(const std::string& ref, const Vector2f& origin) {
	active_particle_systems.at(ref).origin = origin;
}

unsigned int NSPM::getVAO(Data::ParticlePresets preset) const {
	return vao_map.at(preset);
}

unsigned int NSPM::getVBO(Data::ParticlePresets preset) const {
	return vbo_map.at(preset);
}
