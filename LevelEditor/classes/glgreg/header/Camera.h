/**********************************************************************
* FILENAME :        Camera.h
*
* DESCRIPTION :
*       Contains a 'Camera' class which contains and manages a camera matrix and projection matrix.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include "math/Matrix.h"
#include "data structures/Point.h"

class Camera
{
public:
	/*constructors and destructor:*/
	/*
	Camera Constructor
	@param float filedOfView
	@param float aspectRatio
	@param float nearPlane : camera near clipping plane.
	@param float farPlane : camera far clipping plane.

	creates a new camera
	*/
	Camera(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);
	
	/*
	Camera Constructor
	@param float filedOfView
	@param float aspectRatio
	@param float nearPlane : camera near clipping plane.
	@param float farPlane : camera far clipping plane.
	@param Matrix4x4* mvm:

	creates a new camera and copies the modelViewMatrix 'mvm' to the camera's matrix
	*/
	Camera(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane, Matrix4x4* mvm);
	~Camera();

	GLfloat rotationX;
	GLfloat rotationY;

	/*
	void moveLocal
	@param float x
	@param float y
	@param float z

	moves the camera by an increment of <x,y,z> after a rotation is applied
	*/
	void moveLocal(GLfloat x, GLfloat y, GLfloat z);
	void resetMatrix();
	void rotateGlobal(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	void rotateLocal(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void translation

	translates the matrix by the negative camera position
	*/
	void translation();
	
	/*
	void moveGlobal

	translates the matrix by the negative camera position
	*/
	void moveGlobal(GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void moveTo
	@param float x
	@param float y
	@param float z

	moves the camera by an increment of <x,y,z>
	*/
	void moveTo(GLfloat x, GLfloat y, GLfloat z);
	
	/*Getters*/
	Matrix4x4* const projectionMatrix();
	Matrix4x4* const mvm();
	Matrix4x4* const nm();
	void getGlobalDirection(GLfloat x, GLfloat y, GLfloat z, GLfloat *globalPosition);
	void getGlobalPosition(GLfloat *globalPosition);
	int fov();
	int aspectRatio();
	int nearPlane();
	int farPlane();

	Bounds* cameraBounds;
	GLfloat xDirection[3];
	GLfloat yDirection[3];
	GLfloat zDirection[3];
	GLfloat __cameraPosition[4];

private:

	Matrix4x4* __mvm;
	Matrix4x4* __nm;
	Matrix4x4* __rMatrix;
	Matrix4x4* __invMatrix;
	Matrix4x4* __projectionMatrix;

	GLfloat __fov;
	GLfloat __aspectRatio;
	GLfloat __nearPlane;
	GLfloat __farPlane;
};

#endif