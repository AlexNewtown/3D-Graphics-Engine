#pragma once

#include <stdio.h>
#include <vector>
#include "../entities/PointCloudEntity.h"

class Octree;

class Octree
{
public:
	Octree(std::vector<PointCloud*> points, Bounds* parentBounds);
	virtual ~Octree();
	void sampleOpacity(float* position, float& opacity);
	Bounds* bounds();

private:
	float _center[3];
	std::vector<Point*> _childPoints[8];
	Bounds* _bounds;
	float _opacity;

	Octree* _child[8];

	void build(std::vector<PointCloud*> &points, Bounds* bounds, int depth);
	void computeBounds(std::vector<PointCloud*> &points, Bounds* bounds);
	float computeDensity(float* center, std::vector<PointCloud*> &points);
	bool pointInBounds(float* position);
};