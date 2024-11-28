// @file	batched_texture.frag
// @brief	batched textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec2 f_texcoord;
layout (location=1) flat in int f_sampler_idx;
//layout (location=2) in vec2 dbg_framesize;

layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d[10];	// 1000 because that is the MAX_INSTANCES for rendering textures
//uniform sampler2D u_tex2d;

void main() {
	vec4 tex_color = texture(u_tex2d[f_sampler_idx], f_texcoord);		// layer is specified with f_sampler_idx for sampler2DArray u_tex2d
	//vec4 tex_color = texture(u_tex2d, f_texcoord);
	//tex_color = texture(u_tex2d, vec2(0.5,0.5));

	out_color = tex_color;
	//out_color = vec4(1.0, 0.0, 1.0, 1.0);
	//out_color = vec4(f_sampler_idx, f_sampler_idx, f_sampler_idx, 1.0);
	//out_color = vec4(f_texcoord, 0.0, 1.0);		// used to check if texcoord is valid
	//out_color = vec4(dbg_framesize, 1.0, 1.0);		// used to check if framesize is valid
}

