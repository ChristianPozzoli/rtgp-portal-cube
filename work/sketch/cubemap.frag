#version 410 core

out vec4 colorFrag;

in vec2 interp_UV;

uniform sampler2D tCube;

void main()
{
    colorFrag = texture(tCube, interp_UV * 40.0);
}