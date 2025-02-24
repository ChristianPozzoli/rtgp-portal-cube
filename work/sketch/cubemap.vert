#version 410 core

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
uniform float viewAngle;

out vec2 interp_UV;

const float PI = 3.14159265359;

void main(){
  interp_UV = UV;//position.xy;
  interp_UV.y -= viewAngle / PI;
  vec4 pos = projectionMatrix * viewMatrix * vec4(position, 1.0);

  gl_Position = pos.xyww;
}
