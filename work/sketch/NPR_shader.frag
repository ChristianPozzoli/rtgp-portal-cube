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

uniform sampler2D tex;
uniform float tex_repetition = 1;
uniform float textured;

uniform float thickness;

uniform float timer;

vec3 color()
{
  return colorIn * vec3(mix(vec4(1.0), texture(tex, interp_UV * tex_repetition), textured));
}

vec3 CelShading() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPosition);
    vec3 L = normalize(lightDir);

    float vnDot= dot(V, N);

    if(vnDot <= thickness && vnDot >= -thickness)
    {
        return vec3(0.0);
    }

    float lambertian = max(dot(L, N), 0.0);

    if(lambertian > 0.97)
    {
        return 0.8 * vec3(1.0) + 0.2 * color();
    }
    else if(lambertian > 0.9)
    {
        lambertian = 1;
    }
    else if(lambertian > 0.75)
    {
        lambertian = 0.75;
    }
    else if (lambertian > 0.5)
    {
        lambertian = 0.5;
    }
    else if (lambertian > 0.25)
    {
        lambertian = 0.25;
    }
    else
    {
        lambertian = .1;
    }
    
    return lambertian * color();
}

vec3 SmoothContouringCelShading() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPosition);
    vec3 L = normalize(lightDir);

    float vnDot= abs(dot(V, N));

    float lambertian = max(dot(L, N), 0.0);

    if(lambertian > 0.97)
    {
        return 0.8 * vec3(1.0) + 0.2 * color();
    }
    else if(lambertian > 0.9)
    {
        lambertian = 1;
    }
    else if(lambertian > 0.75)
    {
        lambertian = 0.75;
    }
    else if (lambertian > 0.5)
    {
        lambertian = 0.5;
    }
    else if (lambertian > 0.25)
    {
        lambertian = 0.25;
    }
    else
    {
        lambertian = .1;
    }
    
    return mix(vec3(0), lambertian * color(), smoothstep(0.2, 0.4, vnDot));
}

vec3 ContourEdges() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPosition);

    float vnDot= abs(dot(V, N));
    return mix(vec3(0), vec3(1), smoothstep(0.2, 0.4, vnDot));
}

void main(void)
{
  	colorFrag = vec4(SmoothContouringCelShading(), 1.);
}
