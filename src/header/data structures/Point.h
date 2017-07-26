#ifndef POINT_H
#define POINT_H

#define SORT_ON_X 0
#define SORT_ON_Y 1
#define SORT_ON_Z 2

/* declares a 3D point */
struct Point;
struct PointArray;

/* declares 2 3D points defining the space occupied by a tree */
struct Bounds;

class Kdtree;

struct Point
{
	Point();
	Point(float _x, float _y, float _z);
	void normalize();
	float x;
	float y;
	float z;
};
struct PointArray
{
	PointArray();
	PointArray(int _numPoints);
	Point** points;
	int numPoints;
};

struct Bounds
{
	Bounds();
	Bounds(float bottomX, float bottomY, float bottomZ, float topX, float topY, float topZ);
	Bounds(Point* bottomLeft, Point* topRight);
	~Bounds();
	Point* high;
	Point* low;
	float width;
	float height;
	void reCalculate();
};

float minimum(float x, float y);
float maximum(float x, float y);
void crossProduct(float *x, float* y, float* result);
float dotProduct(float* x, float* y);

float pointAbs(Point* p);
float pointDistance(Point* p1, Point* p2);

Bounds* boundaryClone(Bounds* bounds);

bool isContained(Bounds* container, Bounds *item);
bool pointInBounds(Bounds* container, Point* item);
bool boundsOverlap(Bounds* a, Bounds* b);

void normalize(float* p);
float length(float* v);

void drawPoints(PointArray* pa);
void drawPoint(Point* p);
void printPoint(Point* p);
void drawInBounds(Point* high, Point* low);

#endif