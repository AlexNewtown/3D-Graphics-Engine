#ifndef RAY_H
#define RAY_H

#include "Point.h"
#include "MeshStructures.h"

struct Ray;
struct RayHitInfo;

struct Ray
{
	Ray();
	~Ray();

	Point pos;
	Point dir;
};

struct RayHitInfo
{
	RayHitInfo();
	~RayHitInfo();
	void copy(RayHitInfo* rh);
	void reset();
	Ray ray;
	Point max;
	Point min;
	float tmin;
	float tmax;
	
	float txmin;
	float txmax;
	float tymin;
	float tymax;
	float tzmin;
	float tzmax;

	void computeMinMax(Ray* r);
	unsigned int getSmallestT();
};

Ray* newRay();
RayHitInfo* newRayHitInfo();
bool intersect(Ray* ray, Bounds* bounds, RayHitInfo* hitInfo);
bool intersect(Ray* ray, Face* f, RayHitInfo* hitInfo);
void swap(float *a, float *b);

void rayCast(int pixelX, int pixelY, int screenWidth, int screenHeight, int nearPlane, int farPlane, Ray* rayCasted);

#endif RAY_H