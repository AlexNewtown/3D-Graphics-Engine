#include "entities/BoundaryEntity.h"

BoundaryEntity::BoundaryEntity(char* boundaryModel, Shader* s, int boundaryType) : Model_obj(boundaryModel, s, false)
{
	__boundaryType = boundaryType;
	this->__scale[0] = 10;
	this->__scale[1] = 10;
	this->__scale[2] = 10;

	this->__entityName = std::string("BOUNDARY");

	calculateVolume();
}

BoundaryEntity::~BoundaryEntity()
{

}

int BoundaryEntity::boundaryType()
{
	return __boundaryType;
}

float BoundaryEntity::volume()
{
	return __volume;
}

void BoundaryEntity::calculateVolume()
{

}

SphereBoundaryEntity::SphereBoundaryEntity() : BoundaryEntity(SPHERE_BOUNDARY_MODEL_FILE_PATH,new SphereBoundaryShader(), BOUNDARY_TYPE_SPHERE)
{
	__radius = __scale[0];
}

SphereBoundaryEntity::~SphereBoundaryEntity()
{

}

void SphereBoundaryEntity::setRadius(float radius)
{
	__radius = radius;
	scaleTo(radius, radius, radius);
}

void SphereBoundaryEntity::scaleTo(float x, float y, float z)
{
	Entity::scaleTo(x, y, z);
	__radius = x;
	calculateVolume();
}

float SphereBoundaryEntity::radius()
{
	return __radius;
}

void SphereBoundaryEntity::calculateVolume()
{
	__volume = (4.0 / 3.0)*pow((double)__radius, (double)3.0)*3.141592654;
}


BoundingHierarchy::BoundingHierarchy(std::vector<BoundaryEntity*> be, int sortOn)
{
	__sortOn = sortOn;
	parentBounds = calculateParentBounds(be);
	if (be.size() > 1)
	{
		sort(be);
		std::vector<BoundaryEntity*> childA;
		std::vector<BoundaryEntity*> childB;
		subdivide(be, childA, childB);
		child[0] = new BoundingHierarchy(childA, getNextSortOn());
		child[1] = new BoundingHierarchy(childB, getNextSortOn());
		switch (sortOn)
		{
		case SORT_ON_X:
			splittingPlane = child[0]->parentBounds->high->x;
			break;
		case SORT_ON_Y:
			splittingPlane = child[0]->parentBounds->high->y;
			break;

		case SORT_ON_Z:
			splittingPlane = child[0]->parentBounds->high->z;
			break;
		}
	}
	else
	{
		child[0] = NULL;
		child[1] = NULL;
		leafBoundary = be[0];
	}
}

BoundingHierarchy::~BoundingHierarchy()
{
	if (child[0] != NULL)
		delete child[0];
	if (child[1] != NULL)
		delete child[1];

	delete parentBounds;
}

void BoundingHierarchy::subdivide(std::vector<BoundaryEntity*> &be, std::vector<BoundaryEntity*> &childA, std::vector<BoundaryEntity*> &childB)
{
	float totalVolume = 0;
	for (int i = 0; i < be.size(); i++)
	{
		totalVolume = totalVolume + be[i]->volume();
	}
	
	float halfVolume = totalVolume / 2.0;
	childA.push_back(be[0]);
	float cVolume = be[0]->volume();
	int i;
	for (i = 1; i < be.size(); i++)
	{
		cVolume = cVolume + be[i]->volume();
		if (cVolume > halfVolume)
		{
			break;
		}
		childA.push_back(be[i]);
	}
	for (i; i < be.size(); i++)
		childB.push_back(be[i]);
}

