#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vPosition[];
in vec4 vLocalPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec3 vRadiance[];
in vec3 vBasisX[];
in vec3 vBasisZ[];
in int vFaceIndex[];
in vec3 vEye[];

out vec4 gPosition;
out vec4 gLocalPosition;
out vec4 gColor;
flat out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
out vec3 vectorReturn;
out vec3 radiance;
flat out vec3 basisX;
flat out vec3 basisZ;
flat out int faceIndex;
out vec3 eyeCoords;

void main()
{
	vectorReturn = vec3(gl_in[0].gl_Position);
	gPosition = vPosition[0];
	gLocalPosition = vLocalPosition[0];
	gColor = vColor[0];
	gNormal = vNormal[0];
	gMaterialIndex = vMaterialIndex[0];
	gTextureCoord = vTextureCoord[0];
	radiance = vRadiance[0];
	basisX = vBasisX[0];
	basisZ = vBasisZ[0];
	faceIndex = vFaceIndex[0];
	eyeCoords = vEye[0];

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	
	vectorReturn = vec3(gl_in[1].gl_Position);
	gPosition = vPosition[1];
	gLocalPosition = vLocalPosition[1];
	gColor = vColor[1];
	gNormal = vNormal[1];
	gMaterialIndex = vMaterialIndex[1];
	gTextureCoord = vTextureCoord[1];
	radiance = vRadiance[1];
	basisX = vBasisX[1];
	basisZ = vBasisZ[1];
	faceIndex = vFaceIndex[1];
	eyeCoords = vEye[1];

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	vectorReturn = vec3(gl_in[2].gl_Position);
	gPosition = vPosition[2];
	gLocalPosition = vLocalPosition[2];
	gColor = vColor[2];
	gNormal = vNormal[2];
	gMaterialIndex = vMaterialIndex[2];
	gTextureCoord = vTextureCoord[2];
	radiance = vRadiance[2];
	basisX = vBasisX[2];
	basisZ = vBasisZ[2];
	faceIndex = vFaceIndex[2];
	eyeCoords = vEye[2];

	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	
	EndPrimitive();
}