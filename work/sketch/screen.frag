#version 410 core

in vec2 interp_UV;
in vec3 lightDir;

// output shader variable
out vec4 colorFrag;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D screenTexture;
uniform sampler2D hatchTexture;
uniform sampler2D paperTexture;

const float offset = 1.0 / 2500.0;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 hatch()
{
    vec3 paper = texture(paperTexture, interp_UV).rgb + 0.05;
    vec3 hatchColor = texture(hatchTexture, interp_UV * 10.0).rgb;
    vec3 color_sample_hsv = rgb2hsv(texture(screenTexture, interp_UV).rgb);
    float lambertian = color_sample_hsv.z;
    color_sample_hsv.y = 0.5f;
    color_sample_hsv.z = 0.85f;
    vec3 color_sample_rgb = hsv2rgb(color_sample_hsv);
    if(lambertian > 0.9)
    {
        // return paper;
        return vec3(1.0);
    }
    else if(lambertian > 0.5)
    {
        // return paper - hatchColor.rrr;
        return color_sample_rgb - hatchColor.rrr;
    }
    else if (lambertian > 0.25)
    {
        // return paper - hatchColor.rrr - hatchColor.ggg;
        return color_sample_rgb - hatchColor.rrr - hatchColor.ggg;
    }
    
    // return (paper - hatchColor.rrr - hatchColor.ggg - hatchColor.bbb);
    return (color_sample_rgb - hatchColor.rrr - hatchColor.ggg - hatchColor.bbb);
}

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    float kernel[9] = float[](
         1,  1,  1,
         1, -8,  1,
         1,  1,  1
    );
    
    vec3 sampleTex_normal[9];
    vec3 sampleTex_depth[9];
    vec4 paperTexture = texture(paperTexture, interp_UV);
    //float paperOffset = (paperTexture.r + paperTexture.g + paperTexture.b) / 3.0;
    //paperOffset = paperOffset * 2.0 - 1.0;
    //paperOffset /= 100.0;
    for(int i = 0; i < 9; i++)
    {
        sampleTex_normal[i] = vec3(texture(normalTexture, interp_UV + offsets[i]));
        sampleTex_depth[i] = vec3(texture(depthTexture, interp_UV + offsets[i]));
    }
    
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex_normal[i] * kernel[i] + sampleTex_depth[i] * kernel[i];

    col = length(col) >= 0.15 ? vec3(0.0) : hatch();
    colorFrag = vec4(col, 1.0f);
}
