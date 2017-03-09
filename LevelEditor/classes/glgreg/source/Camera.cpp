#include "Camera.h"
#include <string>

/*
Camera Constructor
@param float filedOfView
@param float aspectRatio
@param float nearPlane : camera near clipping plane.
@param float farPlane : camera far clipping plane.

creates a new camera
*/
Camera::Camera(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane)
{
	__cameraPosition[0] = 0;
	__cameraPosition[1] = 0;
	__cameraPosition[2] = 0;
	__cameraPosition[3] = 1;

	cameraBounds = new Bounds(-1, -1, -1, 1, 1, 1);

	__mvm = new Matrix4x4();
	__rMatrix = new Matrix4x4();
	__invMatrix = new Matrix4x4();
	__projectionMatrix = new Matrix4x4();

	__fov = fieldOfView;
	__aspectRatio = aspectRatio;
	__nearPlane = nearPlane;
	__farPlane = farPlane;
	__mvm->loadIdentity();
	__projectionMatrix->perspective(__fov, __aspectRatio, __nearPlane, __farPlane);
}

/*
Camera Constructor
@param float filedOfView
@param float aspectRatio
@param float nearPlane : camera near clipping plane.
@param float farPlane : camera far clipping plane.
@param Matrix4x4* mvm:

creates a new camera and copies the modelViewMatrix 'mvm' to the camera's matrix
*/
Camera::Camera(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane, Matrix4x4* mvm)
{
	__cameraPosition[0] = -mvm->get(0, 3);
	__cameraPosition[1] = -mvm->get(1, 3);
	__cameraPosition[2] = -mvm->get(2, 3);
	__cameraPosition[3] = 1;

	cameraBounds = new Bounds(-1, -1, -1, 1, 1, 1);
	__mvm = new Matrix4x4();
	__mvm->copy(mvm);
	__mvm->set(0, 3, -__mvm->get(0, 3));
	__mvm->set(1, 3, -__mvm->get(1, 3));
	__mvm->set(2, 3, -__mvm->get(2, 3));
	__invMatrix = new Matrix4x4();
	__projectionMatrix = new Matrix4x4();
	__fov = fieldOfView;
	__aspectRatio = aspectRatio;
	__nearPlane = nearPlane;
	__farPlane = farPlane;
	__projectionMatrix->perspective(__fov, __aspectRatio, __nearPlane, __farPlane);
}

/*destructor*/
Camera::~Camera()
{
	delete cameraBounds;
	delete __mvm;
	delete __nm;
	delete __rMatrix;
	delete __invMatrix;
	delete __projectionMatrix;
}

/*
void moveLocal
@param float x
@param float y
@param float z

moves the camera by an increment of <x,y,z> after a rotation is applied
*/
void Camera::moveLocal(GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat gPos[4];
	getGlobalDirection(x,y,z,gPos);
	moveGlobal(gPos[0],gPos[1],gPos[2]);
}

/*
resets the matricies
*/
void Camera::resetMatrix()
{
	__mvm->loadIdentity();
	__rMatrix->loadIdentity();
}
	
void Camera::rotateGlobal(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	__rMatrix->rotate(-angle,x,y,z);
	__mvm->rotate(angle,x,y,z);
}

void Camera::rotateLocal(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	__mvm->rotationMatrix(-angle,x,y,z,__rMatrix);
	SquareMatrix tempRotation(4);
	SquareMatrix tempResult(4);
	__mvm->rotationMatrix(angle,x,y,z,&tempRotation);
	__mvm->mult(&tempRotation,&tempResult);
	__mvm->copy(&tempResult);
	tempResult.copy(__mvm);
	tempResult.inverse(__invMatrix);
}

/*
void translation

translates the matrix by the negative camera position
*/
void Camera::translation()
{
	__mvm->translate(-__cameraPosition[0], -__cameraPosition[1], -__cameraPosition[2]);
}

/*
void moveGlobal

translates the matrix by the negative camera position
*/
void Camera::moveGlobal(GLfloat x, GLfloat y, GLfloat z)
{
	__cameraPosition[0] = __cameraPosition[0] + x;
	__cameraPosition[1] = __cameraPosition[1] + y;
	__cameraPosition[2] = __cameraPosition[2] + z;
}

/*
void moveTo
@param float x
@param float y
@param float z

moves the camera by an increment of <x,y,z>
*/
void Camera::moveTo(GLfloat x, GLfloat y, GLfloat z)
{
	__mvm->set(0, 3, x);
	__mvm->set(1, 3, y);
	__mvm->set(2, 3, z);
}

Matrix4x4* const Camera::projectionMatrix()
{
	return __projectionMatrix;
}

Matrix4x4* const Camera::mvm()
{
	return __mvm;
}

int Camera::fov()
{
	return __fov;
}
int Camera::aspectRatio()
{
	return __aspectRatio;
}
int Camera::nearPlane()
{
	return __nearPlane;
}
int Camera::farPlane()
{
	return __farPlane;
}

void Camera::getGlobalDirection(GLfloat x, GLfloat y, GLfloat z, GLfloat *globalPosition)
{
	globalPosition[0] = x*xDirection[0] + y*yDirection[0] + z*zDirection[0];
	globalPosition[1] = x*xDirection[1] + y*yDirection[1] + z*zDirection[1];
	globalPosition[2] = x*xDirection[2] + y*yDirection[2] + z*zDirection[2];
}

void Camera::getGlobalPosition(GLfloat *globalPosition)
{
	globalPosition[0] = __cameraPosition[0];
	globalPosition[1] = __cameraPosition[1];
	globalPosition[2] = __cameraPosition[2];
	globalPosition[3] = __cameraPosition[3];
}