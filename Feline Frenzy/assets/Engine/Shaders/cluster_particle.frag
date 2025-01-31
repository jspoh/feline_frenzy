/*****************************************************************//**
 * \file   base_particle.frag
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   31 Jan 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


#version 450

layout(location=0) out vec4 f_color;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)
uniform vec2 particleOrigin;

#define MAX_PARTICLES 1000

#define PARTICLE_MIN_RADIUS 5.0
#define PARTICLE_MAX_RADIUS 10.0

#define PARTICLE_COUNT_GROWTH_RATE 10

#define PARTICLE_MIN_SPEED 1.0;
#define PARTICLE_MAX_SPEED 5.0;

#define PARTICLE_LIFESPAN 5.0

struct Particle {
  bool alive;

  vec2 position;
  vec4 color;
  float radius;
  vec2 vector;
  float speed;
  float time_alive;
};

void main() {
  static float prev_time = iTime;
  static float delta_time = iTime - prev_time;
  prev_time = iTime;

  vec2 uv = gl_FragCoord.xy;    // current uv coords

  static float num_alive_particles = 0;
  static Particle particles[MAX_PARTICLES];

  // spawn new particles
  if (num_alive_particles < MAX_PARTICLES) {
    for (int i=0; i < PARTICLE_COUNT_GROWTH_RATE && num_alive_particles < MAX_PARTICLES; i++) {
        Particle p;

        // init particle attribs
        p.position = particleOrigin;
        p.radius = INITIAL_PARTICLE_RADIUS;
        p.speed = rand() % PARTICLE_MAX_SPEED + PARTICLE_MIN_SPEED;
        p.time_alive = 0;
        p.color = vec4(rand() % 255 / 255.0, rand() % 255 / 255.0, rand() % 255 / 255.0, 1.0);

        // get spot on circle (could also get random angle)
        vec2 spot = vec2(sin(iTime), cos(iTime));
        p.vector = normalize(spot - particleOrigin);

        // find spot in particles array to place new particle
        for (int j=0; j < MAX_PARTICLES; j++) {
            if (!particles[j].alive) {
                particles[j] = p;
                break;
            }
        }
        
        num_alive_particles++;
    }
  }

  // update alive particles
  for (int i=0; i < MAX_PARTICLES; i++) {
    Particle p = particles[i];

    if (!p.alive) {
      continue;
    }

    p.time_alive += delta_time;

    if (p.time_alive > PARTICLE_LIFESPAN) {
         particles[i].alive = false;
         continue;
    }

    p.position += p.vector * p.speed * delta_time;

    // update particles array
    particles[i] = p;
  }

  // draw alive particles
  for (int i=0; i < MAX_PARTICLES; i++) {
    Particle p = particles[i];

    if (!p.alive) {
        continue;
    }

    // distance between current pixel and particle pos
    float distance = length(uv - p.position);

    // skip some computations if not needed. for now just using double the radius
    if (distance > 2 * p.radius) {
      continue;
    }

    // anti aliasing to draw smooth point
    float alpha = smoothstep(p.radius, p.radius - 1.0, distance);
    f_color = vec4(p.color, alpha);
  }
}
