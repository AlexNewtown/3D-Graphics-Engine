#pragma once
#include <stdio.h>
#include <vector>
#include "Point.h"

struct RadianceCache;
class RadianceCacheOctree;

struct RadianceCache
{
	int index;
	float pos[4];
	float norm[3];
	float radiance;
};

class RadianceCacheOctree
{
public:
	RadianceCacheOctree(std::vector<RadianceCache*> radianceCache, Bounds* bounds);
	~RadianceCacheOctree();
	RadianceCacheOctree* children[8];
	Bounds* bounds;
	RadianceCache* node;
private:
	Bounds* calculateBounds(std::vector<RadianceCache*> radianceCache);
	bool repeatedNodes(std::vector<RadianceCache*> radianceCache);
	void combineNodes(std::vector<RadianceCache*> &radianceCache);
};