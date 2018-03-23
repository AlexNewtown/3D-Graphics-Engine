#include "data structures\MeshHierarchyKd.h"
#include "data structures\Point.h"
#include <cmath>

Face** tempArray;

MeshHierarchyKd::MeshHierarchyKd(std::vector<Face*> faces, int numFaces)
{
	__boundary = computeMeshBounds(faces,0,numFaces);
	__node = NULL;
	__children[0] = NULL;
	__children[1] = NULL;
	parent = NULL;

	build(faces, 0, numFaces, numFaces, 0);
	AdjMeshTree* adjTree = new AdjMeshTree();
	setTreeAdjacency(adjTree);
	
}
MeshHierarchyKd::MeshHierarchyKd(std::vector<Face*> faces, int start, int end, int numFacesInMesh, int sortOn, Bounds* bounds)
{
	__node = NULL;
	__children[0] = nullptr;
	__children[1] = nullptr;
	__boundary = computeMeshBounds(faces, start, end);
	build(faces, start, end, numFacesInMesh, sortOn);
}

MeshHierarchyKd::~MeshHierarchyKd()
{
	if(__node != NULL)
		delete __node;
	if(__boundary != NULL)
		delete __boundary;
	if (children(0) != NULL)
		delete children(0);
	if (children(1) != NULL)
		delete children(1);
}

void MeshHierarchyKd::build(std::vector<Face*> faces, int start, int end, int numFacesInMesh, int sortOn)
{
	__sortOn = sortOn;
	__adjTree = new AdjMeshTree();
	face = NULL;

	__start = start;
	__end = end;

	if(end - start == 1)
	{
		__node = new Point(faces[start]->avg[0],faces[start]->avg[1],faces[start]->avg[2]);
		face = faces[start];
		return;
	}

	insertionSortFaces(faces, start, end, sortOn);

	int numPoints = (end - start);
	int mid = (int)floor((float)numPoints*0.5) + start;

	__node = new Point(faces[mid]->avg[0], faces[mid]->avg[1], faces[mid]->avg[2]);

	int numPointsHigh = (end - mid);
	int numPointsLow = (mid  - start);

	if (numPointsHigh > 0)
	{
		__children[1] = new MeshHierarchyKd(faces, mid, end, numFacesInMesh, nextSortOn(sortOn), NULL);
		__children[1]->parent = this;
	}

	if (numPointsLow > 0)
	{
		__children[0] = new MeshHierarchyKd(faces, start, mid, numFacesInMesh, nextSortOn(sortOn), NULL);
		__children[1]->parent = this;
	}
	
}

unsigned int MeshHierarchyKd::nextSortOn(unsigned int sortOn)
{
	switch(sortOn)
	{
	case SORT_ON_X:
		return SORT_ON_Y;
		break;
	case SORT_ON_Y:
		return SORT_ON_Z;
		break;
	case SORT_ON_Z:
		return SORT_ON_X;
		break;
	}
}

bool MeshHierarchyKd::isLeaf()
{
	if(__children[0] == NULL || __children[1] == NULL)
		return true;
	return false;
}

void MeshHierarchyKd::insertionSortFaces(std::vector<Face*> faces, int start, int end, int sortOn)
{
	for (int i = start; i < end; i++)
	{
		for (int j = i + 1; j < end; j++)
		{
			if (faces[j]->avg[__sortOn] < faces[i]->avg[__sortOn])
			{
				Face* temp = faces[i];
				faces[i] = faces[j];
				faces[j] = temp;
			}
		}
	}
}

void MeshHierarchyKd::mergeSortFacesX(std::vector<Face*> faces, int left, int right)
{
	int mid;
	if( right > left)
	{
		mid = (int)floor((right + left)*0.5);
		mergeSortFacesX(faces, left, mid);
		mergeSortFacesX(faces, mid+1,right);

		mergeFaceX(faces, left, mid+1, right);
	}
}
void MeshHierarchyKd::mergeSortFacesY(std::vector<Face*> faces, int left, int right)
{
	int mid;
	if( right > left)
	{
		mid = (int)floor((right + left)*0.5);
		mergeSortFacesY(faces, left, mid);
		mergeSortFacesY(faces, mid+1,right);

		mergeFaceY(faces, left, mid+1, right);
	}
}
void MeshHierarchyKd::mergeSortFacesZ(std::vector<Face*> faces, int left, int right)
{
	int mid;
	if( right > left)
	{
		mid = (int)floor((right + left)*0.5);
		mergeSortFacesZ(faces, left, mid);
		mergeSortFacesZ(faces, mid+1,right);

		mergeFaceZ(faces, left, mid+1, right);
	}
}



