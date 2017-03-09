#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec4 vNormal[];
in int vMaterialIndex[];
in vec2 vTextureCoord[];
in vec4 vBasisX[];
in vec4 vBasisZ[];


out vec4 gNormal;
flat out int gMaterialIndex;
out vec2 gTextureCoord;
flat out vec3 basisX;
flat out vec3 basisZ;

void emitVertex(int index)
{
	gNormal = vNormal[index];
	gMaterialIndex = vMaterialIndex[index];
	gTextureCoord = vTextureCoord[index];

	basisX = vec3(vBasisX[index]);
	basisZ = vec3(vBasisZ[index]);

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