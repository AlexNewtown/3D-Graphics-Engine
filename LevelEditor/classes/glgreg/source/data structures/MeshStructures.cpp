#include "data structures\MeshStructures.h"
#include <cstdlib>
#include <typeinfo>
#include <cmath>
#include <GL\glew.h>
#include <stdio.h>
#include <ctime>


Face::Face()
{
	for(int i=0; i<3; i++)
	{
		vertex[i] = NULL;
		normal[i] = NULL;
		texCoord[i] = NULL;
		avg[i] = 0;
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
	float b = dotProduct(B, B);
	float c = dotProduct(C, C);
	a = sqrt(a);
	b = sqrt(b);
	c = sqrt(c);
	
	float s = (a + b + c) / 2.0;

	float ar = s*(s - a)*(s - b)*(s - c);
	ar = sqrt(ar);
	return ar;
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