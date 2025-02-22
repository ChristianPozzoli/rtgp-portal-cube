#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 tangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform float alongNormalsFactor;

out vec3 vNormal;

void main()
{
	vec4 mvPosition = viewMatrix * modelMatrix * vec4(position, 1.0);
	
	vNormal = normalize(normalMatrix * normal);

    gl_Position = projectionMatrix * mvPosition;
    vec4 outlineOffset = vec4(vNormal.xy * alongNormalsFactor * gl_Position.w, 0.0f, 0.0f);
    gl_Position = gl_Position + outlineOffset;
}