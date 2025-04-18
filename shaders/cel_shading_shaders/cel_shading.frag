#version 410 core

const float PI = 3.14159265359;

out vec4 colorFrag;

in vec3 lightDir;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vViewPosition;
in vec2 interp_UV;

uniform vec3 colorIn;

uniform sampler2D tex;
uniform float tex_repetition = 1;
uniform sampler2D LUT;
uniform float textured;

uniform float thickness;

uniform float timer;

uniform float offset_factor = 250.0;
uniform float outline_threshold_lower = 2.5;
uniform float outline_threshold_upper = 3.0;

uniform int poster_factor_sobel = 1;
uniform int poster_factor_final = 1;

uniform float gloss_threshold_lower = 0.95;
uniform float gloss_threshold_upper = 1.0;
uniform float gloss_factor = 0.5;

vec3 textureKernel() {
  float offset = 1.0 / offset_factor;

  vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );

    float outline_kernel[9] = float[](
         1,  1,  1,
         1, -8,  1,
         1,  1,  1
    );

    int SobelRight[9] = int[](
        //Prewitt operator sobel
        //1, 0, -1,
        //1, 0, -1,
        //1, 0, -1
        
        //Classic sobel
        //1, 0, -1,
        //2, 0, -2,
        //1, 0, -1

        // Sobel–Feldman operator
        3, 0, -3,
        10, 0, -10,
        3, 0, -3
    );

    int SobelDown[9] = int[](
        //Prewitt operator sobel
        //1, 1, 1,
        //0, 0, 0,
        //-1, -1, -1
        
        //Classic sobel
        //1, 2, 1,
        //0, 0, 0,
        //-1, -2, -1
        
        // Sobel–Feldman operator
        3, 10, 3,
        0, 0, 0,
        -3, -10, -3
    );

    
    vec3 sampleTex[9];
    vec3 sampledTextureCenter = texture(tex, interp_UV * tex_repetition).rgb;
    if (poster_factor_final != 1) {
        sampledTextureCenter = round(sampledTextureCenter * poster_factor_final) / poster_factor_final;
    }
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = texture(tex, (interp_UV * tex_repetition + offsets[i])).rgb;
        if (poster_factor_sobel != 1) {
            sampleTex[i] = round(sampleTex[i] * poster_factor_sobel) / poster_factor_sobel;
        }
    }
    
    vec3 col_r = vec3(0.0);
    vec3 col_d = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        col_r += sampleTex[i] * SobelRight[i];
        col_d += sampleTex[i] * SobelDown[i];
    }

    float edge = sqrt(dot(col_r, col_r) + dot(col_d, col_d));

    return mix(sampledTextureCenter, vec3(0.0), smoothstep(outline_threshold_lower, outline_threshold_upper, edge));
}

vec3 color()
{
  return colorIn * mix(vec3(1.0), textureKernel(), textured);
}

vec3 CelShading()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPosition);
    vec3 L = normalize(lightDir);

    //float vnDot= max(dot(V, N), 0.0);

    // if(vnDot <= thickness && vnDot >= -thickness)
    // {
    //     return vec3(0.0);
    // }

    float lambertian = max(dot(L, N), 0.0);

    vec3 lut_color = texture(LUT, vec2(lambertian, 0.0)).rgb;
    float specular_lambertian = smoothstep(gloss_threshold_lower, gloss_threshold_upper, lambertian);
    vec3 specular_color = specular_lambertian * vec3(gloss_factor);
    
    return lut_color * color() + specular_color;
}

void main(void)
{
  	colorFrag = vec4(CelShading(), 1.);
}
