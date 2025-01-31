#version 450 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D u_text2D;
uniform vec3 u_textColor;
uniform float u_opacity;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_text2D, TexCoords).r);
    color = vec4(u_textColor, u_opacity) * sampled;
}  