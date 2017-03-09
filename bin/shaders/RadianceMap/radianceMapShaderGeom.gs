#version 430

layout (points) in;
layout (points, max_vertices = 1) out;

in float vIllum[];

out float gIllum;

void main()
{
	gIllum = vIllum[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}