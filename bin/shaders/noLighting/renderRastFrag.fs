#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
vec3 globalPosition;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

uniform vec4 highlightedColour;
void main()
{
	vec3 g = vec3(gPosition);
	g = normalize(g);
	vec3 n = vec3(gNormal);
	n = normalize(n);
	float s = dot(n,-g);
	//outColor = vec4(s*pixelColor(gTextureCoord,gMaterialIndex),1.0);
	outColor = vec4(s*vec3(1.0,1.0,1.0),1.0);
	fragDepth = gl_FragCoord.z;
}