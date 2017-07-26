#include "data structures\MeshStructures.h"
#include <cstdlib>
#include <typeinfo>
#include <cmath>
#include <GL\glew.h>
#include <stdio.h>
#include <ctime>

Vertex::Vertex()
{

}
Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z)
{
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}

TexCoord::TexCoord()
{

}
TexCoord::TexCoord(GLfloat x, GLfloat y)
{
	pos[0] = x;
	pos[1] = y;
}

Face::Face()
{
	for(int i=0; i<3; i++)
	{
		vertex[i] = NULL;
		normal[i] = NULL;
		texCoord[i] = NULL;
		avg[i] = 0;
		vertIndex[i] = 0;
		normalIndex[i] = 0;
		texCoordIndex[i] = 0;
	}
	brdfIndex = 0;
}

Face::~Face()
{
	delete vertex[0];
	delete vertex[1];
	delete vertex[2];

	delete normal[0];
	delete normal[1];
	delete normal[2];

	delete texCoord[0];
	delete texCoord[1];
	delete texCoord[2];
}

float Face::area()
{
	float A[] = { vertex[0]->pos[0] - vertex[1]->pos[0], vertex[0]->pos[1] - vertex[1]->pos[1] , vertex[0]->pos[2] - vertex[1]->pos[2] };
	float B[] = { vertex[0]->pos[0] - vertex[2]->pos[0], vertex[0]->pos[1] - vertex[2]->pos[1] , vertex[0]->pos[2] - vertex[2]->pos[2] };
	float C[] = { vertex[1]->pos[0] - vertex[2]->pos[0], vertex[1]->pos[1] - vertex[2]->pos[1] , vertex[1]->pos[2] - vertex[2]->pos[2] };
	float a = dotProduct(A, A);
	a = maximum(a, 0.0);

	float b = dotProduct(B, B);
	b = maximum(b, 0.0);

	float c = dotProduct(C, C);
	c = maximum(c, 0.0);

	a = sqrt(a);
	b = sqrt(b);
	c = sqrt(c);
	
	float s = (a + b + c) / 2.0;

	float ar = s*(s - a)*(s - b)*(s - c);
	ar = sqrt(ar);
	return ar;
}

float triangleArea(float* v1, float* v2, float* v3)
{
	float A[] = { v1[0] - v2[0], v1[1] - v2[1] , v1[2] - v2[2] };
	float B[] = { v1[0] - v3[0], v1[1] - v3[1] , v1[2] - v3[2] };
	float C[] = { v2[0] - v3[0], v2[1] - v3[1] , v2[2] - v3[2] };
	float a = dotProduct(A, A);
	a = maximum(a, 0.0);

	float b = dotProduct(B, B);
	b = maximum(b, 0.0);

	float c = dotProduct(C, C);
	c = maximum(c, 0.0);

	a = sqrt(a);
	b = sqrt(b);
	c = sqrt(c);

	float s = (a + b + c) / 2.0;

	float ar = s*(s - a)*(s - b)*(s - c);
	ar = maximum(ar, 0.0);
	ar = sqrt(ar);
	return ar;
}

void interpolateNormals(Face* face, float* position, float* normalResult)
{
	float areaV1 = triangleArea(face->vertex[1]->pos, face->vertex[2]->pos, position);
	float areaV2 = triangleArea(face->vertex[0]->pos, face->vertex[2]->pos, position);
	float areaV3 = triangleArea(face->vertex[0]->pos, face->vertex[1]->pos, position);
	float totalArea = areaV1 + areaV2 + areaV3;
	areaV1 = areaV1 / totalArea;
	areaV2 = areaV2 / totalArea;
	areaV3 = areaV3 / totalArea;

	normalResult[0] = areaV1 * face->normal[0]->pos[0] + areaV2 * face->normal[1]->pos[0] + areaV3 * face->normal[2]->pos[0];
	normalResult[1] = areaV1 * face->normal[0]->pos[1] + areaV2 * face->normal[1]->pos[1] + areaV3 * face->normal[2]->pos[1];
	normalResult[2] = areaV1 * face->normal[0]->pos[2] + areaV2 * face->normal[1]->pos[2] + areaV3 * face->normal[2]->pos[2];
	normalize(normalResult);
}

void listDelete(List *l, void *ptr)
{
	if(ptr == NULL)
		return;
	for(int i=0; i<l->alloc; i++)
	{
		if(l->buf[i] == ptr)
		{
			for(int j=i; j<l->alloc; j++)
			{
				l->buf[j] = l->buf[j+1];
			}
			l->alloc = l->alloc-1;
		}
	}
	return;
}

Array::~Array()
{
	delete buf;
	n = 0;
}