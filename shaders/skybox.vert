#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 interp_UVW;

void main(){
  interp_UVW = position;
  vec4 pos = projectionMatrix * viewMatrix * vec4( position, 1.0 );

  gl_Position = pos.xyww;
}
