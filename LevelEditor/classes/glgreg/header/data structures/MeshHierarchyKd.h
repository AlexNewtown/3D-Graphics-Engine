#ifndef MESH_HIERARCHY_KD
#define MESH_HIERARCHY_KD

#include "data structures\MeshStructures.h"
#include "entities\ObjLoader.h"
#include "Ray.h"

/* what to do:
	take every triangle and add it to the list of triangles in the node.
	subdivide as usual.
	if a triangle intersects with the node, add it to the subdivided list.
	continue recursively until there is a node for each triangle-barycenter.
	each of these nodes should contain all triangles that intersect with it.

	when doing ray traversal check intersections with each of the triangles intersection the leaf. 
*/

class MeshHierarchyKd;
struct AdjMeshTree;
struct KdTreeTextureFloat;
struct KdTreeTexture;
struct KdtreeStdArray;

struct KdTreeTextureFloat
{
	KdTreeTextureFloat();

	GLfloat maxX;
	GLfloat maxY;
	GLfloat maxZ;
	GLfloat maxW;

	GLfloat minX;
	GLfloat minY;
	GLfloat minZ;
	GLfloat minW;
};

struct KdTreeTextureInt
{
	KdTreeTextureInt();

	GLint treeIndex;

	GLint children0;
	GLint children1;

	GLint sortOn;

	GLint numFaces;
	GLint faceIndex;
	
	GLint totalNumFaces;
	GLint paddint2;
};

struct KdTreeTexture
{
	KdTreeTexture(int numNodes);
	~KdTreeTexture();
	int treeSize;
	KdTreeTextureFloat* texFloat;
	KdTreeTextureInt* texInt;
	
	//Array* faceIndexArray;
};

struct AdjMeshTree
{
	AdjMeshTree();
	MeshHierarchyKd** x;
	MeshHierarchyKd** y;
	MeshHierarchyKd** z;
};

class MeshHierarchyKd
{
public:

	//constructors/,deconstructors
	MeshHierarchyKd(std::vector<Face*> faces, int numFaces);
	MeshHierarchyKd(std::vector<Face*> faces, int start, int end, int numFacesInMesh, int sortOn, Bounds* bounds); 

	~MeshHierarchyKd();

	bool isLeaf();

	/* GETTERS */
	MeshHierarchyKd* children(int index);
	Point* node();
	Bounds* getBoundary();
	unsigned int sortOn();
	AdjMeshTree* getAdjTrees();
	unsigned int numIntersectingFaces();
	Face** intersectingFaces();
	void countNode(int* numNodes);
	int treeIndex;
	MeshHierarchyKd* parent;
	Face* face;

	int __start;
	int __end;

	MeshHierarchyKd* getSmallestParentTree(Point* a, Point* b);
	void rayTrace(Ray* ray, RayHitInfo* hitInfo, Face** hitFace);
	void rayTrace(Ray* ray, RayHitInfo* hitInfo, Face** hitFace, MeshHierarchyKd* parent, int levelsUp);

private:
	void mergeFaceX(std::vector<Face*> faces, int left, int mid, int right);
	void mergeFaceY(std::vector<Face*> faces, int left, int mid, int right);
	void mergeFaceZ(std::vector<Face*> faces, int left, int mid, int right);
	Bounds* computeMeshBounds(std::vector<Face*> faces, int start, int end);
protected:
	/* __node contains the position of the tree */
	Point* __node;

	/* bounds of the tree */
	Bounds* __boundary;

	/* children trees s*/
	MeshHierarchyKd* __children[2];

	AdjMeshTree* __adjTree;

	int __numIntersectingFaces;
	Face** __intersectingFaces;

	unsigned int __sortOn;
	void build(std::vector<Face*> faces, int start, int end, int numFacesInMesh, int sortOn);
	void upadteAdjTrees();

	void mergeSortFacesX(std::vector<Face*> faces, int left, int right);
	void mergeSortFacesY(std::vector<Face*> faces, int left, int right);
	void mergeSortFacesZ(std::vector<Face*> faces, int left, int right);

	void insertionSortFaces(std::vector<Face*> faces, int start, int end, int sortOn);

	unsigned int nextSortOn(unsigned int sortOn);

	void findIntersectingFaces(std::vector<Face*> faces, int start, int end);

	void setTreeAdjacency(AdjMeshTree* adjTree);
	AdjMeshTree*  copyAdjTree(AdjMeshTree* adjTree);
	void updateAdjTrees();
};

void getVertexBounds(Vertex** v, Bounds* bounds);
void getLargestBounds(Bounds* largestBounds, Bounds* compare);
float smallest(float a, float b, float c);
float largest(float a, float b, float c);

Face** generateCubeFaces();
Face** testFaceArray(int *numFaces);

void cloneKdTreeIntTex(MeshHierarchyKd* tree, KdTreeTextureInt* texInt);
void cloneKdTreeFloatTex(MeshHierarchyKd* tree, KdTreeTextureFloat* texFloat);
KdTreeTexture* createKdTreeTextureBuffers(MeshHierarchyKd* tree);

void createKdTreeArray(MeshHierarchyKd* tree, MeshHierarchyKd** treeArray, int* currentIndex);

#endif
