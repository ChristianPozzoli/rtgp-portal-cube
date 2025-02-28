/*
06_procedural_base.vert: Vertex shader for the examples on procedural texturing. It is equal to 05_uv2color.vert

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2023/2024
Master degree in Computer Science
Universita' degli Studi di Milano

*/

#version 410 core

// vertex position in world coordinates
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
// UV texture coordinates
layout (location = 2) in vec2 UV;
// the numbers used for the location in the layout qualifier are the positions of the vertex attribute
// as defined in the Mesh class

// model matrix
uniform mat4 modelMatrix;
// view matrix
uniform mat4 viewMatrix;
// Projection matrix
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;
uniform vec3 pointLightPosition;

out vec3 lightDir;
out vec3 vNormal;
flat out vec3 flat_normal;
out vec3 vViewPosition;
out vec2 interp_UV;

void main()
{
	interp_UV = UV;
	vec4 mvPosition = viewMatrix * modelMatrix * vec4(position, 1.0);

	vViewPosition = -mvPosition.xyz;

	vec4 lightPos = viewMatrix * vec4(pointLightPosition, 1.0f);
	lightDir = lightPos.xyz - mvPosition.xyz;
	
	vNormal = normalize(normalMatrix * normal);
	flat_normal = vNormal;

    gl_Position = projectionMatrix * mvPosition;
}