// @file	texture.frag
// @brief	textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=1) in vec2 f_texcoord;

layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d;
uniform vec3 u_textColor;
uniform float u_opacity;

uniform bool u_is_font;

void main() {
	vec4 tex_color = texture(u_tex2d, f_texcoord);

	if (!u_is_font) {
		out_color = vec4(tex_color.rgb, tex_color.a * u_opacity);
	} else {
		out_color = vec4(u_textColor.rgb, u_textColor.r * u_opacity);	// testing for text rendering
	}
}
