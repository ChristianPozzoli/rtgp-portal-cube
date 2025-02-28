#version 410 core


in vec2 interp_UV;
in vec3 lightDir;

out vec4 colorFrag;

uniform sampler2D normalTexture;
uniform sampler2D colorTexture;
uniform sampler2D screenTexture;
uniform sampler2D mapTexture;

uniform vec2 screen_size = vec2(1920, 1080);

uniform float offset_amount = 2500.0;
const int samples = 9;

vec4 compute_region(vec2 l, vec2 u) {
    vec3 sum = vec3(0.0);
    vec3 sq_sum = vec3(0.0);
    for(int i = int(l.x); i <= u.x; ++i) {
        for(int j = int(l.y); j <= u.y; ++j) {
            vec2 offset = vec2((1 / offset_amount) * i, (1 / offset_amount) * j);
            vec2 uv_o = interp_UV + offset;
            vec3 sample_tex = texture(screenTexture, uv_o).rgb;

            sum += sample_tex;
            sq_sum += sample_tex * sample_tex;
        }
    }

    vec3 mean = sum / samples;
    float variance = length(abs((sq_sum / samples) - (mean * mean)));

    return vec4(mean, variance);
}

void main()
{
    vec4 reg_A = compute_region(vec2(-2, -2), vec2(0, 0));
    vec4 reg_B = compute_region(vec2(0, -2), vec2(2, 0));
    vec4 reg_C = compute_region(vec2(-2, 0), vec2(0, 2));
    vec4 reg_D = compute_region(vec2(0, 0), vec2(2, 2));

    vec3 color = reg_A.xyz;
    float minVar = reg_A.w;

    float test = step(reg_B.w, minVar);
    color = mix(color, reg_B.xyz, test);
    minVar = mix(minVar, reg_B.w, test);

    test = step(reg_C.w, minVar);
    color = mix(color, reg_C.xyz, test);
    minVar = mix(minVar, reg_C.w, test);

    test = step(reg_D.w, minVar);
    color = mix(color, reg_D.xyz, test);
    
    colorFrag = vec4(color, 1.0);
}