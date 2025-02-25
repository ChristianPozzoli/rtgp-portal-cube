#version 410 core

#define PI 3.14159265359;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;

// model matrix
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
// view matrix
uniform mat4 viewMatrix;
// Projection matrix
uniform mat4 projectionMatrix;
uniform float viewAngleY;

out vec2 interp_UV;


void main(){
  interp_UV = UV;
  interp_UV.y -= viewAngleY / PI;
  vec4 pos = projectionMatrix * viewMatrix * vec4(position, 1.0);

  gl_Position = pos.xyww;
}
