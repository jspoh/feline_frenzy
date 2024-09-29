// @file	textured_rendering.vert
// @brief	textured vertex shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec2 a_position;
layout (location=2) in vec2 a_tex_coord;

layout (location=1) out vec2 f_texcoord;

uniform mat3 u_transform;

void main() {
	vec2 in_position_transformed = (u_transform * vec3(a_position, 1.0)).xy;
	gl_Position = vec4(in_position_transformed, 0.0, 1.0);
	f_texcoord = a_tex_coord;
}