void MeshHierarchyKd::mergeFaceX(std::vector<Face*>faces, int left, int mid, int right)
{
	int i, leftEnd, numElements, tempPos;

	leftEnd = mid;
	tempPos = 0;
	numElements = right- left;

	while(left < leftEnd && mid <= right)
	{
		if( faces[left]->avg[0]<= faces[mid]->avg[0])
		{
			tempArray[tempPos] = faces[left];
			tempPos++;
			left++;
		}
		else
		{
			tempArray[tempPos] = faces[mid];
			tempPos++;
			mid++;
		}
	}

	while(left < leftEnd)
	{
		tempArray[tempPos] = faces[left];
		left++;
		tempPos++;
	}
	while(mid <= right)
	{
		tempArray[tempPos] = faces[mid];
		mid++;
		tempPos++;
	}

	for(int i=tempPos-1; i>=0; i--)
	{
		faces[right] = tempArray[i];
		right--;
	}
}
void MeshHierarchyKd::mergeFaceY(std::vector<Face*> faces, int left, int mid, int right)
{
	int i, leftEnd, numElements, tempPos;

	leftEnd = mid;
	tempPos = 0;
	numElements = right- left;

	while(left < leftEnd && mid <= right)
	{
		if( faces[left]->avg[1] <= faces[mid]->avg[1])
		{
			tempArray[tempPos] = faces[left];
			tempPos++;
			left++;
		}
		else
		{
			tempArray[tempPos] = faces[mid];
			tempPos++;
			mid++;
		}
	}

	while(left < leftEnd)
	{
		tempArray[tempPos] = faces[left];
		left++;
		tempPos++;
	}
	while(mid <= right)
	{
		tempArray[tempPos] = faces[mid];
		mid++;
		tempPos++;
	}

	for(int i=tempPos-1; i>=0; i--)
	{
		faces[right] = tempArray[i];
		right--;
	}
}
void MeshHierarchyKd::mergeFaceZ(std::vector<Face*> faces, int left, int mid, int right)
{
	int i, leftEnd, numElements, tempPos;

	leftEnd = mid;
	tempPos = 0;
	numElements = right- left;

	while(left < leftEnd && mid <= right)
	{
		if( faces[left]->avg[2] <= faces[mid]->avg[2])
		{
			tempArray[tempPos] = faces[left];
			tempPos++;
			left++;
		}
		else
		{
			tempArray[tempPos] = faces[mid];
			tempPos++;
			mid++;
		}
	}

	while(left < leftEnd)
	{
		tempArray[tempPos] = faces[left];
		left++;
		tempPos++;
	}
	while(mid <= right)
	{
		tempArray[tempPos] = faces[mid];
		mid++;
		tempPos++;
	}

	for(int i=tempPos-1; i>=0; i--)
	{
		faces[right] = tempArray[i];
		right--;
	}
}

void MeshHierarchyKd::findIntersectingFaces(std::vector<Face*> faces, int start, int end)
{
	int numIntersectingFaces = 0;
	int tempArrayIndex = 0;
	Point p;
	Vertex* v[3];
	Bounds bounds;
	Point boundsMin;
	Point boundsMax;
	bounds.low = &boundsMin;
	bounds.high = &boundsMax;
	for(int i=start; i<=end; i++)
	{
		v[0] = faces[i]->vertex[0];
		v[1] = faces[i]->vertex[1];
		v[2] = faces[i]->vertex[2];
		getVertexBounds(v, &bounds);

		/*for(int j=0; j<3; j++)
		{
			p.x = ((Vertex*)(faces[i]->adjVertices->buf[j]))->pos.x;
			p.y = ((Vertex*)(faces[i]->adjVertices->buf[j]))->pos.y;
			p.z = ((Vertex*)(faces[i]->adjVertices->buf[j]))->pos.z;
			if(pointInBounds(__boundary, &p))
			{
				tempArray[tempArrayIndex] = faces[i];
				tempArrayIndex++;
			}
			break;
		}*/
		if(boundsOverlap(__boundary, &bounds))
		{
			tempArray[tempArrayIndex] = faces[i];
			tempArrayIndex++;
		}
	}

	if(tempArrayIndex > 0)
	{
		__intersectingFaces = new Face*[tempArrayIndex];
		__numIntersectingFaces = tempArrayIndex;
		for(int i=0; i<=tempArrayIndex; i++)
		{
			__intersectingFaces[i] = tempArray[i];
		}
	}
	else
	{
		__numIntersectingFaces = 0;
		__intersectingFaces = NULL;
	}

}

