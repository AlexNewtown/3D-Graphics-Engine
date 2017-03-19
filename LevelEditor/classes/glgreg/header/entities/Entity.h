/**********************************************************************
* FILENAME :        Entity.cpp
*
* DESCRIPTION :
*       Contains a class 'Entity' that represents a physical object in the scene
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef ENTITY_H
#define ENTITY_H

#include "math/Matrix.h"
#include "entities/ObjLoader.h"
#include "Camera.h"
#include "math/Mechanics.h"

class RigidBody;
class Entity;

/*class RigidBody
class used for dynamics and kinematics of entities.

localMvm is the transform matrix of the entity
mvm is the global entity matrix, mvm = localMvm*CameraMvm
nm is the normal matrix
mvmInverse is the inverse of the mvm
*/
class RigidBody
{
public:
	RigidBody(float mass);
	virtual ~RigidBody();
	void calculateBodySpaceInertia(void** v, int n);
	virtual void update(float tStep);
	
protected:
	float __mass;
	Matrix4x4* __Ibody;
	Matrix4x4* __IbodyInv;
	Matrix4x4* __I;

	float __position[4];
	float __rotation[3];
	Matrix4x4* __rMatrix;
	float __lMomentum[4];
	float __lMomentum_last[4];
	float __aMomentum[4];
	float __aMomentum_last[4];

private:
	Matrix4x4* __Iinv;
	float __velocity[4];
	float __aVelocity[4];

	float __force[4];
	float __torque[4];


	void calculateRMatrix();
	void calculateInertia();
	void calculateAngularVelocity();
	void calculateVeclocity();
	void calculateForce(float tStep);
	void calculateTorque(float tStep);
};

class Entity
{
public:
	Entity();
	virtual ~Entity();

	/*GETTERS*/
	GLfloat x();
	GLfloat y();
	GLfloat z();
	GLfloat rotX();
	GLfloat rotY();
	GLfloat rotZ();
	GLfloat scaleX();
	GLfloat scaleY();
	GLfloat scaleZ();
	GLfloat* position();
	Matrix4x4* const mvm();
	Matrix4x4* const nm();
	Matrix4x4* const mvmInverse();
	Matrix4x4* const localMvm();
	Matrix4x4* const localNm();

	bool highlighted;
	bool selected;


	virtual void render();
	
	/*
	void update
	@param Camera* cam

	updates the entity matricies
	*/
	virtual void update(Camera* cam);

	/*
	void moveGlobal
	@param float x
	@param float y
	@param float z

	moves the entity by <x,y,z>
	*/
	virtual void moveGlobal(GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void scale
	@param float x
	@param float y
	@param float z

	scales the entity by <x,y,z>
	*/
	virtual void scale(GLfloat x, GLfloat y, GLfloat z);
	
	/*
	void rotate
	@param float x
	@param float y
	@param float z

	rotates the entity by angles <x,y,z>
	*/
	virtual void rotate(GLfloat x, GLfloat y, GLfloat z);

	virtual void moveTo(GLfloat x, GLfloat y, GLfloat z);
	virtual void scaleTo(GLfloat x, GLfloat y, GLfloat z);
	virtual void rotateTo(GLfloat x, GLfloat y, GLfloat z);

	const std::string entityName();


	void computeLocalMatrix();
	void computeGlobalMatrix(Camera* cam);
	void computeNormalMatrix();
protected:
	std::string __entityName;

	Matrix4x4* __mvm;
	Matrix4x4* __nm;
	Matrix4x4* __mvmInverse;

	Matrix4x4* __localMvm;
	Matrix4x4* __localNm;

	GLfloat* __pos;

	GLfloat __rotX;
	GLfloat __rotY;
	GLfloat __rotZ;

	GLfloat __scale[3];

	int __entityType;
};

#endif