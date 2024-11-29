#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 u_projection;
uniform mat3 u_transform;

void main()
{
    vec2 in_position_transformed = (u_transform * vec3(vertex.xy, 1.0)).xy;
    gl_Position = vec4(in_position_transformed, 0.0, 1.0);
    TexCoords = vertex.zw;
}  