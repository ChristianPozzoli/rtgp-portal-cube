#version 410 core


in vec2 interp_UV;
in vec3 lightDir;

out vec4 colorFrag;

uniform sampler2D screenTexture;

uniform vec2 screen_size = vec2(1920, 1080);

uniform float color_factor = 2.0;
uniform float dither_factor = 1.0;
uniform float time = 1.0;

#define MAT_SIZE 4
const int indexMatrix[MAT_SIZE * MAT_SIZE] = int[] (
                                                0, 8, 2, 10,
                                                12, 4, 14, 6,
                                                3, 11, 1, 9,
                                                15, 7, 13, 5
                                                );

// #define MAT_SIZE 8
// const int indexMatrix[MAT_SIZE * MAT_SIZE] = int[] (
//                                                 24, 10, 12, 26, 35, 47, 49, 37,
//                                                 8, 0, 2, 14, 45, 59, 61, 51,
//                                                 22, 6, 4, 16, 43, 57, 63, 53,
//                                                 30, 20, 18, 28, 33, 41, 55, 39,
//                                                 34, 46, 48, 36, 25, 11, 13, 27,
//                                                 44, 58, 60, 50, 9, 1, 3, 15,
//                                                 42, 56, 62, 52, 23, 7, 5, 17,
//                                                 32, 40, 54, 38, 31, 21, 19, 29
//                                                 );

void main()
{
    vec2 uv = (floor((interp_UV * screen_size) / (dither_factor * MAT_SIZE)) * (dither_factor * MAT_SIZE)) / screen_size;
    vec3 color;
    vec2 random_move = vec2(cos(time), sin(time)) / 800;
    color.r = texture(screenTexture, uv).r;
    color.g = texture(screenTexture, uv + random_move).g;
    color.b = texture(screenTexture, uv - random_move).b;
    vec3 rounded_color = round(color * color_factor) / color_factor;
    vec3 dither_color = step(0.5, rounded_color);
    int frag_x = int(mod(gl_FragCoord.x / dither_factor, MAT_SIZE));
    int frag_y = int(mod(gl_FragCoord.y / dither_factor, MAT_SIZE));
    float bayer_value = 1 - (indexMatrix[(frag_x + frag_y * MAT_SIZE)] / dot(MAT_SIZE, MAT_SIZE));
    vec3 pattern_color;
    pattern_color.r = abs(rounded_color.r - dither_color.r) < bayer_value ? dither_color.r : 1 - dither_color.r;
    pattern_color.g = abs(rounded_color.g - dither_color.g) < bayer_value ? dither_color.g : 1 - dither_color.g;
    pattern_color.b = abs(rounded_color.b - dither_color.b) < bayer_value ? dither_color.b : 1 - dither_color.b;
    
    colorFrag = vec4(pattern_color, 1.0);
}