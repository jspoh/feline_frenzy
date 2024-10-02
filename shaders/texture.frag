// @file	texture.frag
// @brief	textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=1) in vec2 f_texcoord;

layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d;
uniform float u_opacity;

void main() {
	vec4 tex_color = texture(u_tex2d, f_texcoord);
	out_color = vec4(tex_color.rgb, tex_color.a * u_opacity);
}
