#include "data structures\RadianceCacheUniformGrid.h"

RadianceCacheUniformGrid::RadianceCacheUniformGrid(std::vector<RadianceCache*> radianceCache)
{
	__radianceCache = radianceCache;
	bounds = computeBounds(radianceCache);
	int voxelIndex = 0;

	float spacingX = (bounds->high->x - bounds->low->x) / (float)NUM_VOXELS_PER_DIMENSION;
	float spacingY = (bounds->high->y - bounds->low->y) / (float)NUM_VOXELS_PER_DIMENSION;
	float spacingZ = (bounds->high->z - bounds->low->z) / (float)NUM_VOXELS_PER_DIMENSION;

	for (int i = 0; i < NUM_VOXELS_PER_DIMENSION; i++)
	{
		for (int j = 0; j < NUM_VOXELS_PER_DIMENSION; j++)
		{
			for (int k = 0; k < NUM_VOXELS_PER_DIMENSION; k++)
			{
				voxel[voxelIndex] = new RadianceVoxel();
				voxel[voxelIndex]->xIndex = i;
				voxel[voxelIndex]->yIndex = j;
				voxel[voxelIndex]->zIndex = k;
				voxelIndex++;
			}
		}
	}

	for (int i = 0; i < radianceCache.size(); i++)
	{
		RadianceCache* currentRC = radianceCache[i];

		int indexX = floor((currentRC->pos[0] - bounds->low->x) / spacingX);
		int indexY = floor((currentRC->pos[1] - bounds->low->y) / spacingY);
		int indexZ = floor((currentRC->pos[2] - bounds->low->z) / spacingZ);

		voxel[voxelIndexToGlobalIndex(indexX, indexY, indexZ)]->radianceCache.push_back(currentRC);
	}
}
RadianceCacheUniformGrid::~RadianceCacheUniformGrid()
{

}

Bounds* RadianceCacheUniformGrid::computeBounds(std::vector<RadianceCache*> radianceCache)
{
	Bounds* b = new Bounds(radianceCache[0]->pos[0], radianceCache[0]->pos[1], radianceCache[0]->pos[2], radianceCache[0]->pos[0], radianceCache[0]->pos[1], radianceCache[0]->pos[2]);
	for (int i = 1; i < radianceCache.size(); i++)
	{
		b->low->x = minimum(b->low->x, radianceCache[i]->pos[0]);
		b->high->x = maximum(b->high->x, radianceCache[i]->pos[0]);

		b->low->y = minimum(b->low->y, radianceCache[i]->pos[1]);
		b->high->y = maximum(b->high->y, radianceCache[i]->pos[1]);

		b->low->z = minimum(b->low->z, radianceCache[i]->pos[2]);
		b->high->z = maximum(b->high->z, radianceCache[i]->pos[2]);
	}

	float boundsExtra = 0.1;
	
	b->low->x = b->low->x - abs(b->low->x*boundsExtra);
	b->low->y = b->low->y - abs(b->low->y*boundsExtra);
	b->low->z = b->low->z - abs(b->low->z*boundsExtra);

	b->high->x = b->high->x + abs(b->high->x*boundsExtra);
	b->high->y = b->high->y + abs(b->high->y*boundsExtra);
	b->high->z = b->high->z + abs(b->high->z*boundsExtra);

	b->reCalculate();

	return b;
}

int RadianceCacheUniformGrid::voxelIndexToGlobalIndex(int x, int y, int z)
{
	return x*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION + y*NUM_VOXELS_PER_DIMENSION + z;
}

void RadianceCacheUniformGrid::createRadianceCacheUniformGridTexture(RadianceCacheTexture** rct, RadianceCacheTextureLookup** rct_lookup, RadianceCacheVoxelTexture** rcvt)
{
	//*rct = (RadianceCacheTexture*)malloc(sizeof(RadianceCacheTexture)*__radianceCache.size());
	*rct = new RadianceCacheTexture[__radianceCache.size()];
	for (int i = 0; i < __radianceCache.size(); i++)
	{
		(*rct)[i].pos[0] = __radianceCache[i]->pos[0];
		(*rct)[i].pos[1] = __radianceCache[i]->pos[1];
		(*rct)[i].pos[2] = __radianceCache[i]->pos[2];
		(*rct)[i].pos[3] = 7770;

		(*rct)[i].norm[0] = __radianceCache[i]->norm[0];
		(*rct)[i].norm[1] = __radianceCache[i]->norm[1];
		(*rct)[i].norm[2] = __radianceCache[i]->norm[2];

		(*rct)[i].illum = __radianceCache[i]->radiance;
	}

	//*rct_lookup = (RadianceCacheTextureLookup*)malloc(sizeof(RadianceCacheTextureLookup)*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION);
	*rct_lookup = new RadianceCacheTextureLookup[NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION];
	for (int i = 0; i < NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION; i++)
	{
		(*rct_lookup)[i].numCachesPerVoxel = 0;
		(*rct_lookup)[i].startingIndex = 0;
		(*rct_lookup)[i].padding[0] = 7771;
	}
	//*rcvt = (RadiaceCacheVoxelTexture*)malloc(sizeof(RadiaceCacheVoxelTexture)*__radianceCache.size());
	*rcvt = new RadianceCacheVoxelTexture[__radianceCache.size()];
	
	int startingtextureIndex = 0;
	int radiaceCacheVoxelTextureIndex = 0;
	for (int i = 0; i < NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION*NUM_VOXELS_PER_DIMENSION; i++)
	{
		(*rct_lookup)[i].startingIndex = startingtextureIndex;
		(*rct_lookup)[i].numCachesPerVoxel = (int)this->voxel[i]->radianceCache.size();
		for (int j = 0; j < this->voxel[i]->radianceCache.size(); j++)
		{
			(*rcvt)[radiaceCacheVoxelTextureIndex].radianceCacheIndex = this->voxel[i]->radianceCache[j]->index;
			(*rcvt)[radiaceCacheVoxelTextureIndex].padding[0] = 7772;
			radiaceCacheVoxelTextureIndex++;
		}
		startingtextureIndex = startingtextureIndex + this->voxel[i]->radianceCache.size();
	}
	
	return;
}

void RadianceCacheUniformGrid::createRadianceCacheUniformGridTextures()
{
	RadianceCacheTexture* rct;
	RadianceCacheTextureLookup* rct_lookup;
	RadianceCacheVoxelTexture* rcvt;
	createRadianceCacheUniformGridTexture(&rct, &rct_lookup, &rcvt);

	
}