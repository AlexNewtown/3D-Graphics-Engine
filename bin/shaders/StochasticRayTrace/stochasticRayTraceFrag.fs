#version 430

in vec4 gPosition;
in vec4 gLocalPosition;
in vec3 gPositionGlobal;
in vec4 gNormal;

flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 basisX;
in vec3 basisZ;
in vec3 normalGlobal;

vec3 computeBumpMapNormal();

uniform float nearPlane;
uniform float farPlane;

layout(location = 0) out vec4 outColor;

uniform sampler2D renderedImage;

const int NUM_MONTE_CARLO_SAMPLES = 1;
const int MAX_RAY_BOUNCES = 1;
vec3 globalIlluminationMonteCarlo();
float directRadiance(vec3 p,vec3 n);
vec3 oneBounceindirectRadiance(vec3 p, vec3 n, int numSamples);
vec3 globalIlluminationStratSamples(vec3 p, vec3 n);

vec3 testBVHRayTrace();

void main()
{
	if(true)
	{
	vec3 n = computeBumpMapNormal();
	vec3 indirect = 20.0*oneBounceindirectRadiance(gPositionGlobal, n, NUM_MONTE_CARLO_SAMPLES);
	vec3 direct = vec3(1.0,1.0,1.0)*directRadiance(gPositionGlobal,n);
	direct = direct*(1.0/3.1415926)*pixelColor(gTextureCoord, gMaterialIndex);
	outColor = vec4(0.0*direct + indirect,1.0);
	}
	else
		outColor = vec4(testBVHRayTrace(),1.0);
}

vec3 testBVHRayTrace()
{
	randomSeed = vec2(gl_FragCoord.x,gl_FragCoord.y);
	float sum = 0.0;
	vec3 n = computeBumpMapNormal();
	vec3 bx = basisX;
	vec3 bz = basisZ;
	float thetaRange = 2*3.14159265;
	float phiRange = 0.5*3.14159265;
	int totalNumIters = 10;

	for(int iter = 0; iter < totalNumIters; iter++)
	{
		float thetaRand = thetaRange*random();
		float phiRand = phiRange*random();
		//phiRand = 0.0;
		float x = cos(thetaRand)*sin(phiRand);
		float z = sin(thetaRand)*sin(phiRand);
		float y = cos(phiRand);
		vec3 rayDir = x*bx + z*bz + y*n;
		rayDir = normalize(rayDir);

		vec3 rayPos = gPositionGlobal;
		RayHitInfo hitInfo;
		int faceIndex;
		float f = 0.0;

		if(rayTraceBVH(rayDir, rayPos + rayDir*0.01, hitInfo, faceIndex))
		{
			Face_vertex face = getFaceBVH(faceIndex);
			vec3 hitNormal = vec3(face.normal1);
			f = dot(hitNormal, -rayDir);
			f = max(0.0,f);
			f = 1.0;
			sum = sum + f;
			vec3 hitPos = vec3(hitInfo.min);
		}

	}
	sum = sum / float(totalNumIters);
	return vec3(1.0,1.0,1.0)*sum;

}


