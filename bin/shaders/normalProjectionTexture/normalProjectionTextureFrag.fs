#version 430

in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
flat in vec3 basisX;
flat in vec3 basisZ;

vec3 computeBumpMapNormal();

uniform float nearPlane;
uniform float farPlane;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 n = computeBumpMapNormal();
	outColor = vec4(n,1.0);
	fragDepth = 1.0;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}