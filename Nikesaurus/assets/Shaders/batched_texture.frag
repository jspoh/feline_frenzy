// @file	batched_texture.frag
// @brief	batched textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec2 f_texcoord;
layout (location=1) flat in float f_sampler_idx;
//layout (location=2) in vec2 dbg_framesize;
layout (location=3) flat in int f_blend;
layout (location=4) flat in vec4 f_color;
layout (location=5) flat in float f_blend_intensity;


layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d[32];	// 32 because that is the MAX_INSTANCES for rendering textures
//uniform sampler2D u_tex2d;

void main() {
	int idx = int(f_sampler_idx);

	vec4 tex_color = texture(u_tex2d[idx], f_texcoord);		// layer is specified with f_sampler_idx for sampler2DArray u_tex2d
	//vec4 tex_color = texture(u_tex2d, f_texcoord);
	//tex_color = texture(u_tex2d, vec2(0.5,0.5));

	if (f_blend == 1) {
		out_color = vec4(mix(tex_color, f_color, f_blend_intensity).rgb, tex_color.a * f_color.a);
		return;
	}

	out_color = tex_color;

	//out_color = vec4(1.0, 0.0, 1.0, 1.0);
	//out_color = vec4(f_sampler_idx/2555555555.0, 0, 0, 1.0);
	//out_color = vec4(f_texcoord, 0.0, 1.0);		// used to check if texcoord is valid
	//out_color = vec4(dbg_framesize, 1.0, 1.0);		// used to check if framesize is valid
}