uniform int rSeedTime;
vec3 globalIlluminationMonteCarlo()
{
	randomSeed = vec2(gl_FragCoord.x + rSeedTime, gl_FragCoord.y - rSeedTime);
	float thetaRange = 2*3.141592653;
	float phiRange = 0.5*3.141592653;

	vec3 sum = vec3(0.0,0.0,0.0);
	RayHitInfo rhi;
	Face_vertex face;
	int faceIndex = 0;
	vec3 globalNormal = computeBumpMapNormal();

	float rayRadiance;
	float probabilitySum = 0.0;
	for(int i=0; i < NUM_MONTE_CARLO_SAMPLES; i++)
	{
		vec3 rayPos = gPositionGlobal;
		vec3 n = globalNormal;
		vec3 bx = basisX;
		vec3 bz = basisZ;
		rayRadiance = 1.0;

		vec3 bounceSum = vec3(0.0,0.0,0.0);
		for(int bounce = 0; bounce < MAX_RAY_BOUNCES; bounce++)
		{
			float thetaRand = thetaRange*random();
			float phiRand = phiRange*pow(random(),2.0);

			float areaWeight = sin(phiRand);
			float cosineWeight = cos(phiRand);
			float x = cos(thetaRand)*sin(phiRand);
			float z = sin(thetaRand)*sin(phiRand);
			float y = cos(phiRand);
			vec3 rayDir = x*bx + z*bz + y*n;
			rayDir = normalize(rayDir);

			float dist = rayTraceFaces(rayDir, rayPos, rhi, faceIndex);

			if(faceIndex != -1)
			{	
				face = getFace(faceIndex);
				/* compute the hit point and hit normal */
				vec3 hitPoint = vec3(rhi.min);
				vec3 hitNormal = vec3(matrixMult(entityNormalMatrix,face.normal1));

				/* compute the reflected cosine weight from the hit point*/
				float reflectedCosineWeight = dot(hitNormal,-rayDir);
					
				/*cosineWeight is the incident cosine weight from the normal*/
				// cosineWeight


				/*compute the direct radiance from the hit point*/
				float dr = directRadiance(hitPoint,hitNormal);

				/*sample the color at the hit point*/
				vec3 reflectedColor = getMaterialKd(face.matIndex);

				/* compute the ray indirect cosine factor */
				rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*(1.0/3.1415926);


				//rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*dr*reflectedColor;
				//sum = sum + cosineWeight*reflectedCosineWeight*dr*reflectedColor;

				//sum = sum + dr*reflectedColor*rayRadiance;
				bounceSum = bounceSum + dr*reflectedColor*rayRadiance;

				/* update the new ray position and normal/basis */
				rayPos = hitPoint;
				n = hitNormal;
				vec3 fv1 = vec3(face.vertex1);
				vec3 fv2 = vec3(face.vertex2);

				fv1 = vec3(matrixMult(entityMatrix,vec4(fv1,1.0)));
				fv2 = vec3(matrixMult(entityMatrix,vec4(fv2,1.0)));
				bx = fv2 - fv1;
				bx = normalize(bx);

				bz = cross(bx,n);
				bz = normalize(bz);

				if(dr > 0.0)
					break;
			}
			else
			{
				break;
			}
		}

		sum = sum + bounceSum;

	}
	sum = (thetaRange*phiRange)*sum/(float(NUM_MONTE_CARLO_SAMPLES));

	/* add the direction radiance at the rasterize point */
	vec3 lDirectx1 = pixelColor(gTextureCoord, gMaterialIndex)*directRadiance(gPositionGlobal,globalNormal);
	//sum = sum + lDirectx1*(1.0/3.1415926);

	return sum;
}

/*computes the one bounce indirect illumination at a point p*/
vec3 oneBounceindirectRadiance(vec3 p, vec3 n, int numSamples)
{
	randomSeed = vec2(gl_FragCoord.x + rSeedTime, gl_FragCoord.y + rSeedTime);
	float thetaRange = 2*3.141592653;
	float phiRange = 0.5*3.141592653;
	float stratThetaRange = thetaRange/float(2*numSamples);
	float stratPhiRange = phiRange/float(numSamples);

	vec3 sum = vec3(0.0,0.0,0.0);
	RayHitInfo rhi;
	Face_vertex face;
	int faceIndex = 0;
	vec3 globalNormal = computeBumpMapNormal();

	float rayRadiance;
	for(int i=0; i < 2*numSamples; i++)
	{
		for(int j=0; j < numSamples; j++)
		{
			vec3 rayPos = gPositionGlobal;
			vec3 n = globalNormal;
			vec3 bx = basisX;
			vec3 bz = basisZ;
			rayRadiance = 1.0;

			float thetaRand = stratThetaRange*random();
			float phiRand = stratPhiRange*random();
			thetaRand = thetaRand + thetaRange*float(i)/float(2*numSamples);
			phiRand = phiRand + phiRange*float(j)/float(numSamples);

			float areaWeight = sin(phiRand);
			float cosineWeight = cos(phiRand);
			float x = cos(thetaRand)*sin(phiRand);
			float z = sin(thetaRand)*sin(phiRand);
			float y = cos(phiRand);
			vec3 rayDir = x*bx + z*bz + y*n;
			rayDir = normalize(rayDir);

			//float dist = rayTraceFaces(rayDir, rayPos, rhi, faceIndex);
			rayTraceBVH(rayDir, rayPos + rayDir*0.1, rhi, faceIndex);

			if(faceIndex != -1)
			{	
				face = getFaceBVH(faceIndex);

				/* compute the hit point and hit normal */
				vec3 hitPoint = vec3(rhi.min);
				//vec3 hitNormal = vec3(matrixMult(entityNormalMatrix,face.normal1));
				//hitNormal = normalize(hitNormal);
				vec3 hitNormal = vec3(face.normal1);

				/* compute the reflected cosine weight from the hit point*/
				float reflectedCosineWeight = dot(hitNormal,-rayDir);
				
				/*cosineWeight is the incident cosine weight from the normal*/
				// cosineWeight

				/*compute the direct radiance from the hit point*/
				float dr = directRadiance(hitPoint,hitNormal);

				/*sample the color at the hit point*/
				vec3 reflectedColor = getMaterialKd(face.matIndex);


				/* compute the ray indirect cosine factor */
				rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*(1.0/3.1415926);


				//rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*dr*reflectedColor;
				//sum = sum + cosineWeight*reflectedCosineWeight*dr*reflectedColor;

				sum = sum + cosineWeight*reflectedCosineWeight*dr*reflectedColor;
			}
			else
			{
				break;
			}
		}
	
	}
	sum = (thetaRange*phiRange)*sum/float(numSamples*2*numSamples);

	/*reflectance*/
	sum = (1.0/3.141592)*sum*pixelColor(gTextureCoord, gMaterialIndex);
	return sum;
}

