// @file	base.frag
// @brief	base fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=0) in vec4 f_color;

layout (location=0) out vec4 out_color;

void main() {
	out_color = f_color;
}

