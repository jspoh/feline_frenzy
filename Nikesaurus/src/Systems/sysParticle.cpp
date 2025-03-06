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

// !TODO: jspoh add auto init values for entity and prefabs
// !TODO: jspoh update update() function to use ParticleEmitter component values

 // !NOTE: jspoh. future - add texture support for particles
 // !NOTE: jspoh. future - add lua support for particles

namespace {
	/**
	 * .
	 *
	 * \param x	min
	 * \param y	max
	 * \param dp	decimal places
	 * \return float
	 */
	float rand_float(const Vector2f& range, const int dp) {
		if (dp < 0) return 0.0f; // Prevent invalid decimal places

		const int multiplier = static_cast<int>(powf(10, dp));
		if (multiplier == 0) return range.x; // Avoid division by zero, return min range

		const int range_scaled = static_cast<int>((range.y - range.x) * multiplier);
		if (range_scaled < 0) return range.x; // Handle invalid range

		const int rand_int = rand() % (range_scaled + 1) + static_cast<int>(range.x * multiplier);
		return static_cast<float>(rand_int) / multiplier;
	}


	float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}

	Vector2f lerp(const Vector2f& a, const Vector2f& b, float t) {
		return Vector2f(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
	}

	Vector4f lerp(const Vector4f& a, const Vector4f& b, float t) {
		return Vector4f(lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t), lerp(a.a, b.a, t));
	}
}


using namespace NIKE::SysParticle;
using NSPM = NIKE::SysParticle::Manager;

NSPM::Manager() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		NIKEE_CORE_ERROR("OpenGL error at the start of {0}: {1}", __FUNCTION__, err);
	}

	active_particle_systems.reserve(MAX_ACTIVE_PARTICLE_SYSTEMS);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		NIKEE_CORE_ERROR("OpenGL error after creating empty vao and vbo in {0}: {1}", __FUNCTION__, err);
	}

	// create vao and vbo for BASE particle preset
	vao_map[Data::ParticlePresets::BASE] = 0;
	vbo_map[Data::ParticlePresets::BASE] = 0;
	glCreateVertexArrays(1, &vao_map[Data::ParticlePresets::BASE]);
	glCreateBuffers(1, &vbo_map[Data::ParticlePresets::BASE]);
	glVertexArrayVertexBuffer(vao_map[Data::ParticlePresets::BASE], 0, vbo_map[Data::ParticlePresets::BASE], 0, 0);

	err = glGetError();
	if (err != GL_NO_ERROR) {
		NIKEE_CORE_ERROR("OpenGL error after creating empty vao and vbo in {0}: {1}", __FUNCTION__, err);
	}
	

	// create vao and vbo for CLUSTER particle preset
	vao_map[Data::ParticlePresets::CLUSTER] = 0;
	vbo_map[Data::ParticlePresets::CLUSTER] = 0;
	NIKE::Assets::RenderLoader::RenderLoader::createClusterParticleBuffers(vao_map[Data::ParticlePresets::CLUSTER], vbo_map[Data::ParticlePresets::CLUSTER]);

	// create vao and vbo for FIRE particley preset
	vao_map[Data::ParticlePresets::FIRE] = vao_map[Data::ParticlePresets::CLUSTER];
	vbo_map[Data::ParticlePresets::FIRE] = vbo_map[Data::ParticlePresets::CLUSTER];

	// create vao and vbo for NONE particle preset
	vao_map[Data::ParticlePresets::NONE] = 0;
	vbo_map[Data::ParticlePresets::NONE] = 0;
	vao_map[Data::ParticlePresets::NONE] = vao_map[Data::ParticlePresets::CLUSTER];
	vbo_map[Data::ParticlePresets::NONE] = vbo_map[Data::ParticlePresets::CLUSTER];

	err = glGetError();
	if (err != GL_NO_ERROR) {
		NIKEE_CORE_ERROR("OpenGL error at the end of {0}: {1}", __FUNCTION__, err);
	}
}

NSPM::~Manager() {
	active_particle_systems.clear();

	for (auto& [preset, vao] : vao_map) {
		glDeleteVertexArrays(1, &vao);
	}
	for (auto& [preset, vbo] : vbo_map) {
		glDeleteBuffers(1, &vbo);
	}
}


NSPM& NSPM::getInstance() {
	static Manager instance;
	return instance;
}

