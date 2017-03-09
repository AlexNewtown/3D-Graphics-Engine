#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 vPosition;

void main()
{	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = eyeCoords;

	gl_Position = clipCoords;
}