#include "shader\OutlineShader.h"

void OutlineShader::setVertexAttribute(Bounds* b)
{
	GLfloat factor = 1.08;
	GLfloat bLow[] = { factor*b->low->x,factor*b->low->y, factor*b->low->z,1.0 };
	GLfloat bHigh[] = { factor*b->high->x, factor*b->high->y, factor*b->high->z, 1.0 };

	GLfloat lineData[] =
	{
		bLow[0],bLow[1],bLow[2],1.0,
		bLow[0],bHigh[1],bLow[2],1.0,
		bLow[0],bHigh[1],bHigh[2],1.0,
		bLow[0],bLow[1],bHigh[2],1.0,
		bLow[0],bLow[1],bLow[2],1.0,
		bHigh[0],bLow[1],bLow[2],1.0,
		bHigh[0],bHigh[1],bLow[2],1.0,
		bLow[0],bHigh[1],bLow[2],1.0,
		bLow[0],bHigh[1],bHigh[2],1.0,
		bHigh[0],bHigh[1],bHigh[2],1.0,
		bHigh[0],bLow[1],bHigh[2],1.0,
		bLow[0],bLow[1],bHigh[2],1.0,
		bHigh[0],bLow[1],bHigh[2],1.0,
		bHigh[0],bLow[1],bLow[2],1.0,
		bHigh[0],bHigh[1],bLow[2],1.0,
		bHigh[0],bHigh[1],bHigh[2],1.0
	};

	//GLfloat vertexData[] = { 1.1*b->low->x, 1.1*b->low->y, 1.1*b->low->z, 1.0, 1.1*b->high->x, 1.1*b->high->y, 1.1*b->high->z, 1.0 };
	addAttributeBuffer(lineData, 64, sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
}

void OutlineShader::addMatrices(Matrix4x4* mvm, Matrix4x4* pm)
{
	addUniform((void*)mvm->data(), "modelViewMatrix", UNIFORM_MAT_4, true);
	addUniform((void*)pm->data(), "projectionMatrix", UNIFORM_MAT_4, false);
}

void OutlineShader::setHighlightedColour(GLfloat r, GLfloat g, GLfloat b)
{
	__highlightedColour[0] = r;
	__highlightedColour[1] = g;
	__highlightedColour[2] = b;
	__highlightedColour[3] = 1.0;

}

OutlineShader::~OutlineShader()
{

}