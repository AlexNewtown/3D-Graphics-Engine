#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 color;

out vec4 vCameraPosition;
out vec4 vGlobalPosition;
out vec4 vLocalPosition;
out vec4 vNormal;
out vec4 vColor;

void main()
{
	vec4 eyeCoords = transformEye( position);
	vGlobalPosition = eyeCoords;
	vec4 clipCoords = transformClip( eyeCoords );
	vCameraPosition = clipCoords;
	vLocalPosition = position;

	vNormal = matrixMult(normalMatrix, normal);
	vNormal = vec4(normalize(vec3(vNormal)), 1.0);

	vColor = color;
	gl_Position = clipCoords;
}