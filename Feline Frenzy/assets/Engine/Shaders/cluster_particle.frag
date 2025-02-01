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

layout(location=0) flat in float f_particle_alive;
layout(location=1) flat in vec2 f_particle_position;
layout(location=2) flat in vec4 f_particle_color;
layout(location=3) flat in vec2 f_particle_size;
layout(location=4) flat in vec2 f_particle_velocity;
layout(location=5) flat in float f_particle_timealive;
layout(location=6) flat in float f_particle_lifespan;
layout(location=7) flat in float f_rotation;

void main() {
  if (f_particle_alive == 0)
    discard;

  f_color = vec4(f_particle_color);
}
