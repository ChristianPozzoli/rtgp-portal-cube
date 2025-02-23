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
in vec3 vViewPosition;
in vec2 interp_UV;


uniform vec3 ambientColor;
uniform vec3 colorIn;
uniform vec3 specularColor;

uniform float Ka;
uniform float Kd;
uniform float Ks;

uniform float shininess;
uniform float alpha;
uniform float F0;

uniform float timer;

uniform sampler2D tex;
uniform float textured;

subroutine vec3 ill_model();

// Subroutine Uniform (it is conceptually similar to a C pointer function)
subroutine uniform ill_model Illumination_Model;

vec3 color()
{
  return colorIn * vec3(mix(vec4(1.0), texture(tex, interp_UV), textured));
}

//////////////////////////////////////////
// a subroutine for a simple noise shader
subroutine(ill_model)
vec3 Lambert()
{
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0.0);

  return Kd * lambertian * color();
}

vec3 FullLambert()
{
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0.0);

  return lambertian * color();
}

vec3 LambertBW()
{
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0.0);

  return vec3(lambertian);
}

void main(void)
{
    float lambertian = max(dot(normalize(lightDir), normalize(vNormal)), 0.05);

  	colorFrag = vec4(mix(lambertian * color(), vec3(1.0), lambertian > 0.9), 1.0);
}
