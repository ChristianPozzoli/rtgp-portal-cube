#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 tangent;

out VS_OUT {
    vec3 normal;
    vec3 light;
    vec3 view;
} vs_out;

uniform vec3 pointLightPosition;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

void main()
{
    vec4 mvPosition = viewMatrix * modelMatrix * vec4(position, 1.0);
	vs_out.view = normalize(-mvPosition).xyz;
	vec4 lightPos = viewMatrix * vec4(pointLightPosition, 1.0f);
	vs_out.light = normalize(lightPos.xyz - mvPosition.xyz);
	vs_out.normal = normalize(viewMatrix * modelMatrix * vec4(tangent, 1.0)).xyz;
    //vs_out.normal = normalize(normalMatrix * normal);
    //vs_out.view = cross(vs_out.normal, normalize(lightPos.xyz - mvPosition.xyz));
    gl_Position = viewMatrix * modelMatrix * vec4(position, 1.0); 
}