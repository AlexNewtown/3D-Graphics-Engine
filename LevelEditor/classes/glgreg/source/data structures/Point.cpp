#include "data structures\Point.h"
#include <math.h>
#include "GL\glew.h"
#include <iostream>

using namespace std;

Point::Point()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Point::Point(float _x, float _y, float _z)
{
	this->x = _x;
	this->y = _y;
	this->z = _z;
}

PointArray::PointArray()
{

}

PointArray::PointArray(int _numPoints)
{
	points = new Point*[_numPoints];
	numPoints = _numPoints;
	for(int i=0; i<_numPoints; i++)
	{
		points[i] = new Point();
	}
}

Bounds::Bounds()
{
	low = NULL;
	high = NULL;
}

Bounds::Bounds(float bottomX, float bottomY, float bottomZ, float topX, float topY, float topZ)
{
	Point* low = new Point();
	Point* high = new Point();

	low->x = bottomX;
	low->y = bottomY;
	low->z = bottomZ;
	high->x = topX;
	high->y = topY;
	high->z = topZ;
	this->low = low;
	this->high = high;

	width = topX - bottomX;
	height = topY - bottomY;
}

Bounds::Bounds(Point* bottomLeft, Point* topRight)
{
	low = bottomLeft;
	high = topRight;
	width = topRight->x - bottomLeft->x;
	height = topRight->y - bottomLeft->y;
}

Bounds::~Bounds()
{
	delete high;
	delete low;
}

Bounds* boundaryClone(Bounds* bounds)
{
	Point* high = new Point();
	high->x = bounds->high->x;
	high->y = bounds->high->y;
	high->z = bounds->high->z;

	Point* low = new Point();
	low->x = bounds->low->x;
	low->y = bounds->low->y;
	low->z = bounds->low->z;

	Bounds* clone = new Bounds(low,high);
	return clone;
}

float minimum(float x, float y)
{
	if( x <= y)
		return x;
	else
		return y;
}
float maximum(float x, float y)
{
	if( x >= y)
		return x;
	else
		return y;
}

void crossProduct(float* x, float* y, float* result)
{
	float r1 = x[1] * y[2];
	float r2 = x[2] * y[1];
	result[0] = x[1]*y[2] - x[2]*y[1];//aybz - azby
	result[1] = x[2]*y[0] - x[0]*y[2];//azbx - axbz
	result[2] = x[0]*y[1] - x[1]*y[0];//axby - aybx
}

float dotProduct(float* x, float* y)
{
	float r = (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
	return r;
}

void Bounds::reCalculate()
{
	width = high->x - low->x;
	height = high->y - low->y;
}

bool isContained(Bounds* container, Bounds *item)
{
	if(item->low->x < container->low->x)
		return false;
	if(item->low->y < container->low->y)
		return false;
	if(item->low->z < container->low->z)
		return false;

	if(item->high->x > container->high->x)
		return false;
	if(item->high->y > container->high->y)
		return false;
	if(item->high->z > container->high->z)
		return false;
	return true;
}

bool pointInBounds(Bounds* container, Point* item)
{
	if(item->x < container->low->x || item->y < container->low->y || item->z < container->low->z)
		return false;
	if(item->x > container->high->x || item->y > container->high->y || item->z > container->high->z)
		return false;
	return true;
}

bool segmentContained(float containerLow, float containerHigh, float itemLow, float itemHigh)
{
	if(itemLow < containerLow)
		return false;
	if(itemHigh > containerHigh)
		return false;
	return true;
}

float pointDistance(Point* startPoint, Point* endPoint)
{
	float x = endPoint->x - startPoint->x;
	float y = endPoint->y - startPoint->y;
	float z = endPoint->z - startPoint->z;
	float mag = 0;
	mag += x*x;
	mag += y*y;
	mag += z*z;
	mag = sqrt(mag);
	return mag;
}

void Point::normalize()
{
	float mag = 0;
	mag += (x)*(x);
	mag += (y)*(y);
	mag += (z)*(z);
	if(mag == 0) return;
	mag = sqrt(mag);
	x = x/mag;
	y = y/mag;
	z = z/mag;
}

void normalize(float* p)
{
	float mag = 0;
	mag += (p[0])*(p[0]);
	mag += (p[1])*(p[1]);
	mag += (p[2])*(p[2]);
	if (mag == 0.0) return;
	mag = sqrt(mag);
	p[0] = p[0] / mag;
	p[1] = p[1] / mag;
	p[2] = p[2] / mag;
}

float length(float* v)
{
	float l = 0;
	l += v[0] * v[0];
	l += v[1] * v[1];
	l += v[2] * v[2];
	return sqrt(l);
}

void drawPoints(PointArray* pa)
{
	Point* currentPoint;
	glPointSize(10.0);
	glBegin(GL_POINTS);
	for(int i=0; i<pa->numPoints; i++)
	{
		currentPoint = pa->points[i];
		glVertex3f(0.0,0.0,0.0);
	}
	glEnd();
}

void drawPoint(Point* p)
{
	glBegin(GL_POINTS);
	glColor3f(1.0,0.0,1.0);
	glVertex3f(p->x,p->y,p->z);
	glEnd();
}
void printPoint(Point* p)
{
	cout << "x:";
	cout << p->x;
	cout << "  ";

	cout << "y:";
	cout << p->y;
	cout << "  ";

	cout << "z:";
	cout << p->z;
	cout << "  ";

	cout << "\n";
}

bool boundsOverlap(Bounds* a, Bounds* b)
{
	Point v1,v2,v3,v4,v5,v6,v7,v8;
	
	v1.x = b->low->x;
	v1.y = b->low->y;
	v1.z = b->low->z;

	v2.x = b->high->x;
	v2.y = b->low->y;
	v2.z = b->low->z;

	v3.x = b->low->x;
	v3.y = b->high->y;
	v3.z = b->low->z;

	v4.x = b->high->x;
	v4.y = b->high->y;
	v4.z = b->low->z;

	v5.x = b->low->x;
	v5.y = b->low->y;
	v5.z = b->high->z;

	v6.x = b->high->x;
	v6.y = b->low->y;
	v6.z = b->high->z;

	v7.x = b->low->x;
	v7.y = b->high->y;
	v7.z = b->high->z;

	v8.x = b->high->x;
	v8.y = b->high->y;
	v8.z = b->high->z;

	if( pointInBounds(a, &v1) )
		return true;
	if( pointInBounds(a, &v2) )
		return true;
	if( pointInBounds(a, &v3) )
		return true;
	if( pointInBounds(a, &v4) )
		return true;
	if( pointInBounds(a, &v5) )
		return true;
	if( pointInBounds(a, &v6) )
		return true;
	if( pointInBounds(a, &v7) )
		return true;
	if( pointInBounds(a, &v8) )
		return true;

	return false;
}