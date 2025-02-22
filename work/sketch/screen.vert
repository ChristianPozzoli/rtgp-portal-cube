/*
00_basic.vert : basic Vertex shader

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2023/2024
Master degree in Computer Science
Universita' degli Studi di Milano

*/


#version 410 core

// vertex position in world coordinates
// the number used for the location in the layout qualifier is the position of the vertex attribute
// as defined in the Mesh class
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UV;

out vec2 interp_UV;

void main()
{
    interp_UV = UV;
    // transformations are applied to each vertex
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
