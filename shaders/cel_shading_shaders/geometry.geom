#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec3 light;
    vec3 view;
} gs_in[];

const float MAGNITUDE = 0.4;
  
uniform mat4 projectionMatrix;

void GenerateLine(int index)
{
    gl_Position = projectionMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projectionMatrix * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
	/*
    gl_Position = projectionMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projectionMatrix * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].light, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
	
    gl_Position = projectionMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projectionMatrix * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].view, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
	*/
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}