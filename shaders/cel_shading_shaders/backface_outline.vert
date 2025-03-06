#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform float alongNormalsFactor;
uniform vec3 geometric_center = vec3(0.0);

out vec3 vNormal;

void main()
{
	vec4 mvPosition = viewMatrix * modelMatrix * vec4(position, 1.0);
	
	vNormal = normalize(normalMatrix * normal);

    gl_Position = projectionMatrix * mvPosition;
    vec4 outlineOffset = vec4(vNormal.xy * alongNormalsFactor * gl_Position.w, 0.0f, 0.0f);
    gl_Position = gl_Position + outlineOffset;
}

// void main()
// {
//     mat4 translation_m = mat4(1.0);
//     translation_m[3] = vec4(- geometric_center, 1.0);
//     mat4 scale_m = mat4(1.0);
//     scale_m[0][0] = alongNormalsFactor;
//     scale_m[1][1] = alongNormalsFactor;
//     scale_m[2][2] = alongNormalsFactor;
//     mat4 neg_translation_m =  mat4(1.0);
//     neg_translation_m[3] = vec4(geometric_center, 1.0);

// 	vec4 mvPosition = viewMatrix * modelMatrix * neg_translation_m * scale_m * translation_m * vec4(position, 1.0);
	
// 	vNormal = normalize(normalMatrix * normal);

//     gl_Position = projectionMatrix * mvPosition;
// }