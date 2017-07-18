#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;
layout(location = 5) in vec4 basisX;
layout(location = 6) in vec4 basisZ;
layout(location = 7) in float vertIndex;

out vec4 vPositionEye;
out vec4 vPositionLocal;
out vec4 vPositionGlobal;

out vec4 vNormalEye;

out int vMaterialIndex;
out vec2 vTextureCoord;

out vec4 vBasisXGlobal;
out vec4 vBasisZGlobal;
out vec4 vNormalGlobal;


void main()
{
	vPositionLocal = vec4(vec3(position),1.0);
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPositionGlobal = matrixMult(entityMatrix,vec4(vec3(position),1.0));
	vPositionEye = eyeCoords;

	vNormalEye = matrixMult(normalMatrix, normal);
	vNormalEye = vec4(normalize(vec3(vNormalEye)),1.0);

 	vTextureCoord = textureCoord;
	
 	vBasisXGlobal = matrixMult(entityNormalMatrix, basisX);
	vBasisXGlobal = vec4(normalize(vec3(vBasisXGlobal)),1.0);

	vBasisZGlobal = matrixMult(entityNormalMatrix, basisZ);
	vBasisZGlobal = vec4(normalize(vec3(vBasisZGlobal)),1.0);

	vNormalGlobal = matrixMult(entityNormalMatrix, normal);
	vNormalGlobal = vec4(normalize(vec3(vNormalGlobal)),0.0);

	gl_Position = clipCoords;

}