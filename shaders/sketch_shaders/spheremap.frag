#version 410 core

out vec4 colorFrag;

in vec2 interp_UV;

uniform sampler2D hatch;

uniform float hatching_repeat = 40.0;

void main()
{
    colorFrag = texture(hatch, interp_UV * hatching_repeat);
}