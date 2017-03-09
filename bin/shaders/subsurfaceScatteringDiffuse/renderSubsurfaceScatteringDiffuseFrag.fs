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

uniform sampler2DArray subsurfaceScatteringRadiance;

vec3 computeBumpMapNormal();

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 diffuseColor = vec3(texture(subsurfaceScatteringRadiance,vec3(gTextureCoord.x, gTextureCoord.y,gMaterialIndex)));

	vec3 color;
	vec3 s = vec3(0.0,0.0,0.0);
	color = pixelColor(gTextureCoord, gMaterialIndex);
	vec3 ambient = vec3(1.0,1.0,1.0)*0.01;
	float direct = 0.1;
	float q = 0.0;
	vec3 n = computeBumpMapNormal();
	
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		
		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);
		s = s + f*direct*shadowedTranslucent(gLocalPosition,vec3(gPosition),n)*dot(lightDirection,n);

	}
	s = s + ambient;
	s.x = min(s.x,1.0);
	s.y = min(s.y,1.0);
	s.z = min(s.z,1.0);	

	s.x = max(0.0,s.x);
	s.y = max(0.0,s.y);
	s.z = max(0.0,s.z);

	color = color*s;
	color = color + diffuseColor;
	
	outColor = vec4(color,1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}