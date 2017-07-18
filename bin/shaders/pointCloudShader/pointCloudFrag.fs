#version 430

in vec4 gCameraPosition;
in vec4 gGlobalPosition;
in vec4 gLocalPosition;
in vec4 gNormal;
in vec4 gColor;
flat in vec4 fragCenter;

void main()
{
	vec2 fc = vec2(fragCenter);
	vec2 cp = vec2(gCameraPosition);
	float dist = length(fc - cp);
	float w = exp(-dist/0.25);

	vec3 viewDir = -vec3(gCameraPosition);
	viewDir = normalize(viewDir);

	float dp = dot(viewDir,vec3(gNormal));
	dp = max(0.0, dp);
	gl_FragColor = vec4(dp*vec3(gColor),w);
}
