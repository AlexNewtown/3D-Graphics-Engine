#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

const int brdfAccuracy = 25;

in vec4 vPosition[];
in vec3 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec3 vBasisX[];
in vec3 vBasisZ[]; 
in irradiance
{
	float irr[brdfAccuracy];
}vIrradiance[];

out vec4 gPosition;
flat out vec3 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
flat out vec3 basisX;
flat out vec3 basisZ;
out irradiance
{
	float irr[brdfAccuracy];
}outIrradiance;

void main()
{
	gPosition = vPosition[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	basisX = vBasisX[0];
	basisZ = vBasisZ[0];
	gl_Position = gl_in[0].gl_Position;
	outIrradiance.irr = vIrradiance[0].irr;
	EmitVertex();
	
	
	gPosition = vPosition[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	basisX = vBasisX[1];
	basisZ = vBasisZ[1];
	gl_Position = gl_in[1].gl_Position;
	outIrradiance.irr = vIrradiance[1].irr;
	EmitVertex();
	

	gPosition = vPosition[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	basisX = vBasisX[2];
	basisZ = vBasisZ[2];
	gl_Position = gl_in[2].gl_Position;
	outIrradiance.irr = vIrradiance[2].irr;
	EmitVertex();
	

	EndPrimitive();
}