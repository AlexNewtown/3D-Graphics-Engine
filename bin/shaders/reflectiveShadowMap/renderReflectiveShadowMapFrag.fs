#version 430

in vec4 gPosition;
in vec4 gLocalPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
vec3 globalPosition;
in vec3 basisX;
in vec3 basisZ;
in vec4 entityPosition;
in vec4 entityNormal;

flat in vec2 texVert1;
flat in vec2 texVert2;
flat in vec2 texVert3;

flat in vec3 vert1;
flat in vec3 vert2;
flat in vec3 vert3;

uniform sampler2DArray globalPositionTexture;
uniform sampler2DArray reflectedRadianceTexture;
uniform sampler2DArray normalTexture;
uniform sampler2DArray normalDepthTexture;


vec3 computeBumpMapNormal();

vec3 indirectLighting(int lightMapIndex);
vec3 indirectReflective(int lightMapIndex);
vec3 indirectSubsurfaceScattering(int lightMapIndex);
vec3 reducedIncidentRadiance(int lightMapIndex);

layout(location = 0) out vec4 outColor;
layout(location = 1) out float fragDepth;

void main()
{
	vec3 color;
	vec3 s = vec3(0.0,0.0,0.0);
	color = pixelColor(gTextureCoord, gMaterialIndex);
	float direct = 1.0/float(numLightMaps);
	vec3 n = computeBumpMapNormal();

	/* Compute direct lighting */
	for(int i=0; i< numLightMaps; i++)
	{
		vec4 lightCameraTransform = matrixMult(lightCameraMatrix[i],gLocalPosition);
		vec3 lightDirection = -normalize(vec3(lightCameraTransform));
		vec3 viewingDirection = -normalize(vec3(gPosition));

		float f = sampleBRDF(lightDirection, viewingDirection, n, gMaterialIndex, gTextureCoord);
		float sh = f*shadowed(gLocalPosition);
		sh = min(sh,1.0);
		sh = max(sh,0.0);
		s = s + direct*sh;
	}
	s = max(vec3(0.0,0.0,0.0),s);

	vec3 indirect = vec3(0.0,0.0,0.0);
	for(int i=0; i < numLightMaps; i++)
	{
		indirect = indirect + direct*color*indirectLighting(i);
	}
	color = color*s + indirect;
	outColor = vec4(color,1.0);
	fragDepth = gl_FragDepth;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(gNormal));
}

const int numRandomSamplesBounce = 5;
const int numRandomSamplesSS = 5;
vec3 indirectLighting(int lightMapIndex)
{
	vec3 ind = vec3(0.0,0.0,0.0);
	ind = ind + indirectReflective(lightMapIndex);
	ind = ind + indirectSubsurfaceScattering(lightMapIndex);
	//ind = ind + reducedIncidentRadiance(lightMapIndex);

	return ind;
}

vec3 indirectReflective(int lightMapIndex)
{
	randomSeed = vec2(2.0,18.9);
	vec3 totalRadiance = vec3(0.0,0.0,0.0);

	vec3 x = vec3(entityPosition);
	vec3 en = vec3(entityNormal);
	vec3 lightDir;

	vec4 lightCameraTransform = matrixMult(lightCameraMatrix[lightMapIndex],gLocalPosition);

	vec2 lightTexPos;
	lightCameraTransform = perspectiveDivide(lightCameraTransform);
	lightCameraTransform = windowCoordinates(lightCameraTransform);
	lightTexPos = vec2(lightCameraTransform.x,lightCameraTransform.y);		

	float r1;
	float r2;
	float radiusMax = 0.15;

	if(lightTexPos.x < 0 || lightTexPos.x > 1.0 || lightTexPos.y < 0 || lightTexPos.y > 1.0)
	{
		lightTexPos = vec2(0.5,0.5);
		radiusMax = 0.5;
	}

	for(int j=0; j < numRandomSamplesBounce; j++)
	{
		r1 = random();
		r2 = random();

		float s = radiusMax*r1*sin(2*3.141592*r2);
		float t = radiusMax*r1*cos(2*3.141592*r2);

		vec2 randPos = lightTexPos + vec2(s,t);

		vec3 rrs = vec3( texture(reflectedRadianceTexture,vec3(randPos, float(lightMapIndex))) );
		if(abs(rrs.x + rrs.y + rrs.z) > 0.0)
		{
			vec3 xs = vec3( texture(globalPositionTexture,vec3(randPos,float(lightMapIndex))) );
			vec3 ns = vec3(texture(normalTexture,vec3(randPos, float(lightMapIndex))));

			vec3 rad;
			rad = abs(rrs);
			vec3 a = normalize(x - xs);
			vec3 b = -a;

			rad = rad*max(0.0, dot(ns, a));
			rad = rad*max(0.0, dot(en, b));
			totalRadiance = totalRadiance + rad;
		}
	}
	totalRadiance = totalRadiance / float(numRandomSamplesBounce);
	return totalRadiance;
}

