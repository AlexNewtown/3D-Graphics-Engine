#ifndef MESHSTRUCTURES_H
#define MESHSTRUCTURES_H

#include <GL\glew.h>
#include "data structures/Point.h"

struct List;
struct Vertex;
struct TexCoord;
struct Edge;
struct Face;

#ifndef _WEB
template <typename T>
inline List *newList(int n)
{
	List *r;
	r = newStruct<List>(1);
	r->buf = (void**)newStruct<T*>(n);
	r->n = n;
	r->alloc = 0;
	return r;
}

struct List
{
	int n, alloc;
	void **buf;
};

template <typename T>
inline int listPush(List *l, void* ptr)
{
	if(l->alloc < l->n)	// if the list can be appended
	{
		((void**)(l->buf))[l->alloc] = ptr;
		l->alloc = l->alloc + 1;
		return (l->alloc - 1);
	}
	else	// else reallocate memory
	{	
		l->buf = (void**)reallocate<T>(l->buf,l->n);
		*((*l).buf + l->alloc) = ptr;
		l->alloc += 1;
		l->n +=  1;
		return (l->alloc - 1);
	}
}

void listDelete(List *l, void *ptr);

#endif

struct Vertex
{
	GLfloat pos[3];
};

struct Edge
{
	Vertex* vertex[2];
};

struct TexCoord
{
	GLfloat pos[2];
};

#define LIGHT_MATERIAL_INDEX -2

struct Face
{
	Face();
	virtual ~Face();
	float area();
	unsigned int index;

	Vertex* vertex[3];
	unsigned int vertIndex[3];

	Vertex* normal[3];
	unsigned int normalIndex[3];

	TexCoord* texCoord[3];
	unsigned int texCoordIndex[3];

	GLfloat avg[3];
	int materialIndex;

	int brdfIndex;
};

struct Array
{
	~Array();
	int n;
	void* buf;
};

#endif

