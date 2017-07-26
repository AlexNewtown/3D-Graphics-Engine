#include "data structures\Ray.h"
#include "data structures\Point.h"
#include <stdio.h>
#include <limits>

using namespace std;

Ray::Ray()
{
	
}
Ray::~Ray()
{
	
}

void Ray::set(float* pos, float* dir)
{
	this->pos.x = pos[0];
	this->pos.y = pos[1];
	this->pos.z = pos[2];

	this->dir.x = dir[0];
	this->dir.y = dir[1];
	this->dir.z = dir[2];
}

RayHitInfo::RayHitInfo()
{
	tmin = 0x7FFFFFFF;
	tmax = 0;

	txmin = 0;
	txmax = 0;
	tymin = 0;
	tymax = 0;
	tzmin = 0;
	tzmax = 0;
}

void RayHitInfo::reset()
{
	tmin = 0x7FFFFFFF;
	tmax = 0;

	txmin = 0;
	txmax = 0;
	tymin = 0;
	tymax = 0;
	tzmin = 0;
	tzmax = 0;
}

RayHitInfo::~RayHitInfo()
{

}

void RayHitInfo::computeMinMax(Ray* r)
{
	ray.dir.x = r->dir.x;
	ray.dir.y = r->dir.y;
	ray.dir.z = r->dir.z;

	ray.pos.x = r->pos.x;
	ray.pos.y = r->pos.y;
	ray.pos.z = r->pos.z;
	
	min.x = ray.pos.x + tmin*ray.dir.x;
	min.y = ray.pos.y + tmin*ray.dir.y;
	min.z = ray.pos.z + tmin*ray.dir.z;

	max.x = ray.pos.x + tmax*ray.dir.x;
	max.y = ray.pos.y + tmax*ray.dir.y;
	max.z = ray.pos.z + tmax*ray.dir.z;
}

unsigned int RayHitInfo::getSmallestT()
{
	if(txmax <= tymax)
	{
		if(txmax <= tzmax)
		{
			// x is the smallest
			return SORT_ON_X;
		}
		else
		{
			//z is the smallest
			return SORT_ON_Z;
		}
	}
	else
	{
		if(tymax <= tzmax)
		{
			// y is the smallest
			return SORT_ON_Y;
		}
		else
		{
			//z is the smallest
			return SORT_ON_Z;
		}
	}
}

void RayHitInfo::copy(RayHitInfo* rh)
{
	ray.dir.x = rh->ray.dir.x;
	ray.dir.y = rh->ray.dir.y;
	ray.dir.z = rh->ray.dir.z;

	ray.pos.x = rh->ray.pos.x;
	ray.pos.y = rh->ray.pos.y;
	ray.pos.z = rh->ray.pos.z;

	max.x = rh->max.x;
	max.y = rh->max.y;
	max.z = rh->max.z;

	min.x = rh->min.x;
	min.y = rh->min.y;
	min.z = rh->min.z;

	tmin = rh->tmin;
	tmax = rh->tmax;

	txmin = rh->txmin;
	txmax = rh->txmax;
	tymin = rh->tymin;
	tymax = rh->tymax;
	tzmin = rh->tzmin;
	tzmax = rh->tzmax;
}

#define PARALLEL_THRESHOLD 0.00000000001

bool intersect(Ray* ray, Bounds* bounds, RayHitInfo* hitInfo)
{
	float tmin = 0.0f;
	float tmax = std::numeric_limits<float>::max();
	float txmin = 0, tymin = 0, tzmin = 0;
	float txmax = std::numeric_limits<float>::max();
	float tymax = std::numeric_limits<float>::max();
	float tzmax = std::numeric_limits<float>::max();

	if (abs(ray->dir.x) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (ray->pos.x < bounds->low->x || ray->pos.x > bounds->high->x)
			return false;
	}
	if (abs(ray->dir.y) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (ray->pos.y < bounds->low->y || ray->pos.y > bounds->high->y)
			return false;
	}
	if (abs(ray->dir.z) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (ray->pos.z < bounds->low->z || ray->pos.z > bounds->high->z)
			return false;
	}

	txmin = (bounds->low->x - ray->pos.x) / ray->dir.x;
	txmax = (bounds->high->x - ray->pos.x) / ray->dir.x;
	if (txmin > txmax)
		swap(&txmin, &txmax);

	tymin = (bounds->low->y - ray->pos.y) / ray->dir.y;
	tymax = (bounds->high->y - ray->pos.y) / ray->dir.y;
	if (tymin > tymax)
		swap(&tymin, &tymax);


	tzmin = (bounds->low->z - ray->pos.z) / ray->dir.z;
	tzmax = (bounds->high->z - ray->pos.z) / ray->dir.z;
	if (tzmin > tzmax)
		swap(&tzmin, &tzmax);

	tmax = minimum(txmax, tymax);
	tmax = minimum(tzmax, tmax);
	tmin = maximum(txmin, tymin);
	tmin = maximum(tzmin, tmin);

	if (tmin > tmax)
		return false;

	hitInfo->tmax = tmax;
	hitInfo->tmin = tmin;
	hitInfo->txmax = txmax;
	hitInfo->txmin = txmin;
	hitInfo->tymax = tymax;
	hitInfo->tymin = tymin;
	hitInfo->tzmax = tzmax;
	hitInfo->tzmin = tzmin;
	hitInfo->computeMinMax(ray);
	return true;
}