int BoundingHierarchy::getNextSortOn()
{
	switch (__sortOn)
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
void BoundingHierarchy::sort(std::vector<BoundaryEntity*> &be)
{
	for (int i = 0; i < be.size(); i++)
	{
		for (int j = i+1; j < be.size(); j++)
		{
			if (be[j]->position()[__sortOn] < be[i]->position()[__sortOn])
			{
				swap(be, i, j);
			}
		}
	}
}

void BoundingHierarchy::swap(std::vector<BoundaryEntity*> &be, int i, int j)
{
	BoundaryEntity* temp = be[i];
	be[i] = be[j];
	be[j] = temp;
}

Bounds* BoundingHierarchy::calculateParentBounds(std::vector<BoundaryEntity*> be)
{
	if (be.size() == 0)
		return NULL;
	Point* low = new Point();
	Point* high = new Point();
	
	low->x = be[0]->bounds->low->x + be[0]->x();
	low->y = be[0]->bounds->low->y + be[0]->y();
	low->z = be[0]->bounds->low->z + be[0]->z();

	high->x = be[0]->bounds->high->x + be[0]->x();
	high->y = be[0]->bounds->high->y + be[0]->y();
	high->z = be[0]->bounds->high->z + be[0]->z();
	
	for (int i = 1; i < be.size(); i++)
	{
		if (be[i]->bounds->low->x + be[i]->x() < low->x)
			low->x = be[i]->bounds->low->x + be[i]->x();
		if (be[i]->bounds->low->y + be[i]->y() < low->y)
			low->y = be[i]->bounds->low->y + be[i]->y();
		if (be[i]->bounds->low->z + be[i]->z() < low->z)
			low->z = be[i]->bounds->low->z + be[i]->z();

		if (be[i]->bounds->high->x + be[i]->x() > high->x)
			high->x = be[i]->bounds->high->x + be[i]->x();
		if (be[i]->bounds->high->y + be[i]->y() > high->y)
			high->y = be[i]->bounds->high->y + be[i]->y();
		if (be[i]->bounds->high->z + be[i]->z() > high->z)
			high->z = be[i]->bounds->high->z + be[i]->z();
	}

	Bounds* b = new Bounds(low,high);
	return b;
}

int BoundingHierarchy::sortOn()
{
	return __sortOn;
}

BoundaryTreeTex::BoundaryTreeTex()
{
	boundaryType = -1;
	index = -1;
	childIndex0 = -1;
	childIndex1 = -1;
	
	radius = 0;
	position[0] = 0;
	position[1] = 0;
	position[2] = 0;

	boundaryHigh[0] = 0;
	boundaryHigh[1] = 0;
	boundaryHigh[2] = 0;
	boundaryHigh[3] = 0;
	
	boundaryLow[0] = 0;
	boundaryLow[1] = 0;
	boundaryLow[2] = 0;
	boundaryLow[3] = 0;
}

BoundaryTreeTex::BoundaryTreeTex(BoundingHierarchy* bh)
{
	if (bh->leafBoundary == NULL)
	{
		boundaryType = BOUNDARY_TYPE_AABB;

		position[0] = (bh->parentBounds->low->x + bh->parentBounds->high->x) / 2.0;
		position[1] = (bh->parentBounds->low->y + bh->parentBounds->high->y) / 2.0;
		position[2] = (bh->parentBounds->low->z + bh->parentBounds->high->z) / 2.0;

		boundaryHigh[0] = bh->parentBounds->high->x;
		boundaryHigh[1] = bh->parentBounds->high->y;
		boundaryHigh[2] = bh->parentBounds->high->z;
		boundaryHigh[3] = 1;

		boundaryLow[0] = bh->parentBounds->low->x;
		boundaryLow[1] = bh->parentBounds->low->y;
		boundaryLow[2] = bh->parentBounds->low->z;
		boundaryLow[3] = 1;

	}
	else
	{
		boundaryType = bh->leafBoundary->boundaryType();
		if (boundaryType == BOUNDARY_TYPE_AABB)
		{
			radius = -1;

		}
		else if (boundaryType == BOUNDARY_TYPE_SPHERE)
		{
			radius = ((SphereBoundaryEntity*)bh)->radius();
		}

		position[0] = (bh->leafBoundary->bounds->low->x + bh->leafBoundary->bounds->high->x) / 2.0;
		position[1] = (bh->leafBoundary->bounds->low->y + bh->leafBoundary->bounds->high->y) / 2.0;
		position[2] = (bh->leafBoundary->bounds->low->z + bh->leafBoundary->bounds->high->z) / 2.0;

		boundaryHigh[0] = bh->leafBoundary->bounds->high->x;
		boundaryHigh[1] = bh->leafBoundary->bounds->high->y;
		boundaryHigh[2] = bh->leafBoundary->bounds->high->z;
		boundaryHigh[3] = 1;

		boundaryLow[0] = bh->leafBoundary->bounds->low->x;
		boundaryLow[1] = bh->leafBoundary->bounds->low->y;
		boundaryLow[2] = bh->leafBoundary->bounds->low->z;
		boundaryLow[3] = 1;
	}

	splittingPlane = bh->splittingPlane;
	sortOn = bh->sortOn();
	index = -1;
	childIndex0 = -1;
	childIndex1 = -1;

	radius = 0;

}

void BoundingHierarchy::toBoundaryTreeTex(std::vector<BoundaryTreeTex*> &btt)
{
	BoundaryTreeTex* b = new BoundaryTreeTex(this);
	b->index = btt.size();
	btt.push_back(b);
	if (this->child[0] != NULL)
	{
		b->childIndex0 = btt.size();
		this->child[0]->toBoundaryTreeTex(btt);
	}
	if (this->child[1] != NULL)
	{
		b->childIndex1 = btt.size();
		this->child[1]->toBoundaryTreeTex(btt);
	}
}