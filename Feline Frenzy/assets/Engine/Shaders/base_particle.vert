/*****************************************************************//**
 * \file   base_particle.vert
 * \brief
 *
 * \author Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
 * \date   31 Jan 2024
 * All content � 2024 DigiPen Institute of Technology Singapore, all rights reserved.
 *********************************************************************/

 #version 450 core

// 2d passthrough vertex shader

// layout(location=0) in vec2 a_position;

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
