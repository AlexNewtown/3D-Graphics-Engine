/**********************************************************************
* FILENAME :        Matrix.cpp
*
* DESCRIPTION :
*       Contains a Matrix class, SquareMatrix class and a 4x4 Matrix class.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#include "math/Matrix.h"
#include <gl\GLU.h>
#include <stdio.h>
#include <math.h>
#define PI 3.14159265359

/*Matrix constructor
@param int n
@param int m
*/
Matrix::Matrix(int n, int m)
{
	this->n = n;
	this->m = m;

	/*create the matrix*/
	mat = new float[n*m];

	for( int i=0; i < n; i++)
	{
		for(int j=0; j<m; j++)
		{
			/*clear the matrix*/
			set(i,j,0.0);
		}
	}
}

Matrix::Matrix()
{
	this->n = 0;
	this->m = 0;
}

const GLfloat* const Matrix::data()
{
	return mat;
}

/*
void add
@param Matrix* m2

Add the current matrix with the matrix 'm2'
*/
void Matrix::add(Matrix* m2)
{
	if( n != m2->n && m != m2->m )
		return;

	for(int i=0; i<n; i++)
	{
		for(int j=0; j<m; j++)
		{
			set(i,j, get(i,j) + m2->get(i,j));
		}
	}
}

/*
void sub
@param Matrix* m2

Subtract the current matrix with the matrix 'm2'
*/
void Matrix::sub(Matrix* m2)
{
	if( n != m2->n && m != m2->m )
		return;

	for(int i=0; i<n; i++)
	{
		for(int j=0; j<m; j++)
		{
			set(i,j, get(i,j) - m2->get(i,j));
		}
	}
}

/*
void mult
@param Matrix* m2
@param Matrix* result

Multiply the current matrix and m2 and set it to result
*/
void Matrix::mult(Matrix* m2,Matrix *result)
{
	for(int i=0; i<this->n; i++)
	{
		for(int j=0; j<m2->m; j++)
		{
			float sum = 0.0;

			for(int k=0; k<m2->n; k++)
			{
				sum = sum + this->get(i,k)*m2->get(k,j);
			}

			result->set(i,j,sum);
		}
	}
}

/*
void mult
@param float v
@param Matrix* result

Multiply the current matrix by v and set it to result
*/
void Matrix::mult(GLfloat v,Matrix* result)
{
	int k = n;
	for(int i=0; i< k; i++)
	{
		for(int j=0; j< m; j++)
			result->set(i,j,mat[i]*v);
	}
}

/*
void multVec
@param float* v
@param Matrix* result

Multiply the current matrix by the vector 'v' and set it to result
*/
void Matrix::multVec(GLfloat* v, GLfloat* result)
{
	for(int i=0; i<n; i++)
	{
		result[i] = 0;
		for(int j=0; j<m; j++)
		{
			result[i] += get(i,j)*v[j];
		}
	}
}

/*
void copy
@param Matrix* m2

copies the matrix m2 to the current matrix
*/
void Matrix::copy(Matrix* m2)
{
	if( n != m2->n && m != m2->m )
		return;

	for(int i=0; i<n; i++)
	{
		for(int j=0; j<m; j++)
		{
			set(i,j,m2->get(i,j));
		}
	}
}

/*
void zeros

clears the current matrix
*/
void Matrix::zeros()
{
	for(int i=0; i<n;i++)
	{
		for(int j=0; j<m; j++)
		{
			set(i,j,0.0);
		}
	}
}

/*
float get
@param int i
@param int j

returns the element of the matrix at (i,j)
*/
float Matrix::get(int i, int j)
{
	return mat[m*i + j];
}

/*
void set
@param int i
@param int j
@param float val

set the element of the matrix at (i,j) to 'val'
*/
void Matrix::set(int i, int j, float val)
{
	mat[m*i + j] = val;
}

/*destructor*/
Matrix::~Matrix()
{
	delete mat;
}


void Matrix::printMatrix()
{
	printf("\n");
	printf("Matrix: ");
	printf("\n");
	for(int i=0 ;i<n; i++)
	{
		for(int j=0; j<m; j++)
		{
			printf("%0.3f   ", get(i,j) );
		}
		printf("\n");
	}
}

void Matrix::exportMatrix(const char* exportPath)
{
	FILE* file = fopen(exportPath, "w");
	if (!file)
		return;

	char* fs = new char[32];
	char delim = ';';
	char lineFeed = '\n';
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			sprintf(fs, "%0.12f", get(i, j));
			fwrite(fs, sizeof(char), 12, file);
			fwrite(&delim, sizeof(char), 1, file);
		}
		fwrite(&lineFeed, sizeof(char), 1, file);
	}

	fclose(file);
	delete fs;
}

/*
void transpose

transposes the current matrix
*/
void Matrix::transpose()
{
	Matrix tempMat(n,m);
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<m; j++)
		{
			tempMat.set(i,j,get(i,j));
		}
	}
	int matIndex = 0;
	for(int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
		{
			mat[matIndex] = tempMat.get(j,i);
			matIndex++;
		}
	}
	float temp = n;
	n = m;
	m = temp;
}

