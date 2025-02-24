#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;

out vec2 interp_UV;

void main()
{
    interp_UV = UV;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
