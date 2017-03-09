#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;
layout(location = 5) in vec4 basisX;
layout(location = 6) in vec4 basisZ;
layout(location = 7) in float vertIndex;

out vec4 vPosition;
out vec4 vLocalPosition;
out vec4 vColor;
out vec4 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vGlobalPosition;
out vec3 vBasisX;
out vec3 vBasisZ;
out int vFaceIndex;
out vec3 vRadiance;

void main()
{
	vLocalPosition = vec4(vec3(position),1.0);
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position );
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = eyeCoords;
	vColor = color;
	vNormal = vec4(normalize(vec3(matrixMult(normalMatrix, normal))),0.0);
	vBasisX = normalize(vec3(matrixMult(normalMatrix, basisX)));
	vBasisZ = normalize(vec3(matrixMult(normalMatrix, basisZ)));

	vTextureCoord = textureCoord;

	vRadiance = getRadiance(int(vertIndex),vBasisX,vec3(vNormal),vBasisZ,-normalize(vec3(eyeCoords)));

	gl_Position = clipCoords;
}