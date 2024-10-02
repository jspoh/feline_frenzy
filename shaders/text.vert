// @file	text.vert
// @brief	text vertex shader
// @author	Ho Shu Hng, 2301339, shuhng.ho.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;

out vec2 Texcoord;

uniform mat3 u_transform; 
void main() {
    vec3 position = vec3(a_position, 1.0); 
    vec3 transformed_position = u_transform * position;

    gl_Position = vec4(transformed_position.xy, 0.0, 1.0);
    Texcoord = a_texcoord;
}
