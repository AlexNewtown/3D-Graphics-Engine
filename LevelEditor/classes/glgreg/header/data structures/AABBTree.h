#ifndef AABBTREE_H
#define AABBTREE_H

#include <stdio.h>
#include <vector>
#include "data structures\MeshStructures.h"
#include "math\Matrix.h"
#include "data structures\Point.h"

#define SORT_X 0
#define SORT_Y 1
#define SORT_Z 2

class AABBTree;

class AABBTree
{
public:
	AABBTree(std::vector<Face*> faces);
	AABBTree(std::vector<Face*> &faces, int start, int end, int sortOn);
	~AABBTree();
	Bounds* bounds();
private:
	Bounds* __bounds;
	int __sortOn;
	AABBTree* __children[2];

	void build(std::vector<Face*> &faces, int start, int end, int sortOn);
	int getNextSortOn();
	void calculateBounds(std::vector<Face*> faces, int start, int end);
	void faceBounds(Face* f, Bounds *b);
	void sort(std::vector<Face*> &faces, int start, int end, int sortOn);
	void merge(std::vector<Face*> &faces, int start, int mid, int end, int sortOn);
};

#endif