#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;
layout(location = 5) in vec4 basisX;
layout(location = 6) in vec4 basisZ;

out vec4 vPosition;
out vec4 vLocalPosition;
out vec4 vColor;
out vec4 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vGlobalPosition;
out vec4 vBasisX;
out vec4 vBasisZ;

void main()
{
	vLocalPosition = vec4(vec3(position),1.0);
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = eyeCoords;
	vColor = color;
	vNormal = matrixMult(normalMatrix, normal);
	vNormal = vec4(normalize(vec3(vNormal)),1.0);
	vTextureCoord = textureCoord;

	vBasisX = matrixMult(normalMatrix, basisX);
	vBasisX = vec4(normalize(vec3(vBasisX)),1.0);

	vBasisZ = matrixMult(normalMatrix, basisZ);
	vBasisZ = vec4(normalize(vec3(vBasisZ)),1.0);

	vGlobalPosition = vec3(clipCoords);
	
	gl_Position = clipCoords;
}