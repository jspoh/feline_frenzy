// @file	texture.frag
// @brief	textured fragment shader
// @author	Poh Jing Seng, 2301363, jingseng.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

layout (location=1) in vec2 f_texcoord;

layout (location=0) out	vec4 out_color;

uniform sampler2D u_tex2d;
uniform float u_opacity;

uniform bool u_is_font;

void main() {
	vec4 tex_color = texture(u_tex2d, f_texcoord);

	if (!u_is_font) {
		out_color = vec4(tex_color.rgb, tex_color.a * u_opacity);
	} else {
		if (tex_color.r < 0.1) discard;
		out_color = vec4(tex_color.r, tex_color.r, tex_color.r, 1);		// testing for text rendering
	}
}
