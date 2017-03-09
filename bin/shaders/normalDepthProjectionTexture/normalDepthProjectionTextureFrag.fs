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

layout(location = 0) out vec4 normalDepth;

void main()
{
	float depth = computeNormalDepth(vec3(gPosition), computeBumpMapNormal());
	//float maxDepth = abs(farPlane - nearPlane);
	//depth = depth / maxDepth;
	normalDepth = vec4(depth*vec3(1.0,1.0,1.0),1.0);
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}