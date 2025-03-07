#version 410 core

in vec2 interp_UV;

out vec4 colorFrag;

uniform vec3 colorIn;
uniform sampler2D tex;
uniform float textured;

vec3 color()
{
  return colorIn * vec3(mix(vec4(1.0), texture(tex, interp_UV), textured));
}

void main()
{
    colorFrag = vec4(color(), 1.0f);
}
