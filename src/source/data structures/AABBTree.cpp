#include "data structures\AABBTree.h"

AABBTree::AABBTree(std::vector<Face*> faces)
{

	std::vector<Face*> facesCopy;
	for(int i=0; i< faces.size(); i++)
	{
		facesCopy.push_back(faces[i]);
	}
	__sortOn = 0;

	build(facesCopy,0,facesCopy.size()-1,__sortOn);
}

AABBTree::~AABBTree()
{
	delete __bounds;
	if (__children[0] != NULL)
		delete __children[0];
	if (__children[1] != NULL)
		delete __children[1];
}

AABBTree::AABBTree(std::vector<Face*> &faces, int start, int end, int sortOn)
{
	build(faces,start,end,sortOn);
}

void AABBTree::build(std::vector<Face*> &faces, int start, int end, int sortOn)
{
	__sortOn = sortOn;
	calculateBounds(faces, start,end);
	this->__children[0] = NULL;
	this->__children[1] = NULL;
	if(start == end)
	{
		
	}
	else
	{
		sort(faces,start,end,__sortOn);
		int mid = floor(((float)(start + end))/2);
		this->__children[0] = new AABBTree(faces,start,mid,getNextSortOn());
		this->__children[1] = new AABBTree(faces,mid+1,end,getNextSortOn());
	}
}

void AABBTree::calculateBounds(std::vector<Face*> faces, int start, int end)
{	
	__bounds = new Bounds(minimum(minimum(faces[start]->vertex[0]->pos[0],faces[start]->vertex[1]->pos[0]), faces[start]->vertex[2]->pos[0]),
							minimum(minimum(faces[start]->vertex[0]->pos[1],faces[start]->vertex[1]->pos[1]), faces[start]->vertex[2]->pos[1]),
							minimum(minimum(faces[start]->vertex[0]->pos[2],faces[start]->vertex[1]->pos[2]), faces[start]->vertex[2]->pos[2]),
							maximum(maximum(faces[start]->vertex[0]->pos[0],faces[start]->vertex[1]->pos[0]), faces[start]->vertex[2]->pos[0]),
							maximum(maximum(faces[start]->vertex[0]->pos[1],faces[start]->vertex[1]->pos[1]), faces[start]->vertex[2]->pos[1]),
							maximum(maximum(faces[start]->vertex[0]->pos[2],faces[start]->vertex[1]->pos[2]), faces[start]->vertex[2]->pos[2]));

	Bounds* b = new Bounds(0,0,0,0,0,0);
	for(int i=start; i<end; i++)
	{
		faceBounds(faces[i],b);
		__bounds->low->x = minimum(__bounds->low->x,b->low->x);
		__bounds->low->y = minimum(__bounds->low->y,b->low->y);
		__bounds->low->z = minimum(__bounds->low->z,b->low->z);

		__bounds->high->x = maximum(__bounds->high->x,b->high->x);
		__bounds->high->y = maximum(__bounds->high->y,b->high->y);
		__bounds->high->z = maximum(__bounds->high->z,b->high->z);
	}
	__bounds->reCalculate();
	delete b;
}

void AABBTree::faceBounds(Face* f, Bounds *b)
{
	float minX = minimum(f->vertex[0]->pos[0],f->vertex[1]->pos[0]);
	minX = minimum(minX, f->vertex[2]->pos[0]);

	float minY = minimum(f->vertex[0]->pos[1],f->vertex[1]->pos[1]);
	minY = minimum(minY, f->vertex[2]->pos[1]);

	float minZ = minimum(f->vertex[0]->pos[2], f->vertex[1]->pos[2]);
	minZ = minimum(f->vertex[2]->pos[2],minZ);



	float maxX = maximum(f->vertex[0]->pos[0],f->vertex[1]->pos[0]);
	maxX = maximum(maxX, f->vertex[2]->pos[0]);

	float maxY = maximum(f->vertex[0]->pos[1],f->vertex[1]->pos[1]);
	maxY = maximum(maxY, f->vertex[2]->pos[1]);

	float maxZ = maximum(f->vertex[0]->pos[2], f->vertex[1]->pos[2]);
	maxZ = maximum(f->vertex[2]->pos[2],maxZ);


	b->low->x = minX;
	b->low->y = minY;
	b->low->z = minZ;

	b->high->x = maxX;
	b->high->y = maxY;
	b->high->z = maxZ;
}

void AABBTree::sort(std::vector<Face*> &faces, int start, int end, int sortOn)
{
	int mid;
	if( end > start)
	{
		mid = (int)floor((end + start)*0.5);
		sort(faces, start, mid, sortOn);
		sort(faces, mid+1,end, sortOn);

		merge(faces, start, mid+1, end, sortOn);
	}
}



void AABBTree::merge(std::vector<Face*> &faces, int start, int mid, int end, int sortOn)
{
	int i, leftEnd, numElements, tempPos;

	leftEnd = mid;
	tempPos = 0;
	numElements = end- start;
	std::vector<Face*> tempArray;
	while(start < leftEnd && mid <= end)
	{
		if( faces[start]->avg[sortOn]<= faces[mid]->avg[sortOn])
		{
			tempArray.push_back(faces[start]);
			start++;
		}
		else
		{
			tempArray.push_back(faces[mid]);
			mid++;
		}
	}

	while(start < leftEnd)
	{
		tempArray.push_back(faces[start]);
		start++;
	}
	while(mid <= end)
	{
		tempArray.push_back(faces[mid]);
		mid++;
	}

	for(int i=tempArray.size()-1; i>=0; i--)
	{
		faces[end] = tempArray[i];
		end--;
	}
}

int AABBTree::getNextSortOn()
{
	switch(__sortOn)
	{
	case SORT_X:
		return SORT_Y;
		break;

	case SORT_Y:
		return SORT_Z;
		break;

	case SORT_Z:
		return SORT_X;
		break;
	}
	return SORT_X;
}

Bounds* AABBTree::bounds()
{
	return __bounds;
}