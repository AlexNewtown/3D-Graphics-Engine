#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 globalPosition;

float lightDecayRate = 600.0;

void main()
{
	vec3 texLookup = vec3(texture(kdTexture,vec3(gTextureCoord.x,1 - gTextureCoord.y, float(gMaterialIndex))));
	
	vec3 totalShading = vec3(0,0,0);
	int i=0;
	float lightDistance;
	float lightDecay;
	for(i=0; i<numLights; i++)
	{
		vec3 lDir = normalize(globalPosition - getLightPosition(i));
		RayHitInfo hitInfo;

		lightDistance = length(globalPosition - getLightPosition(i));
		lightDecay = exp(-lightDistance*lightDistance/lightDecayRate);
		totalShading = totalShading + (ambientColor() + diffuseColor(i))*lightDecay;
	}
	gl_FragColor = vec4( texLookup*totalShading,1.0);
}