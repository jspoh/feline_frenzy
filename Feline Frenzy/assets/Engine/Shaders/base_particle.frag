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
uniform vec2 iMouse;


void main() {
  vec2 uv = gl_FragCoord.xy;    // current uv coords
  vec2 mouse = vec2(iMouse.x, iResolution.y - iMouse.y);  // flip y axis

  float particle_radius = 5.0;

  // distance between current pixel and mouse pos
  float distance = length(uv - mouse);

  // skip some computations if not needed. for now just using double the radius
  if (distance > 2 * particle_radius) {
    discard;
  }

  // anti aliasing to draw smooth point
  float alpha = smoothstep(particle_radius, particle_radius - 1.0, distance);

  f_color = vec4(1.0, 0.0, 0.0, alpha);
}
