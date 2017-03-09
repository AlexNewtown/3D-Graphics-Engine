#version 430

in vec4 gPosition;
in vec4 gColor;
in vec4 gNormal;
flat in int gMaterialIndex;
in vec2 gTextureCoord;
in vec3 globalPosition;
in float gIllum;

const int numClosestCaches = 6;

struct RadianceCache
{
	vec3 position;
	vec3 normal;
	float illum;
};

RadianceCache getCache(int cIndex);
void getClosestCaches(int voxelIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches);
bool insertCache(int cacheIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches);

int getVoxelIndex();
int adjVoxelX(int vIndex, int offset);
int adjVoxelY(int vIndex, int offset);
int adjVoxelZ(int vIndex, int offset);

float computeRadiance(RadianceCache[numClosestCaches] caches, int numCaches);
float computeRadiance();
vec3 localRayOut();


void main()
{
	vec3 texLookup = pixelColor();
	/*
	RadianceCache[numClosestCaches] caches;
	int totalNumCaches = 0;
	int voxelIndex = getVoxelIndex();
	
	getClosestCaches(voxelIndex,caches,totalNumCaches);
	
	if(totalNumCaches < numClosestCaches)
	{
		getClosestCaches(adjVoxelX(voxelIndex,-1),caches,totalNumCaches);
		getClosestCaches(adjVoxelX(voxelIndex,1),caches,totalNumCaches);

		getClosestCaches(adjVoxelY(voxelIndex,-1),caches,totalNumCaches);
		getClosestCaches(adjVoxelY(voxelIndex,1),caches,totalNumCaches);

		getClosestCaches(adjVoxelZ(voxelIndex,-1),caches,totalNumCaches);
		getClosestCaches(adjVoxelZ(voxelIndex,1),caches,totalNumCaches);
	}
	*/
	float r = computeRadiance();
	vec3 rayOut = localRayOut();
	float p = abs(dot(vec3(gNormal),rayOut));
	gl_FragColor = vec4( texLookup*r*p,1.0);
}

vec3 localRayOut()
{
	vec3 ro = normalize(vec3(-gPosition));
	ro = vec3(matrixMult(modelViewMatrixInverse, vec4(ro,0.0)));
	ro = vec3(matrixMult(entityMatrix, vec4(ro,0.0)));
	ro = normalize(ro);
	return ro;
}

float computeRadiance(RadianceCache[numClosestCaches] caches, int numCaches)
{
	float r = 0.0;
	if(numCaches <= 0)
		return 0.0;

	/* compute errors */

	float meanDistance = 0;
	float[numClosestCaches] cacheDistances;
	float[numClosestCaches] errors;

	for(int i=0; i< numCaches; i++)
	{
		cacheDistances[i] = length(globalPosition - caches[i].position);
		meanDistance = meanDistance + cacheDistances[i];
	}
	meanDistance = meanDistance/float(numCaches);

	float totalError = 0.0;
	for(int i=0; i<numCaches; i++)
	{
		errors[i] = cacheDistances[i]/meanDistance + sqrt(1 - dot(vec3(gNormal),caches[i].normal));
		totalError = totalError + 1.0/(errors[i]);
	}

	for(int i=0; i<numCaches; i++)
	{
		r = r + caches[i].illum/errors[i];
	}
	r = r/totalError;

	return r;
}

float computeRadiance()
{
	int voxelIndex = getVoxelIndex();
	int numCachesInVoxel = getRadianceCacheTextureLookupNumCaches(voxelIndex);
	int startingLookup = getRadianceCacheTextureLookupStartingIndex(voxelIndex);

	if(numCachesInVoxel <= 0)
		return 0.0;


	int j = startingLookup;
	float meanDistance = 0.0;
	float rad = 0.0;


	for(int i=0; (i<numCachesInVoxel) || (i<10); i++)
	{
		int cLookup = getVoxelCacheLookup(j);
		RadianceCache c = getCache(cLookup);
		float len = pow(length(globalPosition - c.position),0.2);
		rad = rad + exp(-len)*abs(dot(vec3(gNormal),c.normal))*c.illum;
		j++;
	}
	return rad/20;

	RadianceCache[numClosestCaches] caches;
	j = startingLookup;
	for(int i=0; i < numClosestCaches; i++)
	{
		int cLookup = getVoxelCacheLookup(j);
		caches[i] = getCache(cLookup);
		meanDistance = meanDistance + length(globalPosition - caches[i].position);
		j++;
	}
	meanDistance = meanDistance/float(numClosestCaches);
	
	float radiance = 0;
	float weight;
	float error;
	float totalWeight = 0.0;
	for(int i=0; i<numClosestCaches; i++)
	{
		error = pow(length(globalPosition - caches[i].position),1.0)/ meanDistance + sqrt(1.0 + dot(vec3(gNormal),caches[i].normal));
		weight = 1.0/error;
		totalWeight = totalWeight + weight;
		radiance = radiance + weight*caches[i].illum;
	}
	radiance = radiance;
	return radiance;
}