bool intersect(Ray* ray, Face* f, RayHitInfo* hitInfo)
{
	float E1[] = { f->vertex[1]->pos[0] - f->vertex[0]->pos[0],
					f->vertex[1]->pos[1] - f->vertex[0]->pos[1],
					f->vertex[1]->pos[2] - f->vertex[0]->pos[2] };

	float E2[] = { f->vertex[2]->pos[0] - f->vertex[0]->pos[0],
					f->vertex[2]->pos[1] - f->vertex[0]->pos[1],
					f->vertex[2]->pos[2] - f->vertex[0]->pos[2] };

	float T[] = { ray->pos.x - f->vertex[0]->pos[0],
		ray->pos.y - f->vertex[0]->pos[1],
		ray->pos.z - f->vertex[0]->pos[2] };

	float Q[3];
	crossProduct(T, E1, Q);

	float P[3];
	float rayDir[] = { ray->dir.x, ray->dir.y, ray->dir.z };
	crossProduct(rayDir, E2,P);

	float det = P[0] * E1[0] + P[1] * E1[1] + P[2] * E1[2];

	float detInv = 1 / det;

	float u = (P[0] * T[0] + P[1] * T[1] + P[2] * T[2])*detInv;

	if (u < 0 || u > 1)
		return false;

	float v = (Q[0] * rayDir[0] + Q[1] * rayDir[1] + Q[2] * rayDir[2])*detInv;
	
	if (v < 0 || u > 1)
		return false;

	if (u + v > 1)
		return false;
	float t = (Q[0] * E2[0] + Q[1] * E2[1] + Q[2] * E2[2])*detInv;

	hitInfo->ray.dir.x = ray->dir.x;
	hitInfo->ray.dir.y = ray->dir.y;
	hitInfo->ray.dir.z = ray->dir.z;

	hitInfo->ray.pos.x = ray->pos.x;
	hitInfo->ray.pos.y = ray->pos.y;
	hitInfo->ray.pos.z = ray->pos.z;

	float tmin = dotProduct(E2, Q)*detInv;
	float hit[] = { tmin*ray->dir.x + ray->pos.x, tmin*ray->dir.y + ray->pos.y, tmin*ray->dir.z + ray->pos.z };

	float txmin = hit[0] - ray->pos.x;
	float tymin = hit[1] - ray->pos.y;
	float tzmin = hit[2] - ray->pos.z;

	float tVec[] = { txmin,tymin,tzmin };
	hitInfo->txmin = txmin;
	hitInfo->tymin = tymin;
	hitInfo->tzmin = tzmin;

	hitInfo->min.x = hit[0];
	hitInfo->min.y = hit[1];
	hitInfo->min.z = hit[2];
	hitInfo->tmin = tmin;

	return true;
}

void swap(float *a, float *b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}

void rayCast(int pixelX, int pixelY, int screenWidth, int screenHeight, int nearPlane, int farPlane, Ray* rayCasted)
{

	float aspectRatio = (float)screenWidth/(float)screenHeight;
	float x = (pixelX/(0.5*(float)screenWidth) - 1)*aspectRatio;
	float y = -(pixelY/(0.5*(float)screenHeight)  - 1);

	float z = -(float)nearPlane;

	if(rayCasted == NULL)
	{
		rayCasted = new Ray();
	}

	rayCasted->pos.x = 0.0;
	rayCasted->pos.y = 0.0;
	rayCasted->pos.z = 0.0;
	
	rayCasted->dir.x = x;
	rayCasted->dir.y = y;
	rayCasted->dir.z = z;

	rayCasted->dir.normalize();

}