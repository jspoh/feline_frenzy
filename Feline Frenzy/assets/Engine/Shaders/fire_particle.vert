/*****************************************************************//**
 * \file   fire_particle.vert
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   31 Jan 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

 #version 450 core

// 2d passthrough vertex shader

// layout(location=0) in vec2 a_position;


layout(location=0) in vec2 a_particle_position;     // screen pos
layout(location=1) in vec4 a_particle_color;
layout(location=2) in vec2 a_particle_size;
layout(location=3) in float a_rotation;

layout(location=0) flat out vec2 f_particle_position;     // screen pos
layout(location=1) flat out vec4 f_particle_color;
layout(location=2) flat out vec2 f_particle_size;
layout(location=3) flat out float f_rotation;

uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)
uniform vec2 particleOrigin;


void main() {
    vec2 vertices[6] = vec2[6](
      vec2( 1.0, -1.0),  // bottom-right
      vec2( 1.0,  1.0),  // top-right
      vec2(-1.0,  1.0),  // top-left
      vec2(-1.0,  1.0),  // top-left
      vec2(-1.0, -1.0),   // bottom-left
      vec2( 1.0, -1.0)  // bottom-right
    );

   // Convert particle position (pixels) to NDC [-1, 1]
    vec2 ndcPos = (a_particle_position / iResolution) * 2.0 - 1.0;
    
    // Convert size from pixels to NDC scale
    vec2 ndcSize = a_particle_size / iResolution;

    // Scale vertex by size and translate to position
    vec2 vertexPos = vertices[gl_VertexID % 6] * ndcSize + ndcPos;

    gl_Position = vec4(vertexPos, 0.0, 1.0);
  // gl_Position = vec4(a_position, 0.0, 1.0);

  f_particle_position = a_particle_position;
  f_particle_color = a_particle_color;
  f_particle_size = a_particle_size;
  f_rotation = a_rotation;
}
