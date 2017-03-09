#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;
layout(location = 5) in vec4 basisX;
layout(location = 6) in vec4 basisZ;

out vec4 vPosition;
out vec4 vColor;
out vec4 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vGlobalPosition;
out vec3 vBasisX;
out vec3 vBasisZ;

void main()
{
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = eyeCoords;
	vColor = color;
	vNormal = matrixMult(normalMatrix, vec4(vec3(normal),0.0));
	vNormal = vec4(normalize(vec3(vNormal)),0.0);
	vTextureCoord = textureCoord;

	vGlobalPosition = vec3(matrixMult(entityMatrix,vec4(vec3(position),1.0)));
	vBasisX = normalize(vec3(matrixMult(normalMatrix, basisX)));
	vBasisZ = normalize(vec3(matrixMult(normalMatrix, basisZ)));

	gl_Position = clipCoords;
}

