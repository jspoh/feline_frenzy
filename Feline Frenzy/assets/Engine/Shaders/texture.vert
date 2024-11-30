// @file	texture.vert
// @brief	textured vertex shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout(location = 0) in vec2 a_position;
layout(location = 2) in vec2 a_tex_coord;

layout(location = 1) out vec2 f_texcoord;

uniform mat3 u_transform;
uniform vec2 uvOffset;
uniform vec2 frameSize;
//uniform bool u_flipvertical;
//uniform bool u_fliphorizontal;

void main() {
    vec2 in_position_transformed = (u_transform * vec3(a_position, 1.0)).xy;
    gl_Position = vec4(in_position_transformed, 0.0, 1.0);

    vec2 adjusted_texcoord = a_tex_coord;

    //Apply vertical flip
    //if (u_flipvertical) {
    //    adjusted_texcoord.y = 1.0 - adjusted_texcoord.y;
    //}

    //Apply horizontal flip
    //if (u_fliphorizontal) {
    //    adjusted_texcoord.x = 1.0 - adjusted_texcoord.x;
    //}

    f_texcoord = adjusted_texcoord * frameSize + uvOffset;
}