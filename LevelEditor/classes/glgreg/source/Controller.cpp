/**********************************************************************
* FILENAME :        Controller.cpp
*
* DESCRIPTION :
*       Manages the Camera matrix Utilizing Mouse and Keyboard inputs.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#include "Controller.h"
#include "GL\glew.h"
#include "GL\glut.h"
#include "stdio.h"

#define M_PI 3.14159265358979323846
#define MOVEMENT_SPEED 3.5
static Controller* controllerInstance;

/*
Controller constructor
@param, int screenWidth
@param, int screenHeight
@param, int nearPlane: camera near plane
@param, int farPlane: camera far plane

Class that manages the camera matrix. Contains the Camera Matrix, camera position, rotation and all mouse and keyboard callbacks. 
*/
Controller::Controller(int screenWidth, int screenHeight, int nearPlane, int farPlane)
{
	__camera = new Camera(90.0,(GLfloat)screenWidth/(GLfloat)screenHeight, nearPlane, farPlane);
	
	__screenWidth = screenWidth;
	__screenHeight = screenHeight;
	__nearPlane = nearPlane;
	__farPlane = farPlane;

	__rotTheta = 0;
	__rotPhi = 0;
	__rotZ = 0.0;
	
	__mouseMiddleDown = false;
	__lastMouseX = 0;
	__lastMouseY=0;
	__mouseX = 0;
	__mouseY = 0;

	__startingRotationPhi=0;
	__startingRotationTheta=0;

	middleMouseDown = false;
	rightMouseDown = false;

	updateProjectionMatrix();
	controllerInstance = this;
}

/*destructor*/
Controller::~Controller()
{
	delete __camera;
}

/*
void update
updates the controller parameters based on mouse and keyboard inputs. 
Should be called every frame.
*/
void Controller::update()
{
#ifdef _EDITOR
	if (middleMouseDown)
	{
		/*updates camera rotation*/
		updateRotation();
	}
#endif

#ifdef _VIEWER
	/*updates camera rotation*/
	updateRotation();
#endif

#ifdef _EDITOR
	/*translate the camera is the RMB is pressed (editor only)*/
	if (rightMouseDown)
	{
		float deltaX = (float)(__mouseX - __lastMouseX) / (float)__screenWidth;
		float deltaY = (float)(__mouseY - __lastMouseY) / (float)__screenHeight;
		
		deltaX = deltaX*MOVEMENT_SPEED*100;
		deltaY = deltaY*MOVEMENT_SPEED*100;

		__camera->moveGlobal(deltaX, 0, 0);
		__camera->moveGlobal(0, -deltaY, 0);

		setStarting();
	}
#endif
}

/*
void updateRotation
@param int mouseX
@param int mouseY

updates camera rotation
*/
void Controller::updateRotation(int mouseX, int mouseY)
{
	float deltaX = (float)(mouseX - __lastMouseX);
	float deltaY = (float)(mouseY - __lastMouseY);

	__rotPhi = __startingRotationPhi + 3*((deltaY)/(__screenWidth*0.5))*M_PI;
	__rotTheta = __startingRotationTheta + ((deltaX)/(__screenHeight*0.5))*M_PI;
}

/*
void updateRotation
@param int mouseX
@param int mouseY

updates camera rotation
*/
void Controller::updateRotation()
{
	float deltaX = (float)(__mouseX - __lastMouseX);
	float deltaY = (float)(__mouseY - __lastMouseY);
#ifdef _VIEWER
	__rotPhi = __startingRotationPhi + 3 * ((deltaY) / (__screenWidth*0.5))*M_PI / 4 - M_PI / 4;
	__rotTheta = __startingRotationTheta + ((deltaX) / (__screenHeight*0.5))*M_PI - M_PI;
#endif

#ifdef _EDITOR
	__rotPhi = __startingRotationPhi + 3 * ((deltaY) / (__screenWidth*0.5))*M_PI;
	__rotTheta = __startingRotationTheta + ((deltaX) / (__screenHeight*0.5))*M_PI;
#endif
}

/*
void updateCameraMatrix
compute the camera matrix.
*/
void Controller::updateCameraMatrix()
{
	/* reset the matrix*/
	__camera->resetMatrix();
	__camera->rotationX = __rotTheta;
	__camera->rotationY = __rotPhi;

#ifdef _EDITOR
	/*translate the matrix*/
	__camera->translation();
	/*then rotate*/
	__camera->rotateGlobal(__rotPhi,1,0,0);
	__camera->rotateGlobal(__rotTheta,0,1,0);
#endif

#ifdef _VIEWER
	/*rotate the matrix*/
	__camera->rotateGlobal(__rotPhi, 1, 0, 0);
	__camera->rotateGlobal(__rotTheta, 0, 1, 0);
	/*then translate*/
	__camera->translation();
#endif

	/*compute the camera x,y,z basis*/
	GLfloat cosPhi = cos((double)__rotPhi);
	GLfloat cosTheta = cos((double)__rotTheta);
	GLfloat sinPhi = sin((double)__rotPhi);
	GLfloat sinTheta = sin((double)__rotTheta);

	__camera->zDirection[0] = sinTheta;
	__camera->zDirection[1] = 0;
	__camera->zDirection[2] = -cosTheta;

	__camera->xDirection[0] = cosTheta;
	__camera->xDirection[1] = 0;
	__camera->xDirection[2] = sinTheta;
	
	__camera->yDirection[0] = 0;
	__camera->yDirection[1] = 1;
	__camera->yDirection[2] = 0;

}

void Controller::moveTowardsOrigin(GLfloat scrollDirection)
{
	GLfloat gPos[4];
	__camera->getGlobalPosition(gPos);
	Point p(gPos[0], gPos[1], gPos[2]);
	p.normalize();
	GLfloat speed = MOVEMENT_SPEED*scrollDirection/120.0f;
	__camera->moveGlobal(speed*p.x, speed*p.y, speed*p.z);
}

void Controller::setMousePosition(int x, int y)
{
	this->__mouseX = x;
	this->__mouseY = y;
}

void Controller::setStarting()
{
	__startingRotationPhi = __rotPhi;
	__startingRotationTheta = __rotTheta;
	__lastMouseX = __mouseX;
	__lastMouseY = __mouseY;
}

int Controller::mouseX()
{
	return __mouseX;
}

int Controller::mouseY()
{
	return __mouseY;
}

void Controller::updateProjectionMatrix()
{
	//__pm->perspective(90, (GLfloat)__screenWidth/(GLfloat)__screenHeight, __nearPlane, __farPlane);
}

Matrix4x4* const Controller::pm()
{
	return __camera->projectionMatrix();
}

Camera* const Controller::camera()
{
	return __camera;
}


Controller* getControllerInstance()
{
	return controllerInstance;
}

int Controller::screenWidth() 
{
	return __screenWidth;
}
int Controller::screenHeight()
{
	return __screenHeight;
}