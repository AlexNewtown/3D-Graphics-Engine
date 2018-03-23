
#include "data structures/Octree.h"

Octree::Octree(std::vector<PointCloud*> points, Bounds* parentBounds)
{
	_bounds = new Bounds();
}
Octree::~Octree()
{

}
void Octree::sampleOpacity(float* position, float& opacity)
{

}
Bounds* Octree::bounds()
{
	return _bounds;
}

void Octree::build(std::vector<PointCloud*> &points, Bounds* bounds, int depth)
{

}
void Octree::computeBounds(std::vector<PointCloud*> &points, Bounds* bounds)
{
	if(points.size() > 0)
	{
		bounds->low->x = points[0]->position[0];
		bounds->low->y = points[0]->position[1];
		bounds->low->z = points[0]->position[2];

		bounds->high->x = points[0]->position[0];
		bounds->high->y = points[0]->position[1];
		bounds->high->z = points[0]->position[2];

		if(points.size() > 1)
		{
			for(int i=1; i < points.size(); i++)
			{
				bounds->low->x = MIN(bounds->low->x, points[i]->position[0]);
				bounds->low->y = MIN(bounds->low->y, points[i]->position[1]);
				bounds->low->z = MIN(bounds->low->z, points[i]->position[2]);

				bounds->high->x = MAX(bounds->high->x, points[i]->position[0]);
				bounds->high->y = MAX(bounds->high->y, points[i]->position[1]);
				bounds->high->z = MAX(bounds->high->z, points[i]->position[2]);
			}
		}
	}
	else
	{
		bounds->low->x = 0.0;
		bounds->low->y = 0.0;
		bounds->low->z = 0.0;
		bounds->high->x = 0.0;
		bounds->high->y = 0.0;
		bounds->high->z = 0.0;
	}
}
float Octree::computeDensity(float* center, std::vector<PointCloud*> &points)
{
	_opacity = 0.0;
	float wSum = 0.0;
	for(int i=0; i < points.size(); i++)
	{
		float d = pow(center[0] - points[i]->position[0], 2.0) + pow(center[1] - points[i]->position[1], 2.0) + pow(center[2] - points[i]->position[2], 2.0);
		float w = 1.0 / d;
		wSum += w;
		_opacity += w*points[i]->color[0];
	}
	_opacity /= (wSum + 0.00000001);
}
bool Octree::pointInBounds(float* position)
{
	
}
