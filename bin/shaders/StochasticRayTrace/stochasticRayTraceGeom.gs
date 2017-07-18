#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

/*
in vec4 vPosition[];
in vec4 vLocalPosition[];
in vec4 vColor[];
in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec4 vBasisX[];
in vec4 vBasisZ[];
in vec4 vEntityNormal[];
*/

in vec4 vPositionEye[];
in vec4 vPositionLocal[];
in vec4 vPositionGlobal[];
in vec4 vNormalEye[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec4 vBasisXGlobal[];
in vec4 vBasisZGlobal[];
in vec4 vNormalGlobal[];


out vec4 gPosition;
out vec4 gLocalPosition;
out vec3 gPositionGlobal;
out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;

out vec3 basisX;
out vec3 basisZ;
out vec3 normalGlobal;


void emitVertex(int index)
{
	gPosition = vPositionEye[index];
	gLocalPosition = vPositionLocal[index];
	gPositionGlobal = vec3(vPositionGlobal[index]);

	gNormal = vNormalEye[index];
	gMaterialIndex = vMaterialIndex[index];
	gTextureCoord = vTextureCoord[index];

	basisX = vec3(vBasisXGlobal[index]);
	basisZ = vec3(vBasisZGlobal[index]);
	normalGlobal = vec3(vNormalGlobal[index]);

	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
}

void main()
{
	emitVertex(0);
	emitVertex(1);
	emitVertex(2);
	EndPrimitive();
}