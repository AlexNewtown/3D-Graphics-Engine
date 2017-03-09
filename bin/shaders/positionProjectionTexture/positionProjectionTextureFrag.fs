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

uniform vec3 boundsMin;
uniform vec3 boundsMax;

layout(location = 0) out vec4 positionTexture;

void main()
{
	/*
	vec4 bMinGlobal = transformEye(vec4(boundsMin,1.0));
	vec4 bMaxGlobal = transformEye(vec4(boundsMax,1.0));

	vec3 boundsDistance = vec3(bMaxGlobal) - vec3(bMinGlobal);

	outColor.x = (gPosition.x - bMinGlobal.x)/boundsDistance.x;
	outColor.y = (gPosition.y - bMinGlobal.y)/boundsDistance.y;
	outColor.z = (gPosition.z - bMinGlobal.z)/boundsDistance.z;
	outColor.w = 1.0;
	*/

	positionTexture = vec4(vec3(gPosition),1.0);
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}