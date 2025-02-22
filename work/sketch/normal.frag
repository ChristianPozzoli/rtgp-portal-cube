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
in vec3 lightDir;

// output shader variable
out vec4 colorFrag;


void main()
{
    vec3 norm_col = (normalize(vNormal) / 2.0) + 0.5;
    float lambertian = max(dot(normalize(lightDir), normalize(vNormal)), 0.0);
    if (lambertian > 0.9f)
    {
        norm_col = 1 - norm_col;
    }
    colorFrag = vec4(norm_col, 1.0f);
}
