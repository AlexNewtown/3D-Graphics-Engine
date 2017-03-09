#pragma once
#include "entities/Entity.h"
#include "entities/Model.h"
#include "shader\SphereBoundaryShader.h"

#define BOUNDARY_TYPE_SPHERE 0
#define BOUNDARY_TYPE_AABB 1

#define SPHERE_BOUNDARY_MODEL_FILE_PATH "../bin/assets/obj/sphere/sphere.obj"

class BoundingHierarchy;
class BoundaryEntity;
class SphereBoundaryEntity;
struct BoundaryTreeTex;
struct BoundaryTreeTexInt;
struct BoundaryTreeTexFloat;

class BoundaryEntity : public Model_obj
{
public:
	BoundaryEntity(char* boundaryModel, Shader* s, int boundaryType);
	virtual ~BoundaryEntity();
	int boundaryType();
	float volume();
	virtual void calculateVolume();
protected:
	int __boundaryType;
	float __volume;
private:

};

class SphereBoundaryEntity : public BoundaryEntity
{
public:
	SphereBoundaryEntity();
	~SphereBoundaryEntity();
	void setRadius(float radius);
	float radius();
	virtual void calculateVolume();
	virtual void scaleTo(float x, float y, float z);
private:
	float __radius;
};

class BoundingHierarchy;

class BoundingHierarchy
{
public:
	BoundingHierarchy(std::vector<BoundaryEntity*> be, int sortOn);
	~BoundingHierarchy();
	BoundingHierarchy* child[2];
	int getNextSortOn();
	Bounds* parentBounds;
	BoundaryEntity* leafBoundary;
	float splittingPlane;
	void toBoundaryTreeTex(std::vector<BoundaryTreeTex*> &btt);
	int sortOn();
private:
	void sort(std::vector<BoundaryEntity*> &be);
	void swap(std::vector<BoundaryEntity*> &be, int i, int j);
	void subdivide(std::vector<BoundaryEntity*> &be, std::vector<BoundaryEntity*> &childA, std::vector<BoundaryEntity*> &childB);
	Bounds* calculateParentBounds(std::vector<BoundaryEntity*> be);
	int __sortOn;
};

struct BoundaryTreeTex
{
	BoundaryTreeTex();
	BoundaryTreeTex(BoundingHierarchy* bh);
	int boundaryType;
	int index;
	int childIndex0;
	int childIndex1;
	int sortOn;

	float radius;
	float position[3];

	float boundaryHigh[4];
	float boundaryLow[4];

	float splittingPlane;
};

struct BoundaryTreeTexInt
{
	int index;
	int childIndex0;
	int childIndex1;
	int boundaryType;

	int sortOn;
	int p[3];
};

struct BoundaryTreeTexFloat
{
	float radius;
	float position[3];
	float boundaryHigh[3];
	float splittingPlane;
	float boundaryLow[4];
};