SquareMatrix::~SquareMatrix()
{
	
}

/*
void loadIdentity

sets the current matrix to the identity matrix.
*/
void SquareMatrix::loadIdentity()
{
	for(int i=0; i<this->n; i++)
	{
		for(int j = 0; j<this->n; j++)
		{
			if(i == j)
				set(i,i,1.0);
			else
				set(i,j,0.0);
		}
	}
}

/*
SquareMatrix* inversePsd

@param takes the inverse of the current matrix using cholesky decomposition
*/
SquareMatrix* SquareMatrix::inversePsd()
{
	SquareMatrix lower(n);
	choleskyDecomposition(&lower);
	SquareMatrix inv(lower.n);

	for(int i=0; i<inv.n; i++)
	{
		inv.set(i,i, 1.0/lower.get(i,i) );
	}

	for(int i=0; i<inv.n; i++)
	{
		for(int j=0; j<i; j++)
		{
			float sum = 0;
			for(int k=0; k < i - j; k++)
			{
				sum = sum - lower.get(i,j+k)*inv.get(j+k,j);
			}
			sum = sum/lower.get(i,i);
			inv.set(i,j,sum);
		}
	}
	lower.copy(&inv);
	lower.transpose();
	SquareMatrix* result = new SquareMatrix(n);
	lower.mult(&inv,result);
	return result;
}

SquareMatrix* SquareMatrix::inverseSymm()
{
	//numerical recipies 504
	return NULL;
}

/*
void choleskyDecomposition
@param SquareMatrix* l

Cholesky Decomposition only works if the matrix is positive semi defninite.
*/
void SquareMatrix::choleskyDecomposition(SquareMatrix* l)
{
	float sum = 0;
	for(int i=0; i<n; i++)
	{

		for(int k=0; k<i; k++)
		{
			sum = 0;
			for(int j=0; j< k; j++)
			{
				sum += l->get(i,j)*l->get(k,j);
			}
			l->set(i,k, (1.0/l->get(k,k))*(get(i,k) - sum));
		}

		sum = 0;
		for(int j=0; j < i; j++)
		{
			sum += pow(l->get(i,j),2);
		}
		l->set(i,i, sqrt(get(i,i) - sum) );
	}
}

/*
void luDecomposition
@param SquareMatrix* l
@param SquareMatrix* u

performs the lu decomposition of the current matrix and returns the matricies in l and u.
*/
void SquareMatrix::luDecomposition(SquareMatrix* l, SquareMatrix* u)
{
	l->loadIdentity();

	float i,j,sum;
	for(i=0; i<n; i++)
	{
		
		for(j=0; j<n; j++)
		{

			sum = 0;

			if( i > j)	// lower triangular matrix
			{
				for(int k=0; k< j; k++)
				{
					sum += u->get(k,j)*l->get(i,k);
				}

				l->set(i,j, (get(i,j) - sum)/u->get(j,j) );
			}
			else	//upper triangular matrix
			{
				for(int k=0; k< i; k++)
				{
					sum += u->get(k,j)*l->get(i,k);
				}

				u->set(i,j, get(i,j) - sum);
			}
		}
		
	}
}

/*
void inverse
@param SquareMatrix* result

takes the inverse of the current matrix and returns the matrix as result.
the inverse uses lu decomposition.
*/
void SquareMatrix::inverse(SquareMatrix* result)
{
	SquareMatrix lower(n);
	SquareMatrix lowerInv(n);

	SquareMatrix upper(n);
	SquareMatrix upperInv(n);
	luDecomposition(&lower, &upper);
	

	for(int i=0; i<n; i++)
	{
		lowerInv.set(i,i, 1.0/lower.get(i,i) );
		upperInv.set(i,i, 1.0/upper.get(i,i) );
	}

	for(int i=0; i<n; i++)
	{
		for(int j=0; j<i; j++)
		{
			float sumLower = 0;
			float sumUpper = 0;
			for(int k=0; k < i - j; k++)
			{
				sumLower = sumLower - lower.get(i,j+k)*lowerInv.get(j+k,j);
				sumUpper = sumUpper - upper.get(j+k,i)*upperInv.get(j,j+k);
			}
			sumLower = sumLower/lower.get(i,i);
			sumUpper = sumUpper/upper.get(i,i);
			
			lowerInv.set(i,j,sumLower);
			upperInv.set(j,i,sumUpper);
		}
	}
	upperInv.mult(&lowerInv,result);
}

SquareMatrix* SquareMatrix::inverse()
{
	SquareMatrix* inv = new SquareMatrix(n);
	inverse(inv);
	return inv;
}

Matrix4x4::~Matrix4x4()
{
	
}

