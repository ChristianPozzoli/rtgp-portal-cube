/*
01_fullcolor.frag : basic Fragment shader, it applies an uniform color to all the fragments. Color is passed as uniform from the main application

N.B.)  "00_basic.vert" must be used as vertex shader

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2023/2024
Master degree in Computer Science
Universita' degli Studi di Milano

*/

#version 410 core

in vec3 vNormal;
flat in vec3 flat_normal;
in vec3 vViewPosition;
in vec3 lightDir;

// output shader variable
out vec4 colorFrag;


void main()
{
    vec3 normal = normalize(cross(dFdx(vViewPosition), dFdy(vViewPosition)));
    vec3 norm_col = (normal / 2.0) + 0.5;
    vec3 flat_norm_col = (flat_normal / 2.0) + 0.5;
    colorFrag = vec4(norm_col, 1.0f);
}
