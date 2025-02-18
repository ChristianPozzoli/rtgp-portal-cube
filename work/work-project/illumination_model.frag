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

subroutine(ill_model)
vec3 Phong()
{
  vec3 resultColor = Ka * ambientColor;

  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0.0);

  if(lambertian > 0.0)
  {
    vec3 V = normalize(vViewPosition);
    vec3 R = reflect(-L, N);

    float specAngle = max(dot(R, V), 0.0);
    float specular = pow(specAngle, shininess);

    resultColor += Kd * lambertian * color() + Ks * specular * specularColor;
  }

  return  resultColor;
}

subroutine(ill_model)
vec3 BlinnPhong()
{
  vec3 resultColor = Ka * ambientColor;

  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0.0);

  if(lambertian > 0.0)
  {
    vec3 V = normalize(vViewPosition);
    vec3 H = normalize(L + V);

    float specAngle = max(dot(H, N), 0.0);
    float specular = pow(specAngle, shininess);

    resultColor += Kd * lambertian * color() + Ks * specular * specularColor;
  }

  return  resultColor;
}

float G1(float angle, float alpha)
{
  float r = (alpha + 1.0);
  float k = (r * r) / 8.0;

  float num = angle;
  float denom = angle * (1.0 - k) + k;

  return num / denom;
}

subroutine(ill_model)
vec3 GGX()
{
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float NdotL = max(dot(N, L), 0.0);

  vec3 lambert = (Kd * color()) / PI;
  vec3 specular = vec3(0.0);

  if(NdotL > 0.0)
  {
    vec3 V = normalize(vViewPosition);
    vec3 H = normalize(L + V);

    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    float alpha_squared = alpha * alpha;
    float NdotH_squared = NdotH * NdotH;

    float D = alpha_squared;
    float denom = (NdotH_squared * (alpha_squared - 1.0) + 1.0);

    D /= PI * denom * denom;

    vec3 F = vec3(pow(1.0 - VdotH, 5.0));
    F *= (1.0 - F0);
    F += F0;
    
    float G2 = G1(NdotV, alpha) * G1(NdotL, alpha);

    specular = (F * G2 * D) / (4.0 * NdotV * NdotL);
  }

  return (lambert + specular) * NdotL;
}

void main(void)
{
  	colorFrag = vec4(Illumination_Model(), 1.0f);
}
