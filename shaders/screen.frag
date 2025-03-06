#version 410 core

out vec4 colorFrag;

in vec2 interp_UV;

uniform sampler2D screenTexture;

void main()
{
    colorFrag = texture(screenTexture, interp_UV);
}
