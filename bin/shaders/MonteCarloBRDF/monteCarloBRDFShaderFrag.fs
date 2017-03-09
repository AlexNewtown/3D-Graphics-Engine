#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 globalPosition;
in vec3 basisX;
in vec3 basisZ;

void main()
{
	vec3 texLookup = pixelColor();	
	float shading = computeMonteCarloRadiance();	
	shading = shading/100;
	gl_FragColor = vec4( texLookup*shading,1.0);
}

