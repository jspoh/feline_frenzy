// @file	base.frag
// @brief	base fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @co-author Sean Gwee, g.boonxuensean@digipen.edu
// @date	11 Sep 2024

#version 450 core

// layout (location=0) in vec3 f_color; (uses uniform now)

uniform vec3 f_color;
uniform float f_opacity;

layout (location=0) out vec4 out_color;

void main() {
	out_color = vec4(f_color, f_opacity);
}

