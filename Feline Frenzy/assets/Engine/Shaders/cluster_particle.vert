/*****************************************************************//**
 * \file   cluster_particle.vert
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   31 Jan 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

 #version 450 core

// 2d passthrough vertex shader

// layout(location=0) in vec2 a_position;


layout(location=0) in float a_particle_alive;
layout(location=1) in vec2 a_particle_position;
layout(location=2) in vec4 a_particle_color;
layout(location=3) in float a_particle_size;
layout(location=4) in float a_particle_velocity;
layout(location=5) in float a_particle_timealive;
layout(location=6) in float a_particle_lifespan;
layout(location=7) in float a_rotation;

layout(location=0) flat out float f_particle_alive;
layout(location=1) flat out vec2 f_particle_position;
layout(location=2) flat out vec4 f_particle_color;
layout(location=3) flat out float f_particle_size;
layout(location=4) flat out float f_particle_velocity;
layout(location=5) flat out float f_particle_timealive;
layout(location=6) flat out float f_particle_lifespan;
layout(location=7) flat out float f_rotation;


void main() {
  vec2 vertices[4] = vec2[4](
    vec2(-1.0,  1.0),  // top-left
    vec2( 1.0,  1.0),  // top-right
    vec2(-1.0, -1.0),  // bottom-left
    vec2( 1.0, -1.0)   // bottom-right
  );

  gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
  // gl_Position = vec4(a_position, 0.0, 1.0);
}