void getClosestCaches(int voxelIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches)
{
	if(voxelIndex < 0)
		return;

	int numCachesInVoxel = getRadianceCacheTextureLookupNumCaches(voxelIndex);
	int startingLookup = getRadianceCacheTextureLookupStartingIndex(voxelIndex);

	if(numCachesInVoxel <= 0)
		return;

	for(int i=0; i<numCachesInVoxel; i++)
	{
		int cacheIndex = getVoxelCacheLookup(startingLookup);
		insertCache(cacheIndex,caches,numCaches);
		startingLookup++;
	}
}

bool insertCache(int cacheIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches)
{

	if(numCaches < numClosestCaches)
	{
		RadianceCache c = getCache(cacheIndex);
		caches[numCaches] = c;
		numCaches++;
		return true;
	}
	return false;

	RadianceCache rc = getCache(cacheIndex);
	float insertDistance = length(globalPosition - rc.position);
	for(int i=0; i< numCaches; i++)
	{
		float d = length(caches[i].position - globalPosition);
		if(insertDistance < d)
		{
			for(int j=numCaches-1; j > i; j--)
			{
				caches[j] = caches[j-1];
			}
			caches[i] = rc;
			if(numCaches < numClosestCaches - 1)
				numCaches++;
			return true;
		}
	}
	if(numCaches < numClosestCaches)
	{
		caches[numCaches] = rc;
		numCaches++;
		return true;
	}
	return false;
}

int getVoxelIndex()
{
	float spacingX = (cacheBoundsMax.x - cacheBoundsMin.x) / float(numVoxelsPerDimension);
	float spacingY = (cacheBoundsMax.y - cacheBoundsMin.y) / float(numVoxelsPerDimension);
	float spacingZ = (cacheBoundsMax.z - cacheBoundsMin.z) / float(numVoxelsPerDimension);

	int indexX = int(floor((globalPosition.x - cacheBoundsMin.x) / spacingX));
	int indexY = int(floor((globalPosition.y - cacheBoundsMin.y) / spacingY));
	int indexZ = int(floor((globalPosition.z - cacheBoundsMin.z) / spacingZ));

	int ind = indexX*numVoxelsPerDimension*numVoxelsPerDimension + indexY*numVoxelsPerDimension + indexZ;
	if(ind > numVoxelsPerDimension*numVoxelsPerDimension*numVoxelsPerDimension)
		return numVoxelsPerDimension*numVoxelsPerDimension*numVoxelsPerDimension;
	if(ind < 0)
		return 0;

	return ind;
}

int adjVoxelX(int vIndex, int offset)
{
	int index = vIndex + offset*numVoxelsPerDimension*numVoxelsPerDimension;
	if(index < 0 || index >= numVoxelsPerDimension*numVoxelsPerDimension*numVoxelsPerDimension)
		return -1;
	return index;
}
int adjVoxelY(int vIndex, int offset)
{
	int index = vIndex + offset*numVoxelsPerDimension;
	if(index < 0 || index >= numVoxelsPerDimension*numVoxelsPerDimension*numVoxelsPerDimension)
		return -1;
	return index;
}
int adjVoxelZ(int vIndex, int offset)
{
	int index = vIndex + offset;
	if(index < 0 || index >= numVoxelsPerDimension*numVoxelsPerDimension*numVoxelsPerDimension)
		return -1;
	return index;
}