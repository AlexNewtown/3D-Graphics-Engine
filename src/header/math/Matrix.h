/**********************************************************************
* FILENAME :        Matrix.h
*
* DESCRIPTION :
*       Contains a Matrix class, SquareMatrix class and a 4x4 Matrix class.
*		SquareMatrix is a square matrix and contans functions only relevant to square matricies.
*		Matrix4x4 is a matrix with dimensions of 4x4 and is used for cartesian transformations.
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "GL/glew.h"
#include "GL/glut.h"
#include <math.h>

class Matrix;
class SquareMatrix;
class Matrix4x4;

/*
class Matrix
*/
class Matrix
{
public:
	/*
	Matrix constructor
	@param int n
	@param int m
	*/
	Matrix(int n, int m);
	Matrix();

	/*destructor*/
	virtual ~Matrix();

	/*getters and setters*/
	const GLfloat* const data();


	/*
	float get
	@param int i
	@param int j

	returns the element of the matrix at (i,j)
	*/
	float get(int i, int j);

	/*
	void set
	@param int i
	@param int j
	@param float val

	set the element of the matrix at (i,j) to 'val'
	*/
	void set(int i, int j, float val);
	
	/*
	void transpose

	transposes the current matrix
	*/
	void transpose();
	
	/*
	void mult
	@param Matrix* m2
	@param Matrix* result

	Multiply the current matrix and m2 and set it to result
	*/
	void mult(Matrix* m2, Matrix* result);

	/*
	void mult
	@param float v
	@param Matrix* result

	Multiply the current matrix by v and set it to result
	*/
	void mult(GLfloat v, Matrix* result);

	/*
	void multVec
	@param float* v
	@param Matrix* result

	Multiply the current matrix by the vector 'v' and set it to result
	*/
	void multVec(GLfloat* v, GLfloat* result);
	
	/*
	void add
	@param Matrix* m2

	Add the current matrix with the matrix 'm2'
	*/
	void add(Matrix* m2);
	
	/*
	void sub
	@param Matrix* m2

	Subtract the current matrix with the matrix 'm2'
	*/
	void sub(Matrix* m2);
	void copy(Matrix* m2);

	/*
	void zeros

	clears the current matrix
	*/
	void zeros();

	void printMatrix();
	void exportMatrix(const char* exportPath);
	
	int n;
	int m;
	GLfloat* mat;
protected:

};

/*
class SquareMatrix

extends 'Matrix' where SquareMatrix is assumed to be a square matrix of size 'matSize'
*/
class SquareMatrix : public Matrix
{
public:
	SquareMatrix(int matSize):Matrix(matSize,matSize)
	{
		
	}
	virtual ~SquareMatrix();
	
	/*
	void loadIdentity

	sets the current matrix to the identity matrix.
	*/
	void loadIdentity();

	/*
	void inverse
	@param SquareMatrix* result

	takes the inverse of the current matrix and returns the matrix as result.
	the inverse uses lu decomposition.
	*/
	SquareMatrix* inverse();
	void inverse(SquareMatrix* result);

	/*
	SquareMatrix* inversePsd

	@param takes the inverse of the current matrix using cholesky decomposition
	*/
	SquareMatrix* inversePsd();
	SquareMatrix* inverseSymm();

	/*
	void luDecomposition
	@param SquareMatrix* l
	@param SquareMatrix* u

	performs the lu decomposition of the current matrix and returns the matricies in l and u.
	*/
	void luDecomposition(SquareMatrix* l, SquareMatrix* u);

private:

	/*
	void choleskyDecomposition
	@param SquareMatrix* l

	Cholesky Decomposition only works if the matrix is positive semi defninite.
	*/
	void choleskyDecomposition(SquareMatrix* l);
};

/*
class Matrix4x4

extends 'SquareMatrix', Matrix4x4 is a matrix of size 4x4.
*/
class Matrix4x4:public SquareMatrix
{
public:
	/*constructor*/
	Matrix4x4():SquareMatrix(4)
	{

	}
	virtual ~Matrix4x4();
	
	/*
	void rotate
	@param float angle
	@param float x
	@param float y
	@param float z

	rotates the current matrix by 'angle' by the rotation basis <x,y,z>
	*/
	void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void rotationMatrix
	@param float angle
	@param float x
	@param float y
	@param float z
	@param SquareMatrix* result

	creates a rotationMatrix 'result' using basis <x,y,z> and 'angle'.
	*/
	void rotationMatrix(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, SquareMatrix *result);
	
	/*
	void scale
	@param float x
	@param float y
	@param float z

	scales the matrix by a factor of <x,y,z>
	*/
	void scale(GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void translate
	@param float x
	@param float y
	@param float z

	translates the current matrix by <x,y,z>
	*/
	void translate(GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void perspective
	@param float fov
	@param float aspectRatio
	@param float nearPlane
	@param float farPlane

	sets the current matrix to a perspective projection matrix with the given fov, aspectRatio, nearPlane, farPlane
	*/
	void perspective(GLfloat fov, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);
	
	/*
	void orthographic
	@param float nearPlane
	@param float farPlane

	sets the current matrix to a orthographic projection matrix with the given nearPlane, farPlane
	*/
	void orthographic(GLfloat nearPlane, GLfloat farPlane);
};


void getBasis(float* normal, float* x, float* z);

#endif