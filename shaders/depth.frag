#version 410 core

out vec4 colorFrag;

uniform vec4 colorIn;

float near = 0.1f;
float far = 50.0f;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    colorFrag = vec4(vec3(depth), 1.0f);
}
