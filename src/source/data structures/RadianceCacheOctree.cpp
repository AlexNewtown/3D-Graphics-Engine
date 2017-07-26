#include "data structures\radianceCacheOctree.h"

RadianceCacheOctree::RadianceCacheOctree(std::vector<RadianceCache*> radianceCache, Bounds* bounds)
{
	if (bounds == NULL)
	{
		this->bounds = calculateBounds(radianceCache);
		bounds = this->bounds;
	}
	else
		this->bounds = bounds;

	node = NULL;
	for (int i = 0; i < 8; i++)
		children[i] = NULL;

	if (repeatedNodes(radianceCache))
	{
		combineNodes(radianceCache);
	}

	if (radianceCache.size() == 1)
	{
		this->node = radianceCache[0];
		return;
	}

	std::vector<RadianceCache*> xlylzl;
	std::vector<RadianceCache*> xlylzh;
	std::vector<RadianceCache*> xlyhzl;
	std::vector<RadianceCache*> xlyhzh;
	std::vector<RadianceCache*> xhylzl;
	std::vector<RadianceCache*> xhylzh;
	std::vector<RadianceCache*> xhyhzl;
	std::vector<RadianceCache*> xhyhzh;

	float xPlane = (bounds->low->x + bounds->high->x)*0.5;
	float yPlane = (bounds->low->y + bounds->high->y)*0.5;
	float zPlane = (bounds->low->z + bounds->high->z)*0.5;

	for (int i = 0; i < radianceCache.size(); i++)
	{
		float currentX = radianceCache[i]->pos[0];
		float currentY = radianceCache[i]->pos[1];
		float currentZ = radianceCache[i]->pos[2];
		if (currentX < xPlane)
		{
			if (currentY < yPlane)
			{
				if (currentZ < zPlane)
				{
					xlylzl.push_back(radianceCache[i]);
				}
				else
				{
					xlylzh.push_back(radianceCache[i]);
				}
			}
			else
			{
				if (currentZ < zPlane)
				{
					xlyhzl.push_back(radianceCache[i]);
				}
				else
				{
					xlyhzh.push_back(radianceCache[i]);
				}
			}

		}
		else
		{
			if (currentY < yPlane)
			{
				if (currentZ < zPlane)
				{
					xhylzl.push_back(radianceCache[i]);
				}
				else
				{
					xhylzh.push_back(radianceCache[i]);
				}
			}
			else
			{
				if (currentZ < zPlane)
				{
					xhyhzl.push_back(radianceCache[i]);
				}
				else
				{
					xhyhzh.push_back(radianceCache[i]);
				}
			}
		}
	}

	if (xlylzl.size() != 0)
	{
		Bounds* b = new Bounds(bounds->low->x, 
			bounds->low->y,
			bounds->low->z,
			(bounds->high->x + bounds->low->x)*0.5,
			(bounds->high->y + bounds->low->y)*0.5,
			(bounds->high->z + bounds->low->z)*0.5);
		children[0] = new RadianceCacheOctree(xlylzl,b);
	}

	if (xlylzh.size() != 0)
	{
		Bounds* b = new Bounds(bounds->low->x, 
			bounds->low->y,
			(bounds->high->z + bounds->low->z)*0.5,
			(bounds->high->x + bounds->low->x)*0.5,
			(bounds->high->y + bounds->low->y)*0.5,
			bounds->high->z);
		children[1] = new RadianceCacheOctree(xlylzh,b);
	}

	if (xlyhzl.size() != 0)
	{
		Bounds* b = new Bounds(bounds->low->x, 
			(bounds->high->y + bounds->low->y)*0.5,
			bounds->low->z,
			(bounds->high->x + bounds->low->x)*0.5,
			bounds->high->y,
			(bounds->high->z + bounds->low->z)*0.5);
		children[2] = new RadianceCacheOctree(xlyhzl,b);
	}

	if (xlyhzh.size() != 0)
	{
		Bounds* b = new Bounds(bounds->low->x, 
			(bounds->high->y + bounds->low->y)*0.5,
			(bounds->high->z + bounds->low->z)*0.5,
			(bounds->high->x + bounds->low->x)*0.5,
			bounds->high->y,
			bounds->high->z);
		children[3] = new RadianceCacheOctree(xlyhzh,b);
	}

	if (xhylzl.size() != 0)
	{
		Bounds* b = new Bounds( (bounds->high->x + bounds->low->x)*0.5, 
			bounds->low->y,
			bounds->low->z,
			bounds->high->x,
			(bounds->high->y + bounds->low->y)*0.5,
			(bounds->high->z + bounds->low->z)*0.5);
		children[4] = new RadianceCacheOctree(xhylzl,b);
	}

	if (xhylzh.size() != 0)
	{
		Bounds* b = new Bounds((bounds->high->x + bounds->low->x)*0.5, 
			bounds->low->y,
			(bounds->high->z + bounds->low->z)*0.5,
			bounds->high->x,
			(bounds->high->y + bounds->low->y)*0.5,
			bounds->high->z);
		children[5] = new RadianceCacheOctree(xhylzh,b);
	}

	if (xhyhzl.size() != 0)
	{
		Bounds* b = new Bounds((bounds->high->x + bounds->low->x)*0.5, 
			(bounds->high->y + bounds->low->y)*0.5,
			bounds->low->z,
			bounds->high->x,
			bounds->high->y,
			(bounds->high->z + bounds->low->z)*0.5);
		children[6] = new RadianceCacheOctree(xhyhzl,b);
	}

	if (xhyhzh.size() != 0)
	{
		Bounds* b = new Bounds((bounds->high->x + bounds->low->x)*0.5, 
			(bounds->high->y + bounds->low->y)*0.5,
			(bounds->high->z + bounds->low->z)*0.5,
			bounds->high->x,
			bounds->high->y,
			bounds->high->z);
		children[7] = new RadianceCacheOctree(xhyhzh,b);
	}
	
}

RadianceCacheOctree::~RadianceCacheOctree()
{
	for (int i = 0; i < 8; i++)
	{
		if (children[i] != NULL)
			delete children[i];
	}
	if (bounds != NULL)
		delete bounds;
	if (node != NULL)
		delete node;

}

Bounds* RadianceCacheOctree::calculateBounds(std::vector<RadianceCache*> radianceCache)
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
	return b;
}

bool RadianceCacheOctree::repeatedNodes(std::vector<RadianceCache*> radianceCache)
{
	float eps = 0.00001;
	for (int i = 0; i < radianceCache.size(); i++)
	{
		for (int j = i + 1; j < radianceCache.size(); j++)
		{
			if ( abs(radianceCache[i]->pos[0] - radianceCache[j]->pos[0]) < eps || 
				abs(radianceCache[i]->pos[1] - radianceCache[j]->pos[1]) < eps ||
				abs(radianceCache[i]->pos[2] - radianceCache[j]->pos[2]) < eps)
				return false;
		}
	}
	return true;
}

void RadianceCacheOctree::combineNodes(std::vector<RadianceCache*> &radianceCache)
{
	for (int i = 1; i < radianceCache.size(); i++)
	{
		radianceCache[0]->radiance = radianceCache[0]->radiance + radianceCache[i]->radiance;
	}

	while (radianceCache.size() > 1)
	{
		RadianceCache* r = radianceCache[radianceCache.size() - 1];
		radianceCache.pop_back();
		delete r;
	}
}