/*
void rotationMatrix
@param float angle
@param float x
@param float y
@param float z
@param SquareMatrix* result

creates a rotationMatrix 'result' using basis <x,y,z> and 'angle'.
*/
void Matrix4x4::rotationMatrix(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, SquareMatrix* result)
{
	GLfloat sinAngle = sin(angle);
	GLfloat cosAngle = cos(angle);

	result->set(0,0, x*x + (1 - x*x)*cosAngle);
	result->set(0,1,x*y*(1- cosAngle) - z*sinAngle);
	result->set(0,2,x*z*(1 - cosAngle) + y*sinAngle);
	result->set(0,3,0);

	result->set(1,0,x*y*(1 - cosAngle) + z*sinAngle);
	result->set(1,1,y*y + (1 - y*y)*cosAngle);
	result->set(1,2,y*z*(1 - cosAngle) - x*sinAngle);
	result->set(1,3,0);

	result->set(2,0,z*x*(1 - cosAngle) - y*sinAngle);
	result->set(2,1,z*y*(1 - cosAngle) + x*sinAngle);
	result->set(2,2,z*z + (1 - z*z)*cosAngle);
	result->set(2,3,0);

	result->set(3,0,0);
	result->set(3,1,0);
	result->set(3,2,0);
	result->set(3,3,1);
}

/*
void rotate
@param float angle
@param float x
@param float y
@param float z

rotates the current matrix by 'angle' by the rotation basis <x,y,z>
*/
void Matrix4x4::rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	SquareMatrix tempMat(4);
	rotationMatrix(angle,x,y,z,&tempMat);
	SquareMatrix multResult(4);
	mult(&tempMat,&multResult);
	copy(&multResult);
}

/*
void translate
@param float x
@param float y
@param float z

translates the current matrix by <x,y,z>
*/
void Matrix4x4::translate(GLfloat x, GLfloat y, GLfloat z)
{
	SquareMatrix transMat(4);
	transMat.loadIdentity();
	transMat.set(0,3,x);
	transMat.set(1,3,y);
	transMat.set(2,3,z);
	SquareMatrix transResult(4);
	mult(&transMat,&transResult);
	copy(&transResult);
}

/*
void scale
@param float x
@param float y
@param float z

scales the matrix by a factor of <x,y,z>
*/
void Matrix4x4::scale(GLfloat x, GLfloat y, GLfloat z)
{
	Matrix4x4 scaleMat;
	scaleMat.set(1,1,x);
	scaleMat.set(2,2,y);
	scaleMat.set(3,3,z);
	scaleMat.set(4,4,1);
	Matrix4x4 result;
	this->mult(&scaleMat,&result);
	this->copy(&result);
}

/*
void perspective
@param float fov
@param float aspectRatio
@param float nearPlane
@param float farPlane

sets the current matrix to a perspective projection matrix with the given fov, aspectRatio, nearPlane, farPlane
*/
void Matrix4x4::perspective(GLfloat fov, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane)
{
	set(0,0,aspectRatio);
	set(0,1,0);
	set(0,2,0);
	set(0,3,0);

	set(1,0,0);
	set(1,1,1);
	set(1,2,0);
	set(1,3,0);

	set(2,0,0);
	set(2,1,0);
	set(2, 2, (farPlane + nearPlane) / (farPlane - nearPlane));
	set(2, 3, (-2*farPlane*nearPlane) / (farPlane - nearPlane));

	set(3,0,0);
	set(3,1,0);
	set(3,2, -1);
	set(3,3,0);
}

/*
void orthographic
@param float nearPlane
@param float farPlane

sets the current matrix to a orthographic projection matrix with the given nearPlane, farPlane
*/
void Matrix4x4::orthographic(GLfloat nearPlane, GLfloat farPlane)
{
	GLfloat l = -1.0;
	GLfloat r = 1.0;
	GLfloat t = 1.0;
	GLfloat b = -1.0;

	set(0,0,2.0/(r-l));
	set(0,1,0);
	set(0,2,0);
	set(0,3,-(r+l)/(r-l));

	set(1,0,0);
	set(1,1,2.0/(t-b));
	set(1,2,0);
	set(1,3,-(t+b)/(t-b));

	set(2,0,0);
	set(2,1,0);
	set(2,2,2/(farPlane-nearPlane));
	set(2,3,-(farPlane + nearPlane)/(farPlane - nearPlane));

	set(3,0,0);
	set(3,1,0);
	set(3,2,0);
	set(3,3,1);
}

void cross(GLfloat *a, GLfloat *b, GLfloat *result)
{
	result[0] = a[1]*b[2] - b[1]*a[2];
	result[1] = -a[0]*b[2] + b[0]*a[2];
	result[2] = a[0]*b[1] - b[0]*a[1];
}

void getBasis(float* normal, float* x, float* z)
{
	Matrix4x4 matBasis;
	matBasis.loadIdentity();
	matBasis.rotate(-atan2f(normal[0], normal[1]), 0.0, 0.0, 1.0);
	matBasis.rotate(asin(normal[2]), 1.0, 0.0, 0.0);
	x[0] = matBasis.get(0, 0);
	x[1] = matBasis.get(1, 0);
	x[2] = matBasis.get(2, 0);
	z[0] = matBasis.get(0, 2);
	z[1] = matBasis.get(1, 2);
	z[2] = matBasis.get(2, 2);
}