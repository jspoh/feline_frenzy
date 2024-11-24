// @file	batched_texture.frag
// @brief	batched textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec2 f_texcoord;
layout (location=1) flat in int f_sampler_idx;

layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d[10];	// 1000 because that is the MAX_INSTANCES for rendering textures

void main() {
	vec4 tex_color = texture(u_tex2d[f_sampler_idx], f_texcoord);		// layer is specified with f_sampler_idx for sampler2DArray u_tex2d

	out_color = tex_color;
}

