#pragma once

#include "data structures\MeshStructures.h"
#include "stdio.h"
#include <opencv\cv.h>
#include <vector>

#define SORT_ON_X 0
#define SORT_ON_Y 1
#define SORT_ON_Z 2

#define K_PHOTON_MAP_CLUSTERS 200

class PhotonMap;
class PhotonMapKdTree;
class PhotonMapKdTextureFloat;
class PhotonMapKdTextureInt;
class PhotonMapTexture;

class PhotonMap
{
public:
	PhotonMap();
	~PhotonMap();
	float pos[3];
	float power;
	float incidentDirection[3];
	float normal[3];
	unsigned int index;
};

class MaterialFace : public Face
{
public:
	MaterialFace();
	virtual ~MaterialFace();
	float diffuseP;
	float specularP;
	float indexOfRefraction;
};

class PhotonMapKdTree
{
public:
	PhotonMapKdTree(std::vector<PhotonMap*> &photonMap);
	PhotonMapKdTree(std::vector<PhotonMap*> &photonMap, Bounds* parentBounds, int start, int end, int sortOn);
	~PhotonMapKdTree();

	bool isLeaf();
	PhotonMapKdTree* children(int index);
	Point* node();
	Bounds* getBoundary();
	unsigned int sortOn();
	int photonIndex();
	int start();
	int end();

	int treeIndex;
	int numTreePhotons;

private:
	int __start;
	int __end;
	int __photonIndex;
	void insertionSortPhotons(std::vector<PhotonMap*> &photonMap, int start, int end, int sortOn);
	Bounds* computeBounds(std::vector<PhotonMap*> &photonMap, int start, int end);
	void build(std::vector<PhotonMap*> &photonMap, int start, int end, int sortOn);
	int nextSortOn(int sortOn);
protected:
	Point* __node;
	Bounds* __bounds;
	PhotonMapKdTree* __children[2];
	unsigned int __sortOn;
};

class PhotonMapKdTextureFloat
{
public:
	PhotonMapKdTextureFloat();
	float minX;
	float minY;
	float minZ;
	float maxX;
	float maxY;
	float maxZ;
	float padding[2];
};
class PhotonMapKdTextureInt
{
public:
	PhotonMapKdTextureInt();
	int treeIndex;
	int childIndex0;
	int childIndex1;
	int sortOn;
	int photonIndex;
	int numTreePhotons;
	int startIndex;
	int endIndex;
};

class PhotonMapClusterFloat
{
public:
	float x;
	float y;
	float z;
	float radius;
};

class PhotonMapClusterInt
{
public:
	int start;
	int end;
	int padding[2];
};

class PhotonMapTexture
{
public:
	PhotonMapTexture();
	int index;
	float x;
	float y;
	float z;
	float dirX;
	float dirY;
	float dirZ;
	float illum;
};

void buildPhotonMapKdTreeArray(PhotonMapKdTree* tree, std::vector<PhotonMapKdTextureInt*> &texInt, std::vector<PhotonMapKdTextureFloat*> &texFloat);
void* createPhotonMapTexture(std::vector < PhotonMap*> photonMap);
void createPhotonMapKdTexture(PhotonMapKdTree* tree, void* &photonMapKdTextureInt, int* photonMapKdTextureIntSize, void* &photonMapKdTextureFloat, int* photonMapKdTextureFloatSize);

void createPhotonMapClusterTexture(std::vector<PhotonMap*> photonMap, void* &photonMapClusterFloat, void* &photonMapClusterInt, void* &photonMapTexture);