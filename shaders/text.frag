// @file	text.frag
// @brief	text frag shader
// @author	Ho Shu Hng, 2301339, shuhng.ho.poh@digipen.edu
// @date	11 Sep 2024

#version 450 core

out vec4 FragColor;

in vec2 Texcoord;

uniform sampler2D u_texture;
uniform vec3 u_textColor;

void main() {
    vec4 texColor = texture(u_texture, Texcoord);
    float alpha = texture(u_texture, Texcoord).r;
    
    if (alpha < 0.1)
        discard;
        
    FragColor = texColor * vec4(u_textColor, alpha);
}

