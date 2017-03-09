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

uniform sampler2DArray transmittanceRadiance;
uniform sampler2DArray reflectedRadiance;
uniform sampler2DArray normalDepthTex;
uniform sampler2DArray globalPositionTex;

vec3 computeBumpMapNormal();
vec3 subsurfaceScattering();

uniform float nearPlane;
uniform float farPlane;

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 reflectedColor = vec3(texture(reflectedRadiance,vec3(gTextureCoord.x, gTextureCoord.y,float(gMaterialIndex))));

	/*
	vec3 transmittanceColor = vec3(texture(transmittanceRadiance,vec3(gTextureCoord.x, gTextureCoord.y,float(gMaterialIndex))));
	vec3 depthColor = vec3(texture(normalDepthTex,vec3(gTextureCoord.x, gTextureCoord.y,float(gMaterialIndex))));
	vec3 positionColor = vec3(texture(globalPositionTex,vec3(gTextureCoord.x, gTextureCoord.y,float(gMaterialIndex))));
	vec3 color = vec3(0.0,0.0,0.0);
	
	float s = 0;
	color = pixelColor(gTextureCoord, gMaterialIndex);
	float ambient = 0.00;
	float direct = 1;
	float q = 0.0;
	vec3 n = computeBumpMapNormal();
	
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));
		
		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);

		s = s + f*direct*shadowed(gLocalPosition);
	}
	s = min(s + ambient,1.0);
	s = max(0,s);
	color = color*s;
	color = color + diffuseColor;
	color = color*diffuseWeight(gPosition,gNormal);
	*/

	outColor = vec4(reflectedColor + subsurfaceScattering(),1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}

const float DELTA_SAMPLE = 0.01;
const int NUM_DIPOLE_SAMPLES = 6;
vec3 subsurfaceScattering()
{
	vec3 position = vec3(gPosition);
	vec3 normal = computeBumpMapNormal();
	
	vec3 a = vec3(0.032,0.17,0.48);
	vec3 rs = vec3(0.74,0.88,1.01);
	//vec3 a = vec3(0.0021, 0.0041, 0.0071);
	//vec3 rs = vec3(2.19, 2.62, 3.00);
	float ior = 1.5;

	vec2 texSample;
	vec3 sss = vec3(0.0,0.0,0.0);
	int dipoleSamplesHalf = NUM_DIPOLE_SAMPLES/2;
	for(int x = -dipoleSamplesHalf; x <= dipoleSamplesHalf; x++)
	{
		for(int y = -dipoleSamplesHalf; y <= dipoleSamplesHalf; y++)
		{
			texSample = vec2(gTextureCoord.x + x*DELTA_SAMPLE, gTextureCoord.y + y*DELTA_SAMPLE);
			float depthSample = texture(normalDepthTex,vec3(texSample,float(gMaterialIndex))).x;

			vec3 outPosition = vec3(texture(globalPositionTex,vec3(texSample,float(gMaterialIndex))));
			vec3 transmittanceColor = vec3(texture(transmittanceRadiance,vec3(texSample,float(gMaterialIndex))));
			//vec3 ssSample = transmittanceColor*subsurfaceScatteringInf(position, outPosition, normal, a, rs, 1.44);
			vec3 ssSample = subsurfaceScatteringThin(position, outPosition, normal, a, rs, ior, depthSample);
			sss = sss + ssSample;
		}
	}

	return sss/((NUM_DIPOLE_SAMPLES + 1)*(NUM_DIPOLE_SAMPLES + 1));
}