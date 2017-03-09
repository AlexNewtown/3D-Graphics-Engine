#version 430

uniform int farPlane;
uniform int nearPlane;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	/*gl_FragDepth = gl_FragCoord.z;*/
	outColor = vec4(gl_FragCoord.z,0.0,0.0,1.0);

	/*fragDepth = gl_FragCoord.z;*/
	fragDepth = gl_FragDepth;
}