#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in float materialIndex;
layout(location = 4) in vec2 textureCoord;

out vec4 vPosition;
out vec4 vColor;
out vec4 vNormal;
out int vMaterialIndex;
out vec2 vTextureCoord;
out vec3 vGlobalPosition;
out float vIllum;

vec3 gNormal;
vec3 globalPosition;
int gMaterialIndex;
vec2 gTextureCoord;

const int numClosestCaches = 10;

struct RadianceCache
{
	vec3 position;
	vec3 normal;
	float illum;
};

void getClosestCaches(int voxelIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches);
bool insertCache(int cacheIndex, inout RadianceCache[numClosestCaches] caches, inout int numCaches);

int getVoxelIndex();
int adjVoxelX(int vIndex, int offset);
int adjVoxelY(int vIndex, int offset);
int adjVoxelZ(int vIndex, int offset);

float computeRadiance(RadianceCache[numClosestCaches] caches, int numCaches);

void main()
{
	vMaterialIndex = int(round(materialIndex));
	
	vec4 eyeCoords = transformEye( position);
	vec4 clipCoords = transformClip( eyeCoords );
	
	vPosition = position;
	vColor = color;
	/*
	vNormal = matrixMult(normalMatrix, normal);
	*/
	vNormal = matrixMult(entityMatrix, vec4(vec3(normal),0.0));
	vNormal = vec4(normalize(vec3(vNormal)),0.0);
	vTextureCoord = textureCoord;

	vGlobalPosition = vec3(matrixMult(entityMatrix,vec4(vec3(position),1.0)));

	vIllum = 1.0;
	gNormal = vec3(vNormal);
	globalPosition = vGlobalPosition;
	gMaterialIndex = int(vMaterialIndex);
	if(false)
	{
		if(clipCoords.x < -1.0 || clipCoords.x > 1.0 || clipCoords.y < -1.0 || clipCoords.y > 1.0 )
		{
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
			vIllum = computeRadiance(caches, totalNumCaches);
		}
	}
	gl_Position = clipCoords;
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