#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];

out vec4 gPosition;

void main()
{
	gPosition = vPosition[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	gPosition = vPosition[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	gPosition = vPosition[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}