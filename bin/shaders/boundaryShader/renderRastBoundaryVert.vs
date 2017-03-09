#version 430

layout(location = 0) in vec4 position;

uniform float radius;

void main()
{	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	gl_Position = clipCoords;
}