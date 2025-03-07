#version 410 core

in vec2 interp_UV;
in vec3 lightDir;

out vec4 colorFrag;

uniform sampler2D normalTexture;
uniform sampler2D lambertdepthTexture;
uniform sampler2D colorTexture;
uniform sampler2D hatchTexture;

uniform vec3 background_color = vec3(1.0);
uniform vec3 edge_color = vec3(0.25);
uniform float color_saturation = 0.4;
uniform float color_brightness = 0.9;
uniform float edge_threshold = 0.15;
uniform float noise_frequency_edge = 100.0;
uniform float noise_strength_edge = 0.002;
uniform float noise_strength_color = 0.002;

const float kernel_offset = 1.0 / 2500.0;


//	Classic Perlin 2D Noise 
//	by Stefan Gustavson (https://github.com/stegu/webgl-noise)
//
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

float cnoise(vec2 P)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, 289.0);
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
    vec4 i = permute(permute(ix) + iy);
    vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
    vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

/////////////////////////////////////////////////////////////////

vec3 rgb2hsb(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsb2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 hatch(float noise)
{
    vec2 noise_uv = interp_UV - noise * noise_strength_color;
    vec3 hatchColor = texture(hatchTexture, interp_UV).rgb;
    vec3 color_sample_hsb = rgb2hsb(texture(colorTexture, noise_uv).rgb);
    float lambertian = texture(lambertdepthTexture, noise_uv).r;
    lambertian = clamp(lambertian * 0.3 + color_sample_hsb.z * 0.7, 0.05, lambertian);
    color_sample_hsb.y = color_saturation;
    color_sample_hsb.z = color_brightness;
    vec3 color_sample_rgb = hsb2rgb(color_sample_hsb);
    
    return lambertian >= 0.9 ? background_color :
            (color_sample_rgb -
            hatchColor.rrr -
            step(lambertian, 0.5) * hatchColor.ggg -
            step(lambertian, 0.25) * hatchColor.bbb);
}

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-kernel_offset,  kernel_offset), // top-left
        vec2( 0.0f,           kernel_offset), // top-center
        vec2( kernel_offset,  kernel_offset), // top-right
        vec2(-kernel_offset,  0.0f),          // center-left
        vec2( 0.0f,           0.0f),          // center-center
        vec2( kernel_offset,  0.0f),          // center-right
        vec2(-kernel_offset, -kernel_offset), // bottom-left
        vec2( 0.0f,          -kernel_offset), // bottom-center
        vec2( kernel_offset, -kernel_offset)  // bottom-right
    );

    // Edge detection kernel
    float kernel[9] = float[](
         1,  1,  1,
         1, -8,  1,
         1,  1,  1
    );

    // Noise in order to give a sketch line and imprecise coloration
    float noise = cnoise(interp_UV * noise_frequency_edge);
    vec2 uv = interp_UV + noise * noise_strength_edge;
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        vec3 sampleTex_normal = texture(normalTexture, uv + offsets[i]).rgb;
        vec3 sampleTex_depth = texture(lambertdepthTexture, uv + offsets[i]).ggg;
        col += sampleTex_normal * kernel[i] + sampleTex_depth * kernel[i];
    }
    
    col = edge_threshold > 0 && dot(col, col) >= edge_threshold ?
            edge_color :
            hatch(noise);
    colorFrag = vec4(col, 1.0f);
}
