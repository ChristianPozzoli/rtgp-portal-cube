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

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

#define MAT_SIZE 4
const int indexMatrix[MAT_SIZE * MAT_SIZE] = int[] (
                                                0, 8, 2, 10,
                                                12, 4, 14, 6,
                                                3, 11, 1, 9,
                                                15, 7, 13, 5
                                                );


float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main()
{
    vec2 uv = (floor((interp_UV * screen_size) / (dither_factor * MAT_SIZE)) * (dither_factor * MAT_SIZE)) / screen_size;
    vec3 color = texture(screenTexture, uv).rgb;
    color += ((random(uv) * 2) - 1) * 0.02;
    vec3 rounded_color = round(color * color_factor) / color_factor;
    float dither_color = rounded_color.r < 0.5 ? 0.0: 1.0;
    float diff = abs(rounded_color.r - dither_color);
    int frag_x = int(mod(gl_FragCoord.x / dither_factor, MAT_SIZE));
    int frag_y = int(mod(gl_FragCoord.y / dither_factor, MAT_SIZE));
    float bayer_value = (indexMatrix[(frag_x + frag_y * MAT_SIZE)] / 16.0);
    float pattern_color = diff <= bayer_value ? dither_color : 1 - dither_color;
    color = texture(colorTexture, uv).rgb;
    color = rgb2hsb(color);
    color.x = mod(color.x + (1 - pattern_color) * 0.5, 1);
    color.y = step(0.05, color.y) * 1.0;
    color.z = (pattern_color * 0.6) + 0.4;
    color = hsb2rgb(color);
    colorFrag = vec4(color, 1.0);
}