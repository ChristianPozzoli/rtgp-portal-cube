#version 410 core

in vec2 interp_UV;

out vec4 colorFrag;

uniform sampler2D screenTexture;

uniform float offset_amount = 2500.0;

void main()
{
    float kernel_offset = 1 / offset_amount;
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
    
    // Gaussian blur approximation kernel
    float kernel_fract = 16.0;
    float kernel[9] = float[](
         1, 2, 1,
         2, 4, 2,
         1, 2, 1
    );

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        vec3 sampleTex = texture(screenTexture, interp_UV + offsets[i]).rgb;
        col += sampleTex * kernel[i] / kernel_fract;
    }

    colorFrag = vec4(col, 1.0f);
}