bool NSPM::addActiveParticleSystem(const std::string& ref, Data::ParticlePresets preset, const Vector2f& origin, Data::ParticleRenderType particle_render_type, float duration, bool using_world_pos) {
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
	new_particle_system.using_world_pos = using_world_pos;
	new_particle_system.render_type = particle_render_type;


	active_particle_systems[ref] = new_particle_system;
	next_ps_id++;

	return true;
}


bool NSPM::removeActiveParticleSystem(const std::string& ref) {
	if (active_particle_systems.find(ref) == active_particle_systems.end()) {
		return false;
	}
	active_particle_systems.erase(ref);
	return true;
}


void NSPM::update() {
	const float dt = NIKE_WINDOWS_SERVICE->getDeltaTime();

	for (auto& [ref, ps] : active_particle_systems) {
		// Update particle system
		if (ps.duration != -1 && ps.time_alive >= ps.duration) {
			ps.is_alive = false;
		}

		if (!ps.is_alive && ps.particles.size() == 0) {
			continue;
		}

		ps.time_alive += dt;

		// update particles
		{
			for (auto& p : ps.particles) {
				// Update particle
				p.pos += p.vector * p.velocity * dt;
				p.velocity += p.acceleration * dt;
				p.time_alive += dt;

				if (p.lifespan != -1.f) {
					if (p.preset == Data::ParticlePresets::NONE) {
						if (ps.particle_color_changes_over_time) {
							// linearly interpolate color over time
							p.color = lerp(p.color, ps.particle_final_color, p.time_alive / p.lifespan);
						}
						if (ps.particle_size_changes_over_time) {
							// linearly interpolate size over time
							p.size = lerp(p.size, ps.particle_final_size, p.time_alive / p.lifespan);
						}
					}
					else {
						// fade out
						p.color.a -= dt / p.lifespan;

						// darken
						p.color.r -= dt / p.lifespan;
						p.color.g -= dt / p.lifespan;
						p.color.b -= dt / p.lifespan;
					}
				}

				// particle EnemyDeathState

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
			float LIFESPAN{};
			float ACCELERATION{};
			int NEW_PARTICLES_PER_SECOND{};
			Vector2f PARTICLE_VELOCITY_RANGE{};
			Vector2f VECTOR{};
			float VELOCITY{};
			Vector4f COLOR{};
			float ROTATION{};
			Vector2f SIZE{};
			Vector2f PARTICLE_ORIGIN{};

			switch (ps.preset) {
			case Data::ParticlePresets::NONE: {
				LIFESPAN = ps.particle_lifespan;
				ACCELERATION = ps.particle_acceleration;
				NEW_PARTICLES_PER_SECOND = ps.num_new_particles_per_second;
				PARTICLE_VELOCITY_RANGE = ps.particle_velocity_range;
				float MAX_OFFSET_X = rand_float(ps.particle_rand_x_offset_range, 1);
				float MAX_OFFSET_Y = rand_float(ps.particle_rand_y_offset_range, 1);
				const float vecX = rand_float(ps.particle_vector_x_range, 1);
				const float vecY = rand_float(ps.particle_vector_y_range, 1);
				VECTOR = { vecX, vecY };
				VELOCITY = rand_float(ps.particle_velocity_range, 1);
				COLOR = ps.particle_color_is_random 
					? Vector4f{ static_cast<float>(rand() % 255) / 255.f, static_cast<float>(rand() % 255) / 255.f, static_cast<float>(rand() % 255) / 255.f, 1.f } 
					: ps.particle_color;
				ps.particle_color = COLOR;
				ROTATION = ps.particle_rotation;
				const float width = rand_float(ps.particle_rand_width_range, 1);
				SIZE = { width, width };
				PARTICLE_ORIGIN = {
					ps.origin.x + (static_cast<int>(MAX_OFFSET_X) ? static_cast<float>(rand() % static_cast<int>(MAX_OFFSET_X)) : 0),
					ps.origin.y + (static_cast<int>(MAX_OFFSET_Y) ? static_cast<float>(rand() % static_cast<int>(MAX_OFFSET_Y)) : 0)
				};
				break;
			}
			case Data::ParticlePresets::CLUSTER: {
				PARTICLE_ORIGIN = ps.origin;
				LIFESPAN = 5.f;
				ACCELERATION = 0.f;
				NEW_PARTICLES_PER_SECOND = 50;
				PARTICLE_VELOCITY_RANGE = { 1.f, 10.f };
				VECTOR = { static_cast<float>(rand() % 200 - 100) / 100.f, static_cast<float>(rand() % 200 - 100) / 100.f };
				VECTOR.normalize();
				VELOCITY = PARTICLE_VELOCITY_RANGE.x + static_cast<float>(rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x));
				COLOR = {
						rand() % 255 / 255.f,
						rand() % 255 / 255.f,
						rand() % 255 / 255.f,
						1.f
				};
				ROTATION = 0.f;
				SIZE = { 5.f, 5.f };
				break;
			}
			case Data::ParticlePresets::FIRE: {
				LIFESPAN = 3.f;
				ACCELERATION = 10.f;
				NEW_PARTICLES_PER_SECOND = 100;
				PARTICLE_VELOCITY_RANGE = { 1.f, 10.f };
				int MAX_OFFSET = 10;

				const float offset = static_cast<float>(rand() % MAX_OFFSET);
				float rx = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
				float ry = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
				float len = sqrtf(rx * rx + ry * ry);
				if (len == 0) len = 1;  // Prevent division by zero
				rx /= len;
				ry /= len;
				const float x_offset = rx * offset;
				const float y_offset = ry * offset;
				PARTICLE_ORIGIN = {
					ps.origin.x + x_offset,
					ps.origin.y + y_offset
				};

				VECTOR = { 0.f, static_cast<float>(rand() % 100) / 100.f };
				VECTOR.normalize();
				VELOCITY = PARTICLE_VELOCITY_RANGE.x + static_cast<float>(rand() % static_cast<int>(PARTICLE_VELOCITY_RANGE.y - PARTICLE_VELOCITY_RANGE.x));
				LIFESPAN = LIFESPAN - (x_offset / MAX_OFFSET);
				COLOR = {
					(rand() % 100 + 155.f) / 255.f ,
					0.f,
					0.f,
					1.f
				};
				ROTATION = 0.f;
				SIZE = { 5.f, 5.f };
				break;
			}
			}

			// spawn new particles

			static float time_since_last_spawn = 0.f;
			time_since_last_spawn += dt;

			static float particles_to_spawn = 0.f;
			particles_to_spawn += time_since_last_spawn * NEW_PARTICLES_PER_SECOND;

			if (ps.is_alive && ps.particles.size() > MAX_PARTICLE_SYSTEM_ACTIVE_PARTICLES) {
				particles_to_spawn = 0.f;
				time_since_last_spawn = 0.f;
			}

			if (ps.is_alive && particles_to_spawn >= 1) {
				for (int _{}; static_cast<float>(_) < particles_to_spawn; _++) {
					Particle new_particle;
					new_particle.preset = ps.preset;
					new_particle.pos = PARTICLE_ORIGIN;
					new_particle.vector = VECTOR;
					new_particle.velocity = VELOCITY;
					new_particle.acceleration = ACCELERATION;
					new_particle.lifespan = LIFESPAN;
					new_particle.color = COLOR;
					new_particle.rotation = ROTATION;
					new_particle.is_alive = true;
					new_particle.size = SIZE;

					ps.particles.push_back(new_particle);
				}

				// reset state
				time_since_last_spawn = 0.f;
				particles_to_spawn -= std::floor(particles_to_spawn);
			}

			// end update particles
		}

	}

	// remove dead particle systems
	for (auto it = active_particle_systems.begin(); it != active_particle_systems.end();) {
		if (!it->second.is_alive && it->second.particles.size() == 0) {
			it = active_particle_systems.erase(it);
		}
		else {
			++it;
		}
	}
}