vec3 globalIlluminationStratSamples(vec3 p, vec3 n)
{
	int numSamples = NUM_MONTE_CARLO_SAMPLES;
	randomSeed = vec2(gl_FragCoord.x + rSeedTime, gl_FragCoord.y + rSeedTime);
	float thetaRange = 2*3.141592653;
	float phiRange = 0.5*3.141592653;
	float stratThetaRange = thetaRange/float(2*numSamples);
	float stratPhiRange = phiRange/float(numSamples);

	vec3 sum = vec3(0.0,0.0,0.0);
	RayHitInfo rhi;
	Face_vertex face;
	int faceIndex = 0;
	vec3 globalNormal = computeBumpMapNormal();

	float rayRadiance;
	for(int i=0; i < 2*numSamples; i++)
	{
		for(int j=0; j < numSamples; j++)
		{
			vec3 rayPos = gPositionGlobal;
			vec3 n = globalNormal;
			vec3 bx = basisX;
			vec3 bz = basisZ;
			rayRadiance = 1.0;

			float thetaRand = stratThetaRange*random();
			float phiRand = stratPhiRange*random();
			thetaRand = thetaRand + thetaRange*float(i)/float(2*numSamples);
			phiRand = phiRand + phiRange*float(j)/float(numSamples);

			float areaWeight = sin(phiRand);
			float cosineWeight = cos(phiRand);
			float x = cos(thetaRand)*sin(phiRand);
			float z = sin(thetaRand)*sin(phiRand);
			float y = cos(phiRand);
			vec3 rayDir = x*bx + z*bz + y*n;
			rayDir = normalize(rayDir);

			float dist = rayTraceFaces(rayDir, rayPos, rhi, faceIndex);

			if(faceIndex != -1)
			{	
				face = getFace(faceIndex);

				/* compute the reflected cosine weight from the hit point*/
				float reflectedCosineWeight = dot(vec3(face.normal1),-rayDir);
					
				/*cosineWeight is the incident cosine weight from the normal*/
				// cosineWeight

				/* compute the hit point and hit normal */
				vec3 hitPoint = vec3(rhi.min);
				vec3 hitNormal = vec3(matrixMult(entityNormalMatrix,face.normal1));

				/*compute the direct radiance from the hit point*/
				float dr = directRadiance(hitPoint,hitNormal);

				/*sample the color at the hit point*/
				vec3 reflectedColor = getMaterialKd(face.matIndex);


				/* compute the ray indirect cosine factor */
				rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*(1.0/3.1415926);


				//rayRadiance = rayRadiance*cosineWeight*reflectedCosineWeight*dr*reflectedColor;
				//sum = sum + cosineWeight*reflectedCosineWeight*dr*reflectedColor;

				//sum = sum + dr*reflectedColor*rayRadiance;
				sum = sum + dr*reflectedColor*rayRadiance;
			}
			else
			{
				break;
			}
		}
	
	}
	sum = (thetaRange*phiRange)*sum/float(numSamples*2*numSamples);
	return sum;
}

float directRadiance(vec3 p,vec3 n)
{
	if(numPointLights <= 0)
		return 0.0;

	float radianceSum = 0.0;
	for(int i=0; i < numPointLights; i++)
	{
		vec3 lp = getPointLightPosition(i);
		float r = getPointLightIntensity(i);

		vec3 dir = lp - p;
		float lightDistance = length(dir);
		dir = normalize(dir);

		RayHitInfo rhi;
		int faceIndex = 0;
		float faceDistance = rayTraceFaces(dir, p, rhi, faceIndex);

		if(faceIndex == -1 || lightDistance < faceDistance )
		{
			radianceSum += r*dot(dir,n);
		}
	}
	return radianceSum;
}

vec3 computeBumpMapNormal()
{
	return getBumpMapNormal(basisX, basisZ, gTextureCoord, gMaterialIndex, vec3(normalGlobal));
}