/*

08_random_patterns.frag: a fragment shader with different noise-based effects

N.B. 1)  "06_procedural_base.vert" must be used as vertex shader

N.B. 2)  the different effects are implemented using Shaders Subroutines

N.B. 3) we use simplex noise implementation from
        https://github.com/stegu/webgl-noise//wiki
        to generate the fragments colors


author: Davide Gadia

Real-Time Graphics Programming - a.a. 2023/2024
Master degree in Computer Science
Universita' degli Studi di Milano

*/

#version 410 core

const float PI = 3.14159265359;

// output shader variable
out vec4 colorFrag;

in vec3 lightDir;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vViewPosition;
in vec2 interp_UV;

uniform vec3 ambientColor;
uniform vec3 colorIn;
uniform vec3 specularColor;

uniform sampler2D tex;
uniform sampler2D LUT;
uniform float textured;

uniform float thickness;

uniform float timer;

vec3 color()
{
  return colorIn * vec3(mix(vec4(1.0), texture(tex, interp_UV), textured));
}

vec3 CelShading()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPosition);
    vec3 L = normalize(lightDir);

    float vnDot= max(dot(V, N), 0.0);

    // if(vnDot <= thickness && vnDot >= -thickness)
    // {
    //     return vec3(0.0);
    // }

    float lambertian = max(dot(L, N), 0.0);

    vec3 lut_color = texture(LUT, vec2(lambertian, 0.0)).rgb;
    float specular_lambertian = max((lambertian - 0.95), 0.0) * 1 / (1 - 0.95);
    vec3 specular_color = texture(LUT, vec2(specular_lambertian, 0.0)).rgb * vec3(0.5);

    vec3 R = reflect(-L, N);

    float specAngle = max(dot(R, V), 0.0);
    float specular = pow(specAngle, 25);
    
    return lut_color * color() + specular_color;
}

void main(void)
{
  	colorFrag = vec4(CelShading(), 1.);
}
