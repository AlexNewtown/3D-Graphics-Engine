#version 430

in vec4 gPosition;
in vec4 gLocalPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
vec3 globalPosition;
flat in vec3 basisX;
flat in vec3 basisZ;

flat in vec2 texVert1;
flat in vec2 texVert2;
flat in vec2 texVert3;

flat in vec3 vert1;
flat in vec3 vert2;
flat in vec3 vert3;

vec3 computeBumpMapNormal();

uniform float nearPlane;
uniform float farPlane;

uniform int transmittance;

layout(location = 0) out vec4 transmittedRadiance;
layout(location = 1) out vec4 reflectedRadiance;

void main()
{
	vec3 color = pixelColor(gTextureCoord, gMaterialIndex);
	

	//color = color*photonIrradianceReflected(vec3(gPosition),computeBumpMapNormal());

	/*color = color*photonIrradianceTransmitted(vec3(gPosition),computeBumpMapNormal());*/
	
	//color = color*photonIrradiance(vec3(gPosition),computeBumpMapNormal());

	knnSearch(vec3(gPosition));
	vec3 n = computeBumpMapNormal();
	transmittedRadiance = vec4(color*photonIrradianceTransmitted(vec3(gPosition),n,false),1.0);
	reflectedRadiance = vec4(color*photonIrradianceReflected(vec3(gPosition),n,false),1.0);
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}