std::vector<ParticleSystem>NSPM::getActiveParticleSystems() const {
	std::vector<ParticleSystem> v;
	v.reserve(active_particle_systems.size());
	for (const auto& [ref, ps] : active_particle_systems) {
		v.push_back(ps);
	}
	return v;
}

std::unordered_map<std::string, NIKE::SysParticle::ParticleSystem>& NSPM::getActiveParticleSystemsMap() {
	return active_particle_systems;
}

void NSPM::setParticleSystemOrigin(const std::string& ref, const Vector2f& origin) {
	active_particle_systems.at(ref).origin = origin;
}

void NSPM::setParticleSystemPreset(const std::string& ref, Data::ParticlePresets preset) {
	active_particle_systems.at(ref).preset = preset;
}

void NSPM::setParticleSystemDuration(const std::string& ref, float duration) {
	active_particle_systems.at(ref).duration = duration;
}

NIKE::SysParticle::ParticleSystem& NSPM::getParticleSystem(const std::string& ref) {
	return active_particle_systems.at(ref);
}

unsigned int NSPM::getVAO(Data::ParticlePresets preset) const {
	return vao_map.at(preset);
}

unsigned int NSPM::getVBO(Data::ParticlePresets preset) const {
	return vbo_map.at(preset);
}

int NSPM::getNewPSID() {
	return next_ps_id;
}

