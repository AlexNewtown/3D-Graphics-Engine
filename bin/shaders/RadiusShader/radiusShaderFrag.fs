#version 430

in vec4 gPosition;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

uniform vec4 highlightedColour;
void main()
{
	outColor = vec4(1.0,1.0,1.0,0.1);
	fragDepth = gl_FragCoord.z;
}