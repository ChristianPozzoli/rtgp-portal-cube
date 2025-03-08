#version 410 core

in vec2 interp_UV;

out vec4 colorFrag;

uniform sampler2D screenTexture;
uniform sampler2D previousScreenTexture;

uniform float offset_amount = 2500.0;

void main()
{
    vec4 screenSample = texture(screenTexture, interp_UV);
    vec4 previousScreenSample = texture(previousScreenTexture, interp_UV);

    colorFrag = screenSample * 0.5 + previousScreenSample * 0.5;
}
