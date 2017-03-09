#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;

out vec4 vPosition;
out vec4 vColor;
out vec4 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vGlobalLightPosition;
out vec3 vGlobalPosition;

/*
layout (std140) uniform lightData
{
	vec4 lightPosition;
	vec4 lightDirection;
	vec4 lightDiffuseColor;
	vec4 lightSpecularColor;
};
*/

uniform vec4 lPosition;