MeshHierarchyKd* MeshHierarchyKd::children(int index)
{
	if(index < 0 || index > 1)
		return NULL;
	else
		return __children[index];
}

Point* MeshHierarchyKd::node()
{
	return __node;
}
Bounds* MeshHierarchyKd::getBoundary()
{
	return __boundary;
}
unsigned int MeshHierarchyKd::sortOn()
{
	return __sortOn;
}
AdjMeshTree* MeshHierarchyKd::getAdjTrees()
{
	return __adjTree;
}
unsigned int MeshHierarchyKd::numIntersectingFaces()
{
	return __numIntersectingFaces;
}
Face** MeshHierarchyKd::intersectingFaces()
{
	return __intersectingFaces;
}

void getVertexBounds(Vertex** v, Bounds* bounds)
{
	bounds->low->x = smallest(v[0]->pos[0], v[1]->pos[0], v[2]->pos[0]);
	bounds->high->x = largest(v[0]->pos[0], v[1]->pos[0], v[2]->pos[0]);

	bounds->low->y = smallest(v[0]->pos[1], v[1]->pos[1], v[2]->pos[1]);
	bounds->high->y = largest(v[0]->pos[1], v[1]->pos[1], v[2]->pos[1]);

	bounds->low->z = smallest(v[0]->pos[2], v[1]->pos[2], v[2]->pos[2]);
	bounds->high->z = largest(v[0]->pos[2], v[1]->pos[2], v[2]->pos[2]);

	bounds->reCalculate();
}

float smallest(float a, float b, float c)
{
	if( a < b)
	{
		if( a < c)
			return a;
		else
			return c;
	}
	else
	{
		if( b < c)
			return b;
		else
			return c;
	}

}

float largest(float a, float b, float c)
{
	if( a > b)
	{
		if( a > c)
			return a;
		else
			return c;
	}
	else
	{
		if( b > c)
			return b;
		else
			return c;
	}
}

void getLargestBounds(Bounds* largestBounds, Bounds* compare)
{
	if(compare->low->x < largestBounds->low->x)
		largestBounds->low->x = compare->low->x;
	if(compare->low->y < largestBounds->low->y)
		largestBounds->low->y = compare->low->y;
	if(compare->low->z < largestBounds->low->z)
		largestBounds->low->z = compare->low->z;
	if(compare->high->x > largestBounds->high->x)
		largestBounds->high->x = compare->high->x;
	if(compare->high->y > largestBounds->high->y)
		largestBounds->high->y = compare->high->y;
	if(compare->high->z > largestBounds->high->z)
		largestBounds->high->z = compare->high->z;
}


void MeshHierarchyKd::countNode(int* numNodes)
{
	this->treeIndex = *numNodes;
	*numNodes = *numNodes + 1;


	if(__children[0] != NULL)
	{
		__children[0]->countNode(numNodes);
	}

	if(__children[1] != NULL)
	{
		__children[1]->countNode(numNodes);
	}
}

void MeshHierarchyKd::setTreeAdjacency(AdjMeshTree* adjTree)
{
	__adjTree = adjTree;
	if(!isLeaf())
	{
		AdjMeshTree* childLowAdjTrees = copyAdjTree(adjTree);
		AdjMeshTree* childHighAdjTrees = copyAdjTree(adjTree);

		switch(__sortOn)
		{
		case 0:
			childLowAdjTrees->x[1] = children(1);
			childHighAdjTrees->x[0] = children(0);
			break;

		case 1:
			childLowAdjTrees->y[1] = children(1);
			childHighAdjTrees->y[0] = children(0);
			break;

		case 2:
			childLowAdjTrees->z[1] = children(1);
			childHighAdjTrees->z[0] = children(0);
			break;
		}
		children(1)->setTreeAdjacency(childHighAdjTrees);
		children(0)->setTreeAdjacency(childLowAdjTrees);
	}
	else
	{
		updateAdjTrees();
	}
}

AdjMeshTree* MeshHierarchyKd::copyAdjTree(AdjMeshTree* adjTree)
{
	AdjMeshTree* adjTreeCopy = new AdjMeshTree();
	adjTreeCopy->x[0] = adjTree->x[0];
	adjTreeCopy->x[1] = adjTree->x[1];

	adjTreeCopy->y[0] = adjTree->y[0];
	adjTreeCopy->y[1] = adjTree->y[1];

	adjTreeCopy->z[0] = adjTree->z[0];
	adjTreeCopy->z[1] = adjTree->z[1];
	return adjTreeCopy;
}

