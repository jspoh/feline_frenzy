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

	active_particle_systems.push_back(new_particle_system);

	return true;
}


void NPSM::update() {
	for (ParticleSystem& ps : active_particle_systems) {
		switch (ps.preset) {
		case ParticlePresets::BASIC: {
			if (ps.particles.size() < MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES) {
				// add new particle
				Particle new_particle;

			}


			for (auto& particle : ps.particles) {
				// Update particle
			}
			break;
		}
		}
	}
}
