#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec3 vGlobalPosition[];
in vec3 vBasisX[];
in vec3 vBasisZ[];

out vec4 gPosition;
out vec4 gColor;
out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
out vec3 vectorReturn;
out vec3 globalPosition;
out vec3 basisX;
out vec3 basisZ;

vec2 rr = vec2(0.0,0.0);

void main()
{
	vectorReturn = vec3(0,0,0);

	rr = vec2(vGlobalPosition[0].x,vGlobalPosition[0].y);
	gPosition = vPosition[0];
	gColor = vColor[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	globalPosition = vGlobalPosition[0];
	basisX = vBasisX[0];
	basisZ = vBasisZ[0];
	
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	rr = vec2(vGlobalPosition[1].x,vGlobalPosition[1].y);
	gPosition = vPosition[1];
	gColor = vColor[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	globalPosition = vGlobalPosition[1];
	basisX = vBasisX[1];
	basisZ = vBasisZ[1];

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	rr = vec2(vGlobalPosition[2].x,vGlobalPosition[2].y);
	gPosition = vPosition[2];
	gColor = vColor[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	globalPosition = vGlobalPosition[2];
	basisX = vBasisX[2];
	basisZ = vBasisZ[2];
	
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}