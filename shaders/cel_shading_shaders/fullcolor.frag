#version 410 core

out vec4 colorFrag;

uniform vec3 colorIn;

void main()
{
    colorFrag = vec4(colorIn, 1.0f);
}