void MeshHierarchyKd::updateAdjTrees()
{
	// go through each adjTree,
	// if it is a leaf do nothing
	// if it is not a leaf scan though the tree until
	Point pointBoundsA;
	Point pointBoundsB;
	if(__adjTree->x[0] != NULL)
	{
		if(!__adjTree->x[0]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->low->x;
			pointBoundsA.y = getBoundary()->low->y;
			pointBoundsA.z = getBoundary()->low->z;
			
			pointBoundsB.x = getBoundary()->low->x;
			pointBoundsB.y = getBoundary()->high->y;
			pointBoundsB.z = getBoundary()->high->z;

			__adjTree->x[0] = __adjTree->x[0]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}

	if(__adjTree->x[1] != NULL)
	{
		if(!__adjTree->x[1]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->high->x;
			pointBoundsA.y = getBoundary()->high->y;
			pointBoundsA.z = getBoundary()->high->z;
			
			pointBoundsB.x = getBoundary()->high->x;
			pointBoundsB.y = getBoundary()->low->y;
			pointBoundsB.z = getBoundary()->low->z;

			__adjTree->x[1] = __adjTree->x[1]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}

	if(__adjTree->y[0] != NULL)
	{
		if(!__adjTree->y[0]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->low->x;
			pointBoundsA.y = getBoundary()->low->y;
			pointBoundsA.z = getBoundary()->low->z;
			
			pointBoundsB.x = getBoundary()->high->x;
			pointBoundsB.y = getBoundary()->low->y;
			pointBoundsB.z = getBoundary()->high->z;

			__adjTree->y[0] = __adjTree->y[0]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}

	if(__adjTree->y[1] != NULL)
	{
		if(!__adjTree->y[1]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->low->x;
			pointBoundsA.y = getBoundary()->high->y;
			pointBoundsA.z = getBoundary()->low->z;
			
			pointBoundsB.x = getBoundary()->high->x;
			pointBoundsB.y = getBoundary()->high->y;
			pointBoundsB.z = getBoundary()->high->z;

			__adjTree->y[1] = __adjTree->y[1]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}

	if(__adjTree->z[0] != NULL)
	{
		if(!__adjTree->z[0]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->low->x;
			pointBoundsA.y = getBoundary()->low->y;
			pointBoundsA.z = getBoundary()->low->z;
			
			pointBoundsB.x = getBoundary()->high->x;
			pointBoundsB.y = getBoundary()->high->y;
			pointBoundsB.z = getBoundary()->low->z;

			__adjTree->z[0] = __adjTree->z[0]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}

	if(__adjTree->z[1] != NULL)
	{
		if(!__adjTree->z[1]->isLeaf())
		{
			pointBoundsA.x = getBoundary()->low->x;
			pointBoundsA.y = getBoundary()->low->y;
			pointBoundsA.z = getBoundary()->high->z;
			
			pointBoundsB.x = getBoundary()->high->x;
			pointBoundsB.y = getBoundary()->high->y;
			pointBoundsB.z = getBoundary()->high->z;

			__adjTree->z[1] = __adjTree->z[1]->getSmallestParentTree(&pointBoundsA, &pointBoundsB);
		}
	}
}

MeshHierarchyKd* MeshHierarchyKd::getSmallestParentTree(Point* a, Point* b)
{
	switch(__sortOn)
	{
	case 0:
		if(!isLeaf())
		{
			if(a->x >= node()->x && b->x >= node()->x)
			{
				__children[1]->getSmallestParentTree(a,b);
			}
			else if(a->x < node()->x && b->x < node()->x)
			{
				__children[0]->getSmallestParentTree(a,b);
			}
			else
			{
				return this;
			}
		}
		else
		{
			return this;
		}
		break;

	case 1:
		if(!isLeaf())
		{
			if(a->y >= node()->y && b->y >= node()->y)
			{
				__children[1]->getSmallestParentTree(a,b);
			}
			else if(a->y < node()->y && b->y < node()->y)
			{
				__children[0]->getSmallestParentTree(a,b);
			}
			else
			{
				return this;
			}
		}
		else
		{
			return this;
		}
		break;

	case 2:
		if(!isLeaf())
		{
			if(a->z >= node()->z && b->z >= node()->z)
			{
				__children[1]->getSmallestParentTree(a,b);
			}
			else if(a->z < node()->z && b->z < node()->z)
			{
				__children[0]->getSmallestParentTree(a,b);
			}
			else
			{
				return this;
			}
		}
		else
		{
			return this;
		}
		break;
	}
}

Bounds* MeshHierarchyKd::computeMeshBounds(std::vector<Face*> faces, int start, int end)
{
	Face* currentFace = faces[start];
	
	float minX = minimum(minimum(currentFace->vertex[0]->pos[0], currentFace->vertex[1]->pos[0]), currentFace->vertex[2]->pos[0]) - 1.0;
	float minY = minimum(minimum(currentFace->vertex[0]->pos[1], currentFace->vertex[1]->pos[1]), currentFace->vertex[2]->pos[1]) - 1.0;
	float minZ = minimum(minimum(currentFace->vertex[0]->pos[2], currentFace->vertex[1]->pos[2]), currentFace->vertex[2]->pos[2]) - 1.0;

	float maxX = maximum(maximum(currentFace->vertex[0]->pos[0], currentFace->vertex[1]->pos[0]), currentFace->vertex[2]->pos[0]) + 1.0;
	float maxY = maximum(maximum(currentFace->vertex[0]->pos[1], currentFace->vertex[1]->pos[1]), currentFace->vertex[2]->pos[1]) + 1.0;
	float maxZ = maximum(maximum(currentFace->vertex[0]->pos[2], currentFace->vertex[1]->pos[2]), currentFace->vertex[2]->pos[2]) + 1.0;

	for (int i = start+1; i < end; i++)
	{
		Face* currentFace = faces[i];

		minX = minimum(minimum(minimum(currentFace->vertex[0]->pos[0], currentFace->vertex[1]->pos[0]), currentFace->vertex[2]->pos[0]), minX);
		minY = minimum(minimum(minimum(currentFace->vertex[0]->pos[1], currentFace->vertex[1]->pos[1]), currentFace->vertex[2]->pos[1]), minY);
		minZ = minimum(minimum(minimum(currentFace->vertex[0]->pos[2], currentFace->vertex[1]->pos[2]), currentFace->vertex[2]->pos[2]), minZ);

		maxX = maximum(maximum(maximum(currentFace->vertex[0]->pos[0], currentFace->vertex[1]->pos[0]), currentFace->vertex[2]->pos[0]), maxX);
		maxY = maximum(maximum(maximum(currentFace->vertex[0]->pos[1], currentFace->vertex[1]->pos[1]), currentFace->vertex[2]->pos[1]), maxY);
		maxZ = maximum(maximum(maximum(currentFace->vertex[0]->pos[2], currentFace->vertex[1]->pos[2]), currentFace->vertex[2]->pos[2]), maxZ);

	}

	return new Bounds(new Point(minX, minY, minZ), new Point(maxX, maxY, maxZ));
}

void MeshHierarchyKd::rayTrace(Ray* ray, RayHitInfo* hitInfo, Face** hitFace)
{
	RayHitInfo hi;
	if (isLeaf())
	{
		if (intersect(ray, face, &hi))
		{
			if (hi.tmin > 0 && hi.tmin < hitInfo->tmin)
			{
				hitInfo->copy(&hi);
				*hitFace = face;
			}
		}
	}
	else
	{
		if (this->children(0) != NULL)
		{
			if (intersect(ray, this->children(0)->getBoundary() , &hi))
			{
				this->children(0)->rayTrace(ray, hitInfo, hitFace);
			}
		}
		if (this->children(1) != NULL)
		{
			if (intersect(ray, this->children(1)->getBoundary() , &hi))
			{
				this->children(1)->rayTrace(ray, hitInfo, hitFace);
			}
		}
	}
}

void MeshHierarchyKd::rayTrace(Ray* ray, RayHitInfo* hitInfo, Face** hitFace, MeshHierarchyKd* parent, int levelsUp)
{
	RayHitInfo hi;
	if (isLeaf())
	{
		if (intersect(ray, face, &hi))
		{
			if (hi.tmin > 0 && hi.tmin < hitInfo->tmin)
			{
				hitInfo->copy(&hi);
				*hitFace = face;
				parent = this;
				for (int i = 0; i < levelsUp; i++)
				{
					parent = parent->parent;
				}
			}
		}
	}
	else
	{
		if (this->children(0) != NULL)
		{
			if (intersect(ray, this->children(0)->getBoundary(), &hi))
			{
				this->children(0)->rayTrace(ray, hitInfo, hitFace);
			}
		}
		if (this->children(1) != NULL)
		{
			if (intersect(ray, this->children(1)->getBoundary(), &hi))
			{
				this->children(1)->rayTrace(ray, hitInfo, hitFace);
			}
		}
	}
}

AdjMeshTree::AdjMeshTree()
{
	x = new MeshHierarchyKd*[2];
	y = new MeshHierarchyKd*[2];
	z = new MeshHierarchyKd*[2];
	x[0] = NULL;
	x[1] = NULL;
	y[0] = NULL;
	y[1] = NULL;
	z[0] = NULL;
	z[1] = NULL;
}

KdTreeTextureFloat::KdTreeTextureFloat()
{
	maxX = 0.0;
	maxY = 0.0;
	maxZ = 0.0;
	maxW = 0.0;

	minX = 0.0;
	minY = 0.0;
	minZ = 0.0;
	minW = 0.0;
}

KdTreeTextureInt::KdTreeTextureInt()
{	
	treeIndex = -1;

	children0 = -1;
	children1 = -1;

	sortOn = -1;

	numFaces = 0;
	faceIndex = -1;
}

KdTreeTexture::~KdTreeTexture()
{
	if(texFloat != nullptr)
		delete texFloat;
	if(texInt != nullptr)
		delete texInt;
}

KdTreeTexture::KdTreeTexture(int numNodes)
{
	treeSize = numNodes;
	texFloat = new KdTreeTextureFloat[treeSize];
	texInt = new KdTreeTextureInt[treeSize];
}

void cloneKdTreeIntTex(MeshHierarchyKd* tree, KdTreeTextureInt &texInt)
{
	texInt.treeIndex = tree->treeIndex;
	
	/* SET CHILDREN */
	if(tree->children(0) == NULL)
	{
		texInt.children0 = -1;
	}
	else
	{
		texInt.children0 = tree->children(0)->treeIndex;
	}
	if(tree->children(1) == NULL)
	{
		texInt.children1 = -1;
	}
	else
	{
		texInt.children1 = tree->children(1)->treeIndex;
	}



	if(tree->isLeaf())
	{
		texInt.numFaces = tree->numIntersectingFaces();
		texInt.faceIndex =  tree->face->index;
	}
	else
	{
		texInt.numFaces = 0;
	}
	texInt.sortOn = tree->sortOn();
}
void cloneKdTreeFloatTex(MeshHierarchyKd* tree, KdTreeTextureFloat &texFloat)
{
	texFloat.maxX = tree->getBoundary()->high->x;
	texFloat.maxY = tree->getBoundary()->high->y;
	texFloat.maxZ = tree->getBoundary()->high->z;
	texFloat.maxW = 1.0;

	texFloat.minX = tree->getBoundary()->low->x;
	texFloat.minY = tree->getBoundary()->low->y;
	texFloat.minZ = tree->getBoundary()->low->z;
	texFloat.minW = 1.0;
}

void createKdTreeArray(MeshHierarchyKd* tree, MeshHierarchyKd** treeArray, int* currentIndex)
{
	treeArray[*currentIndex] = tree;
	*currentIndex = *currentIndex + 1;
	if( tree->children(0) != NULL)
	{
		createKdTreeArray(tree->children(0), treeArray, currentIndex);
	}
	if( tree->children(1) != NULL)
	{
		createKdTreeArray(tree->children(1), treeArray, currentIndex);
	}
}

KdTreeTexture* createKdTreeTextureBuffers(MeshHierarchyKd* tree)
{
	int numNodes = 0;
	tree->countNode(&numNodes);

	int currentIndex = 0;
	MeshHierarchyKd** treeArray = new MeshHierarchyKd*[numNodes];
	createKdTreeArray(tree, treeArray, &currentIndex);

	KdTreeTexture* treeTexture = new KdTreeTexture(numNodes);

	int faceIndex = 0;
	for(int i=0; i<numNodes; i++)
	{
		cloneKdTreeIntTex( treeArray[i], treeTexture->texInt[i]);
		cloneKdTreeFloatTex( treeArray[i], treeTexture->texFloat[i] );
		if(treeArray[i]->isLeaf())
		{
			treeTexture->texInt[i].faceIndex = treeArray[i]->face->index;
			faceIndex++;
		}
		else
		{
			treeTexture->texInt[i].faceIndex = -1;
		}
	}
	
	delete treeArray;
	return treeTexture;
}