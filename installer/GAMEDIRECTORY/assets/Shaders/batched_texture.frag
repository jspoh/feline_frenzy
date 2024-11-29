// @file	batched_texture.frag
// @brief	batched textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec2 f_texcoord;
layout (location=1) flat in int f_sampler_idx;

layout (location=0) out	vec4 out_color;

uniform sampler2DArray u_tex2d;	// 1000 because that is the MAX_INSTANCES for rendering textures

void main() {
	vec4 tex_color = texture(u_tex2d, vec3(f_texcoord, f_sampler_idx));

	out_color = tex_color;
}
