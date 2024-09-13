// @file	base.vert
// @brief	base vertex shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;

layout (location=0) out vec4 f_color;

void main() {
	gl_Position = vec4(a_position, 0.0, 1.0);
	f_color = a_color;
}


