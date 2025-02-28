#version 410 core


in vec2 interp_UV;
in vec3 lightDir;

out vec4 colorFrag;

uniform sampler2D normalTexture;
uniform sampler2D colorTexture;
uniform sampler2D screenTexture;
uniform sampler2D mapTexture;

uniform vec2 screen_size = vec2(1920, 1080);

uniform float color_factor = 2.0;
uniform float dither_factor = 1.0;


#define MAT_SIZE 4
const int indexMatrix[MAT_SIZE * MAT_SIZE] = int[] (
                                                0, 8, 2, 10,
                                                12, 4, 14, 6,
                                                3, 11, 1, 9,
                                                15, 7, 13, 5
                                                );

// #define MAT_SIZE 8
// const int indexMatrix[MAT_SIZE * MAT_SIZE] = int[](
//                                                 0,  32, 8,  40, 2,  34, 10, 42,
//                                                 48, 16, 56, 24, 50, 18, 58, 26,
//                                                 12, 44, 4,  36, 14, 46, 6,  38,
//                                                 60, 28, 52, 20, 62, 30, 54, 22,
//                                                 3,  35, 11, 43, 1,  33, 9,  41,
//                                                 51, 19, 59, 27, 49, 17, 57, 25,
//                                                 15, 47, 7,  39, 13, 45, 5,  37,
//                                                 63, 31, 55, 23, 61, 29, 53, 21
//                                                 );

void main()
{
    vec2 uv = (floor((interp_UV * screen_size) / (dither_factor * MAT_SIZE)) * (dither_factor * MAT_SIZE)) / screen_size;
    vec3 color = texture(screenTexture, uv).rgb;
    vec3 rounded_color = round(color * color_factor) / color_factor;
    float dither_color = rounded_color.r < 0.5 ? 0.0: 1.0;
    float diff = abs(rounded_color.r - dither_color);
    int frag_x = int(mod(gl_FragCoord.x / dither_factor, MAT_SIZE));
    int frag_y = int(mod(gl_FragCoord.y / dither_factor, MAT_SIZE));
    vec3 pattern_color = vec3(diff < indexMatrix[(frag_x + frag_y * MAT_SIZE)] / 16.0 ? dither_color : 1 - dither_color);
    color = pattern_color * texture(colorTexture, uv).rgb;
    colorFrag = vec4(color, 1.0);
}