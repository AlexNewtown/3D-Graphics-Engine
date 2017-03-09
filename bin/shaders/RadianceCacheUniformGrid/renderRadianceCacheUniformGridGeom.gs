#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec3 vGlobalPosition[];
in float vIllum[];

out vec4 gPosition;
out vec4 gColor;
out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
out vec3 vectorReturn;
out vec3 globalPosition;
out float gIllum;

void main()
{
	vectorReturn = vec3(0,0,0);

	gPosition = vPosition[0];
	gColor = vColor[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	globalPosition = vGlobalPosition[0];
	gIllum = vIllum[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	
	gPosition = vPosition[1];
	gColor = vColor[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	globalPosition = vGlobalPosition[1];
	gIllum = vIllum[1];

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	gPosition = vPosition[2];
	gColor = vColor[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	globalPosition = vGlobalPosition[2];
	gIllum = vIllum[2];
	
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}