void NSPM::resetEntityParticleSystems() {
	for (auto& [ref, ps] : active_particle_systems) {
		// mark particle system to be removed
		// pe prefix denotes particle emitter (from entity)
		const std::string prefix{ ENTITY_PARTICLE_EMITTER_PREFIX };
		if (ref.substr(0, prefix.size()) == prefix) {
			ps.is_alive = false;
		}
	}


	next_ps_id = 0;
}


// setters for particle system new particle traits

void NSPM::setParticleSystemNumNewParticlesPerSecond(const std::string& ref, int num_new_particles_per_second) {
	active_particle_systems.at(ref).num_new_particles_per_second = num_new_particles_per_second;
}

void NSPM::setParticleSystemParticleLifespan(const std::string& ref, float particle_lifespan) {
	active_particle_systems.at(ref).particle_lifespan = particle_lifespan;
}

void NSPM::setParticleSystemParticleAcceleration(const std::string& ref, float particle_acceleration) {
	active_particle_systems.at(ref).particle_acceleration = particle_acceleration;
}

void NSPM::setParticleSystemParticleVelocityRange(const std::string& ref, const Vector2f& particle_velocity_range) {
	active_particle_systems.at(ref).particle_velocity_range = particle_velocity_range;
}

void NSPM::setParticleSystemParticleVectorXRange(const std::string& ref, const Vector2f& particle_vector_x_range) {
	active_particle_systems.at(ref).particle_vector_x_range = particle_vector_x_range;
}

void NSPM::setParticleSystemParticleVectorYRange(const std::string& ref, const Vector2f& particle_vector_y_range) {
	active_particle_systems.at(ref).particle_vector_y_range = particle_vector_y_range;
}

void NSPM::setParticleSystemParticleColorIsRandom(const std::string& ref, bool particle_color_is_random) {
	active_particle_systems.at(ref).particle_color_is_random = particle_color_is_random;
}

void NSPM::setParticleSystemParticleColor(const std::string& ref, const Vector4f& particle_color) {
	active_particle_systems.at(ref).particle_color = particle_color;
}

void NSPM::setParticleSystemParticleRandXOffsetRange(const std::string& ref, const Vector2f& particle_rand_x_offset_range) {
	active_particle_systems.at(ref).particle_rand_x_offset_range = particle_rand_x_offset_range;
}

void NSPM::setParticleSystemParticleRandYOffsetRange(const std::string& ref, const Vector2f& particle_rand_y_offset_range) {
	active_particle_systems.at(ref).particle_rand_y_offset_range = particle_rand_y_offset_range;
}

void NSPM::setParticleSystemParticleRotation(const std::string& ref, float particle_rotation) {
	active_particle_systems.at(ref).particle_rotation = particle_rotation;
}

void NSPM::setParticleSystemParticleRandWidthRange(const std::string& ref, const Vector2f& particle_rand_width_range) {
	active_particle_systems.at(ref).particle_rand_width_range = particle_rand_width_range;
}

void NSPM::setParticleSystemParticleFinalSize(const std::string& ref, const Vector2f& particle_final_size) {
	active_particle_systems.at(ref).particle_final_size = particle_final_size;
}

void NSPM::setParticleSystemParticleFinalColor(const std::string& ref, const Vector4f& particle_final_color) {
	active_particle_systems.at(ref).particle_final_color = particle_final_color;
}

void NSPM::setParticleSystemParticleRotationSpeed(const std::string& ref, float particle_rotation_speed) {
	active_particle_systems.at(ref).particle_rotation_speed = particle_rotation_speed;
}

void NSPM::setParticleSystemParticleColorChangesOverTime(const std::string& ref, bool particle_color_changes_over_time) {
	active_particle_systems.at(ref).particle_color_changes_over_time = particle_color_changes_over_time;
}

void NSPM::setParticleSystemParticleSizeChangesOverTime(const std::string& ref, bool particle_size_changes_over_time) {
	active_particle_systems.at(ref).particle_size_changes_over_time = particle_size_changes_over_time;
}

