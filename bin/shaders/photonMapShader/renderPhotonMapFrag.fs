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

uniform sampler2DArray photonMapDiffuseTexture;
vec3 computeBumpMapNormal();

void main()
{
	vec3 color;
	vec3 n = computeBumpMapNormal();
	/*
	float s = 0;
	color = pixelColor(gTextureCoord, gMaterialIndex);
	float direct = 1.0;
	float q = 0.0;
	vec3 n = computeBumpMapNormal();
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		
		//vec3 viewingDirection = -normalize(vec3(gPosition));
		vec3 viewingDirection = -normalize(vec3(globalPosition));
		
		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);
		s = s + f*direct*shadowed(gLocalPosition);
	}

	s = max(0,s);
	color = color*s;
	*/

	vec3 photomMapColor = vec3(texture(photonMapDiffuseTexture,vec3(gTextureCoord,float(gMaterialIndex))));
	color = photomMapColor*diffuseWeight(gPosition,vec4(n,0.0));
	gl_FragColor = vec4(color,1.0);
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}