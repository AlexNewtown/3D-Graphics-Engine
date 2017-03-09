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

flat in vec2 texVert1;
flat in vec2 texVert2;
flat in vec2 texVert3;

flat in vec3 vert1;
flat in vec3 vert2;
flat in vec3 vert3;

in vec3 radiance;

vec3 computeBumpMapNormal();

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 color;
	float s = 0;
	color = pixelColor(gTextureCoord, gMaterialIndex);
	
	float ambient = 0.00;
	float direct = 1;
	vec3 n = computeBumpMapNormal();
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		s = s + direct*shadowed(gLocalPosition); 
	}
	s = min(s + ambient,1.0);
	s = max(0,s);
	color = (color*s + (1.0 - s)*pixelColor(gTextureCoord, gMaterialIndex))*diffuseWeight(gPosition,gNormal);

	outColor = vec4(radiance + color,1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}