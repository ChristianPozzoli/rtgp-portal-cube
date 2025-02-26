#version 410 core

in vec2 interp_UV;
in vec3 lightDir;

out vec4 colorFrag;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D screenTexture;
uniform sampler2D mapTexture;

uniform vec2 screen_size = vec2(1920, 1080);

uniform float color_factor = 2.0;
uniform float dither_factor = 1.0;

const int indexMatrix[16] = int[] (
                                0, 8, 2, 10,
                                12, 4, 14, 6,
                                3, 11, 1, 9,
                                15, 7, 13, 5
                            );

void main()
{
    vec2 uv = (round(gl_FragCoord.xy / (dither_factor * 4)) * (dither_factor * 4)) / screen_size;
    vec3 color = texture(screenTexture, uv).rgb;
    vec3 rounded_color = round(color * color_factor) / color_factor;
    float dither_color = rounded_color.r < 0.5 ? 0.0: 1.0;
    float diff = abs(rounded_color.r - dither_color);
    int frag_x = int(mod(gl_FragCoord.x / dither_factor, 4));
    int frag_y = int(mod(gl_FragCoord.y / dither_factor, 4));
    color = vec3(diff < indexMatrix[(frag_x + frag_y * 4)] / 16.0 ? dither_color : 1 - dither_color);
    colorFrag = vec4(color, 1.0);
}