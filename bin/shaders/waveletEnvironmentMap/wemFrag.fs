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
flat in int faceIndex;
in vec3 radiance;
flat in vec3 midPoint;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

vec3 shadowedRadiance();

void main()
{
	/*
	mat3 rotMat = radianceRotation(midPoint, vec3(gPosition), basisX, vec3(gNormal), basisZ);
	vec3 r = getRadiance(faceIndex,basisX,vec3(gNormal),basisZ,-normalize(vec3(gPosition)),rotMat);
	vec3 texLookup = vec3(texture(kdTexture,vec3(gTextureCoord.x,1 - gTextureCoord.y, float(gMaterialIndex))));
	*/
	vec3 texLookup = pixelColor(gTextureCoord, gMaterialIndex)*shadowedRadiance();
	outColor = vec4((radiance + texLookup)*diffuseWeight(gPosition,gNormal),1.0);

	fragDepth = gl_FragCoord.z;
}

vec3 shadowedRadiance()
{
	float s = 0;
	float ambient = 0.00;
	float direct = 1.0;

	vec3 n = getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);
		s = f*direct*shadowed(gLocalPosition);
	}

	s = min(s + ambient,1.0);
	s = max(0,s);
	return vec3(1.0,1.0,1.0)*s;
}