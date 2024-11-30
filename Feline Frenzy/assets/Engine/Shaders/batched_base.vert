// @file	batched_base.vert
// @brief	batched base vertex shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
// @date	5 Nov 2024

#version 450 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;

layout (location = 4) in vec3 a_model_to_ndc_0;
layout (location = 5) in vec3 a_model_to_ndc_1;
layout (location = 6) in vec3 a_model_to_ndc_2;

layout (location=0) out vec4 f_color;

void main() {
	mat3 a_model_to_ndc = mat3(a_model_to_ndc_0, a_model_to_ndc_1, a_model_to_ndc_2);

	//	gl_Position = vec4(a_position, 0.0, 1.0); no uniform
	gl_Position = vec4( vec2(a_model_to_ndc * vec3(a_position, 1.f)), 0.0, 1.0);
	f_color = a_color;
}


