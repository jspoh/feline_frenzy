#version 450 core

/*****************************************************************//**
 * \file   textured_particle.frag
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   7 March 2024
 * All content Copyright 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/


layout(location=0) out vec4 f_color;

uniform sampler2D u_tex2d;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)
uniform vec2 particleScreenOrigin;

layout(location=0) flat in vec2 f_particle_position;
layout(location=1) flat in vec4 f_particle_color;
layout(location=2) flat in vec2 f_particle_size;
layout(location=3) flat in float f_rotation;
layout(location=4) in vec2 f_texcoord;

void main() {
  // f_color = vec4(f_particle_color);
  f_color = texture(u_tex2d, f_texcoord);
  //f_color = vec4(1.0f);
}
