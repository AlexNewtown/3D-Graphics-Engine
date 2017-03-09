#version 430

layout(location = 0) in vec4 position;

uniform int farPlane;
uniform int nearPlane;

bool isCylindricalProjection = false;

void main()
{	
	vec4 eyeCoords = transformEye( vec4(vec3(position),1.0));
	vec4 clipCoords = transformClip( eyeCoords );
	if(isCylindricalProjection)
	{
		clipCoords = cylindricalProjection(vec3(eyeCoords),nearPlane,farPlane);
	}
	gl_Position = clipCoords;
}