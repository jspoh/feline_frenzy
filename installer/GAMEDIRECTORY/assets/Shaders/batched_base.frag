// @file	batched_base.frag
// @brief	batched base fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu (100%)
// @date	5 Nov 2024

#version 450 core

// layout (location=0) in vec3 f_color; (uses uniform now)

layout (location=0) in vec4 a_color;

layout (location=0) out vec4 out_color;

void main() {
	out_color = a_color;
	//out_color = vec4(0, 1, 0, 1);		// !TODO: remove this used for testing
}

