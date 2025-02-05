#version 450 core

/*****************************************************************//**
 * \file   base_particle.frag
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   31 Jan 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


layout(location=0) out vec4 f_color;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)
uniform vec2 particleOrigin;


void main() {
  vec2 uv = gl_FragCoord.xy;    // current uv coords

  float particle_radius = 5.0;

  // make spinning lol
  vec2 particle_pos = particleOrigin;
  particle_pos.x += 50.0 * sin(iTime * 3);
  particle_pos.y += 50.0 * cos(iTime * 3);

  // distance between current pixel and particle pos
  float distance = length(uv - particle_pos);

  // skip some computations if not needed. for now just using double the radius
  if (distance > 2 * particle_radius) {
    discard;
  }

  // anti aliasing to draw smooth point
  float alpha = smoothstep(particle_radius, particle_radius - 1.0, distance);

  f_color = vec4(1.0, 0.0, 0.0, alpha);
}
