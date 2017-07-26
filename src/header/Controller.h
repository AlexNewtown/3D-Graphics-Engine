/**********************************************************************
* FILENAME :        Controller.cpp
*
* DESCRIPTION :
*       Manages the Camera matrix Utilizing Mouse and Keyboard inputs.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "math/Matrix.h"
#include "Camera.h"

class Controller
{
public:

	/*
	Controller constructor
	@param, int screenWidth
	@param, int screenHeight
	@param, int nearPlane: camera near plane
	@param, int farPlane: camera far plane

	Class that manages the camera matrix. Contains the Camera Matrix, camera position, rotation and all mouse and keyboard callbacks.
	*/
	Controller(int screenWidth, int ScreenHeight, int nearPlane, int farPlane);
	~Controller();

	/*update functions*/
	/*
	void update
	updates the controller parameters based on mouse and keyboard inputs.
	Should be called every frame.
	*/
	void update();

	/*
	void updateRotation
	@param int mouseX
	@param int mouseY

	updates camera rotation
	*/
	void updateRotation(int mouseX, int mouseY);
	
	/*
	void updateRotation
	@param int mouseX
	@param int mouseY

	updates camera rotation
	*/
	void updateRotation();

	/*
	void updateCameraMatrix
	compute the camera matrix.
	*/
	void updateCameraMatrix();
	
	void setMousePosition(int x, int y);
	void setStarting();
	void moveTowardsOrigin(GLfloat scrollDirection);

	/*Getters*/
	Matrix4x4* const pm();
	Camera* const camera();
	int mouseX();
	int mouseY();
	int screenWidth();
	int screenHeight();

	bool middleMouseDown;
	bool rightMouseDown;

private:
	int __buttonState;
	int __screenWidth;
	int __screenHeight;
	int __nearPlane;
	int __farPlane;
	Camera* __camera;
	float __rotTheta;
	float __rotPhi;
	float __rotZ;
	int __mouseX;
	int __mouseY;
	int __lastMouseX;
	int __lastMouseY;
	bool __mouseMiddleDown;
	GLfloat __startingRotationPhi;
	GLfloat __startingRotationTheta;
	bool __leftMouseDown;
	GLfloat localTransform[4];
	void updateProjectionMatrix();
};

/*global controller instance*/
Controller* getControllerInstance();

#endif