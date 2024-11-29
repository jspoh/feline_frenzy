// @file	batched_texture.vert
// @brief	batched texture vertex shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
// @date	7 Nov 2024

#version 450 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoord;
layout (location = 2) in float a_sampler_idx;

layout (location = 4) in vec3 a_model_to_ndc_0;
layout (location = 5) in vec3 a_model_to_ndc_1;
layout (location = 6) in vec3 a_model_to_ndc_2;

layout (location=8) in vec2 framesize;
layout (location=9) in vec2 uv_offset;

layout (location=11) in int a_blend;		// bool
layout (location=12) in vec4 a_color;
layout (location=13) in float a_blend_intensity;


layout (location=0) out vec2 f_texcoord;
layout (location=1) flat out float f_sampler_idx;
//layout (location=2) out vec2 dbg_framesize;

layout (location=3) flat out int f_blend;
layout (location=4) flat out vec4 f_color;
layout (location=5) flat out float f_blend_intensity;

void main() {
	mat3 a_model_to_ndc = mat3(a_model_to_ndc_0, a_model_to_ndc_1, a_model_to_ndc_2);

	//	gl_Position = vec4(a_position, 0.0, 1.0); no uniform
	gl_Position = vec4( vec2(a_model_to_ndc * vec3(a_position, 1.f)), 0.0, 1.0);
	f_texcoord = a_texcoord * framesize + uv_offset;
	f_sampler_idx = a_sampler_idx;

	// debug framesize
	//dbg_framesize = framesize;

	f_blend = a_blend;
	f_color = a_color;
	f_blend_intensity = a_blend_intensity;
}


