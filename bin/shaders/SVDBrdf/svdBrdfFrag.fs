#version 430

in vec4 gPosition;
flat in vec3 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
flat in vec3 basisX;
flat in vec3 basisZ;

const int brdfAccuracy = 25;

in irradiance
{
	float irr[brdfAccuracy];
}outIrradiance;

float irr[brdfAccuracy];


void main()
{
	vec3 c = pixelColor();
	viewingVector(vec3(gPosition),basisX,gNormal,basisZ);
	float f = 100.0*radiance(outIrradiance.irr);
	gl_FragColor = vec4(f*c,1.0);
}