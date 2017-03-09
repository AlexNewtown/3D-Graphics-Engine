#pragma once

#include "RadianceCacheOctree.h"
#include "stdio.h"
#include <vector>
#include "Point.h"

class RadianceCacheUniformGrid;

#define NUM_VOXELS_PER_DIMENSION 100

struct RadianceVoxel
{
	int xIndex;
	int yIndex;
	int zIndex;

	std::vector<RadianceCache*> radianceCache;
};

struct RadianceCacheTexture
{
	float pos[4];
	float norm[3];
	float illum;
};

struct RadianceCacheTextureLookup
{
	int startingIndex;
	int numCachesPerVoxel;
	int padding[2];
};

struct RadianceCacheVoxelTexture
{
	int radianceCacheIndex;
	int padding[3];
};

class RadianceCacheUniformGrid
{
public:
	RadianceCacheUniformGrid(std::vector<RadianceCache*> radianceCache);
	~RadianceCacheUniformGrid();

	Bounds* bounds;
	RadianceVoxel* voxel[NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION];
	void createRadianceCacheUniformGridTexture(RadianceCacheTexture** rct, RadianceCacheTextureLookup** rct_lookup, RadianceCacheVoxelTexture** rcvt);
	void createRadianceCacheUniformGridTextures();
	std::vector<RadianceCache*> __radianceCache;
private:
	Bounds* computeBounds(std::vector<RadianceCache*> radianceCache);
	int voxelIndexToGlobalIndex(int x, int y, int z);
};
