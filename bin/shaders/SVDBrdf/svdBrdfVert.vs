#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;
layout(location = 5) in vec4 basisX;
layout(location = 6) in vec4 basisZ;

out vec4 vPosition;
out vec3 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vBasisX;
out vec3 vBasisZ; 

const int brdfAccuracy = 25;
out irradiance
{
	float irr[brdfAccuracy];
} vIrradiance;

void main()
{
	vMaterialIndex = int(round(materialIndex));
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = matrixMult(entityMatrix, vec4(vec3(position),1.0));
	
	vec4 nY = matrixMult(normalMatrix, normal);
	vNormal = normalize(vec3(nY));
	vec4 nX = matrixMult(normalMatrix, basisX);
	vBasisX = normalize(vec3(nX));
	vec4 nZ = matrixMult(normalMatrix, basisZ);
	vBasisZ = normalize(vec3(nZ));

	vTextureCoord = textureCoord;
	vIrradiance.irr = irradianceVector(gl_VertexID);

	gl_Position = clipCoords;
}