vec3 indirectSubsurfaceScattering(int lightMapIndex)
{
	randomSeed = vec2(2.0,18.9);
	vec3 totalRadiance = vec3(0.0,0.0,0.0);

	vec3 x = vec3(entityPosition);
	vec3 en = vec3(entityNormal);
	vec3 lightDir = normalize(x - vec3(lightPosition[lightMapIndex]));

	vec4 lightCameraTransform = matrixMult(lightCameraMatrix[lightMapIndex],gLocalPosition);

	vec2 lightTexPos;
	lightCameraTransform = perspectiveDivide(lightCameraTransform);
	lightCameraTransform = windowCoordinates(lightCameraTransform);
	lightTexPos = vec2(lightCameraTransform.x,lightCameraTransform.y);		

	float r1;
	float r2;
	float radiusMax = 0.001;

	for(int i=0; i < numRandomSamplesSS; i++)
	{
		r1 = random();
		r2 = random();
		float s = radiusMax*r1*sin(2*3.141592*r2);
		float t = radiusMax*r1*cos(2*3.141592*r2);

		vec2 randPos = lightTexPos + vec2(s,t);
						
		vec3 rrs = vec3( texture(reflectedRadianceTexture,vec3(randPos, float(lightMapIndex))) );
		if(abs(rrs.x + rrs.y + rrs.z) > 0.0)
		{
			vec3 xs = vec3( texture(globalPositionTexture,vec3(randPos,float(lightMapIndex))) );
			vec3 ns = vec3(texture(normalTexture,vec3(randPos, float(lightMapIndex))));
					
			vec3 rad = abs(rrs);
			vec3 a = normalize(x - xs);
			vec3 b = -a;
								
			float inputRadScale = dot(-lightDir,en);
			surfaceNormal = en;
			float ff = fresnelTransmittance(-lightDir, vec3(0.0,0.0,0.0));
		
			totalRadiance = totalRadiance + inputRadScale*ff*rad*subsurfaceScatteringInf(x, xs, en, absorption, reducedScattering, indexOfRefraction);
		
		}
	}
	totalRadiance = totalRadiance / float(numRandomSamplesSS);
	return totalRadiance;
}

vec3 reducedIncidentRadiance(int lightMapIndex)
{
	randomSeed = vec2(2.0,18.9);
	vec3 totalRadiance = vec3(0.0,0.0,0.0);

	vec3 x = vec3(entityPosition);
	vec3 en = vec3(entityNormal);
	vec3 lightDir = normalize(x - vec3(lightPosition[lightMapIndex]));

	vec4 lightCameraTransform = matrixMult(lightCameraMatrix[lightMapIndex],gLocalPosition);

	vec2 lightTexPos;
	lightCameraTransform = perspectiveDivide(lightCameraTransform);
	lightCameraTransform = windowCoordinates(lightCameraTransform);
	lightTexPos = vec2(lightCameraTransform.x,lightCameraTransform.y);		

	float r1;
	float r2;
	float radiusMax = 0.03;

	for(int i=0; i < numRandomSamplesSS; i++)
	{
		r1 = random();
		r2 = random();
		float s = radiusMax*r1*sin(2*3.141592*r2);
		float t = radiusMax*r1*cos(2*3.141592*r2);

		vec2 randPos = lightTexPos + vec2(s,t);
						
		vec3 rrs = vec3( texture(reflectedRadianceTexture,vec3(randPos, float(lightMapIndex))) );
		if(abs(rrs.x + rrs.y + rrs.z) > 0.0)
		{
			vec3 xs = vec3( texture(globalPositionTexture,vec3(randPos,float(lightMapIndex))) );
			vec3 ns = vec3(texture(normalTexture,vec3(randPos, float(lightMapIndex))));
					
			vec3 rad = abs(rrs);
			vec3 a = normalize(x - xs);
			vec3 b = -a;
								
			float inputRadScale = dot(-lightDir,en);
			surfaceNormal = en;
			float ff = fresnelTransmittance(-lightDir, vec3(0.0,0.0,0.0));

			rad = rad*max(0.0,dot(lightDir,en));
			totalRadiance = totalRadiance + rad*exp(-(absorption + reducedScattering)*length(x - xs));			
		}
	}
	totalRadiance = totalRadiance / float(numRandomSamplesSS);
	return totalRadiance;
}