#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 globalPosition;
in float illum;

vec3 localRayOut();

void main()
{
	vec3 texLookup = pixelColor();
	float p = dot(localRayOut(),vec3(gNormal));
	texLookup = texLookup*p*illum*500;
	
	gl_FragColor = vec4( texLookup,1.0);
}

vec3 localRayOut()
{
	vec3 ro = normalize(vec3(-gPosition));
	return ro;
}