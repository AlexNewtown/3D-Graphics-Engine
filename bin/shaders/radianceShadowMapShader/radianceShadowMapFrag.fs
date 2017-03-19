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

uniform sampler2DArray globalPositionTexture;
uniform sampler2DArray reflectedRadianceTexture;
uniform sampler2DArray normalTexture;

vec3 computeBumpMapNormal();

vec3 indirectLighting();

layout(location = 0) out vec4 globalPositionOutput;
layout(location = 1) out vec4 normalOutput;
layout(location = 2) out vec4 reflectedRadianceOutput;

void main()
{
	vec3 color;
	color = pixelColor(gTextureCoord, gMaterialIndex);
	if(color.x == 0.0 && color.y == 0.0 && color.z == 0.0)
	{
		color = vec3(1.0,1.0,1.0);
	}
	
	vec3 n = computeBumpMapNormal();
	normalOutput = vec4(n,1.0);
	globalPositionOutput = vec4(vec3(gPosition),1.0);

	vec3 lightDir = vec3(lightPosition[0]) - vec3(gPosition);
	lightDir = normalize(lightDir);

	color = color*dot(vec3(gNormal),lightDir);
	reflectedRadianceOutput = vec4(color,1.0);
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}
