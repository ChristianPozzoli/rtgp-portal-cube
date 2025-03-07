#version 410 core

out vec4 colorFrag;

in vec3 lightDir;
in vec3 vNormal;

uniform float highlight_threshold;

float near = 0.1f;
float far = 50.0f;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec3 LambertBW()
{
  vec3 N = normalize(vNormal);
  vec3 L = normalize(lightDir);

  float lambertian = max(dot(L, N), 0);

  return vec3(mix(lambertian, 1.0, lambertian > highlight_threshold));
}

void main(void)
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
  	colorFrag = vec4(LambertBW().r, depth, 0.0, 1.0);
}
