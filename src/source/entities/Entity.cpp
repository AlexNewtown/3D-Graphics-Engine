/**********************************************************************
* FILENAME :        Entity.h
*
* DESCRIPTION :
*       Contains a class 'Entity' that represents a physical object in the scene
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#include "entities/Entity.h"
#include "Camera.h"
#include "entities/ObjLoader.h"

Entity::Entity()
{
	__mvm = new Matrix4x4();
	__nm = new Matrix4x4();
	__mvmInverse = new Matrix4x4();
	__localMvm = new Matrix4x4();
	__localNm = new Matrix4x4();
	__mInverse = new Matrix4x4();

	__rotX = 0;
	__rotY = 0;
	__rotZ = 0;

	__scale[0] = 1;
	__scale[1] = 1;
	__scale[2] = 1;

	highlighted = false;
	selected = false;
}

Entity::~Entity()
{
	
	delete __mvm;
	delete __nm;
	delete __mvmInverse;

	delete __localMvm;
	delete __localNm;

	__entityName.clear();
}

/*
void update
@param Camera* cam

updates the entity matricies
*/
void Entity::update(Camera* cam)
{
	computeLocalMatrix();
	computeGlobalMatrix(cam);
	computeNormalMatrix();
	computeInverseMatrix(cam);
}

const std::string Entity::entityName()
{
	return __entityName;
}

void Entity::render()
{
	
}

Matrix4x4* const Entity::mvm()
{
	return __mvm;
}
Matrix4x4* const Entity::nm()
{
	return __nm;
}
Matrix4x4* const Entity::mvmInverse()
{
	return __mvmInverse;
}
Matrix4x4* const Entity::localNm()
{
	return __localNm;
}
Matrix4x4* const Entity::mInverse()
{
	return __mInverse;
}

Matrix4x4* const Entity::localMvm()
{
	return __localMvm;
}

GLfloat* Entity::position()
{
	return __pos;
}

/*
void moveGlobal
@param float x
@param float y
@param float z

moves the entity by <x,y,z>
*/
void Entity::moveGlobal(GLfloat x, GLfloat y, GLfloat z)
{
	__pos[0] = x + __pos[0];
	__pos[1] = y + __pos[1];
	__pos[2] = z + __pos[2];

}

/*
void scale
@param float x
@param float y
@param float z

scales the entity by <x,y,z>
*/
void Entity::scale(GLfloat x, GLfloat y, GLfloat z)
{
	__scale[0] = x;
	__scale[1] = y;
	__scale[2] = z;

}

/*
void rotate
@param float x
@param float y
@param float z

rotates the entity by angles <x,y,z>
*/
void Entity::rotate(GLfloat x, GLfloat y, GLfloat z)
{
	__rotX = __rotX + x;
	__rotY = __rotY + y;
	__rotZ = __rotZ + z;
}

/*
void moveTo
@float x
@float y
@float z

sets the position of the entity to <x,y,z>
*/
void Entity::moveTo(GLfloat x, GLfloat y, GLfloat z)
{
	__pos[0] = x;
	__pos[1] = y;
	__pos[2] = z;
}

/*
void scaleTo
@float x
@float y
@float z

sets the scale of the entity to <x,y,z>
*/
void Entity::scaleTo(GLfloat x, GLfloat y, GLfloat z)
{
	__scale[0] = x;
	__scale[1] = y;
	__scale[2] = z;
}

/*
void rotateTo
@float x
@float y
@float z

sets the rotation of the entity to <x,y,z>
*/
void Entity::rotateTo(GLfloat x, GLfloat y, GLfloat z)
{
	__rotX = x;
	__rotY = y;
	__rotZ = z;
}

void Entity::computeLocalMatrix()
{
	//MODEL_TRANSLATION*MODEL_ROTATION*MODEL_SCALE

	Matrix4x4 result;

	Matrix4x4 scaleMat;
	scaleMat.set(0,0,__scale[0]);
	scaleMat.set(1,1,__scale[1]);
	scaleMat.set(2,2,__scale[2]);
	scaleMat.set(3,3,1.0);

	Matrix4x4 transMat;
	transMat.loadIdentity();
	transMat.set(0,3,__pos[0]);
	transMat.set(1,3,__pos[1]);
	transMat.set(2,3,__pos[2]);

	Matrix4x4 ro;
	ro.loadIdentity();
	if(__rotX != 0.0)
		ro.rotate(__rotX, 1,0,0);
	if(__rotY != 0.0)
		ro.rotate(__rotY,0,1,0);
	if(__rotZ != 0.0)
		ro.rotate(__rotZ,0,0,1);
	
	transMat.mult(&ro,&result);
	result.mult(&scaleMat, __localMvm);

}

void Entity::computeGlobalMatrix(Camera* cam)
{
	// CAMERA_TRANSLATION*CAMERA_ROTATION*MODEL_TRANSLATION*MODEL_ROTATION*MODEL_SCALE

	Matrix4x4 result;

	cam->mvm()->mult(__localMvm,&result);
	__mvm->copy(&result);
	__mvm->inverse(__mvmInverse);
}

void Entity::computeNormalMatrix()
{
	__nm->copy(__mvm);
	SquareMatrix nmInverse(4);
	__nm->inverse(&nmInverse);
	nmInverse.transpose();
	__nm->copy(&nmInverse);

	__localMvm->inverse(&nmInverse);
	nmInverse.transpose();
	__localNm->copy(&nmInverse);

}

void Entity::computeInverseMatrix(Camera* cam)
{
	Matrix4x4 result;
	cam->projectionMatrix()->mult(__mvm, &result);
	Matrix4x4 resultInv;
	result.set(0,0,result.get(0,0) + 0.0000001);
	result.set(1,1,result.get(1,1) + 0.0000001);
	result.set(2,2,result.get(2,2) + 0.0000001);
	result.set(3,3,result.get(3,3) + 0.0000001);
	result.inverse(&resultInv);
	__mInverse->copy(&resultInv);
}

RigidBody::RigidBody(float mass)
{
	__mass = mass;
	__I = new Matrix4x4();
	__Ibody = new Matrix4x4();
	__Ibody->loadIdentity();
	__IbodyInv = new Matrix4x4();

	__position[0] = 0.0;
	__position[1] = 0.0;
	__position[2] = 0.0;
	__position[3] = 1.0;

	__rotation[0] = 0.0;
	__rotation[1] = 0.0;
	__rotation[2] = 0.0;
	__rMatrix = new Matrix4x4();
	__rMatrix->loadIdentity();

	__lMomentum[0] = 0.0;
	__lMomentum[1] = 0.0;
	__lMomentum[2] = 0.0;
	__lMomentum[3] = 0.0;

	__aMomentum[0] = 0.0;
	__aMomentum[1] = 0.0;
	__aMomentum[2] = 0.0;
	__aMomentum[3] = 0.0;

	__Iinv = new Matrix4x4();
	__velocity[0] = 0.0;
	__velocity[1] = 0.0;
	__velocity[2] = 0.0;
	__velocity[3] = 0.0;
	
	__aVelocity[0] = 0.0;
	__aVelocity[1] = 0.0;
	__aVelocity[2] = 0.0;
	__aVelocity[3] = 0.0;

	__force[0] = 0.0;
	__force[1] = 0.0;
	__force[2] = 0.0;
	__force[3] = 0.0;

	__torque[0] = 0.0;
	__torque[1] = 0.0;
	__torque[2] = 0.0;
	__torque[3] = 0.0;
}

RigidBody::~RigidBody()
{
	delete __Ibody;
	delete __IbodyInv;
	delete __rMatrix;
	delete __Iinv;
}

void RigidBody::calculateBodySpaceInertia(void** v, int n)
{
	Vertex* currentV;
	__Ibody->zeros();
	float mi = __mass/n;
	for(int i=0; i<n; i++)
	{
		currentV = (Vertex*)v[i];
		float innerProduct = currentV->pos[0]*currentV->pos[0] + currentV->pos[1]*currentV->pos[1] + currentV->pos[2]*currentV->pos[2];
		__Ibody->set(0,0,__Ibody->get(0,0) + mi*(innerProduct - currentV->pos[0]*currentV->pos[0]));
		__Ibody->set(0,1,__Ibody->get(0,1) + mi*(-currentV->pos[0]*currentV->pos[1]));
		__Ibody->set(0,2,__Ibody->get(0,2) + mi*(-currentV->pos[0]*currentV->pos[2]));

		__Ibody->set(1,0,__Ibody->get(1,0) + mi*(-currentV->pos[1]*currentV->pos[0]));
		__Ibody->set(1,1,__Ibody->get(1,1) + mi*(innerProduct - currentV->pos[1]*currentV->pos[1]));
		__Ibody->set(1,2,__Ibody->get(1,2) + mi*(-currentV->pos[0]*currentV->pos[2]));

		__Ibody->set(2,0,__Ibody->get(2,0) + mi*(-currentV->pos[2]*currentV->pos[0]));
		__Ibody->set(2,1,__Ibody->get(2,1) + mi*(-currentV->pos[2]*currentV->pos[1]));
		__Ibody->set(2,2,__Ibody->get(2,2) + mi*(innerProduct - currentV->pos[2]*currentV->pos[2]));
	}
	__Ibody->set(3,3,1.0);
	__IbodyInv = (Matrix4x4*)__Ibody->inverse();
}

void RigidBody::calculateRMatrix()
{
	__rMatrix->loadIdentity();
	if(__rotation[0] != 0.0)
		__rMatrix->rotate(__rotation[0], 1,0,0);
	if(__rotation[1] != 0.0)
		__rMatrix->rotate(__rotation[1],0,1,0);
	if(__rotation[2] != 0.0)
		__rMatrix->rotate(__rotation[2],0,0,1);
}
void RigidBody::calculateInertia()
{
	Matrix4x4 rTrans;
	rTrans.copy(__rMatrix);
	rTrans.transpose();

	Matrix4x4 rResult;
	__rMatrix->mult(__Ibody,&rResult);

	rResult.mult(&rTrans,__I);

	__I->inverse(__Iinv);
}
void RigidBody::calculateAngularVelocity()
{
	__Iinv->multVec(__aMomentum,__aVelocity);
}
void RigidBody::calculateVeclocity()
{
	__velocity[0] = __lMomentum[0]/__mass;
	__velocity[1] = __lMomentum[1]/__mass;
	__velocity[2] = __lMomentum[2]/__mass;
}

void RigidBody::update(float tStep)
{
	calculateRMatrix();
	calculateInertia();
	calculateAngularVelocity();
	calculateVeclocity();

	calculateForce(tStep);
	calculateTorque(tStep);
}

void RigidBody::calculateForce(float tStep)
{
	__force[0] = (__lMomentum[0] - __lMomentum_last[0])/tStep;
	__force[1] = (__lMomentum[1] - __lMomentum_last[1])/tStep;
	__force[2] = (__lMomentum[2] - __lMomentum_last[2])/tStep;

	__lMomentum_last[0] = __lMomentum[0];
	__lMomentum_last[1] = __lMomentum[1];
	__lMomentum_last[2] = __lMomentum[2];
}
void RigidBody::calculateTorque(float tStep)
{
	__torque[0] = (__aMomentum[0] - __aMomentum_last[0])/tStep;
	__torque[1] = (__aMomentum[1] - __aMomentum_last[1])/tStep;
	__torque[2] = (__aMomentum[2] - __aMomentum_last[2])/tStep;

	__aMomentum_last[0] = __aMomentum[0];
	__aMomentum_last[1] = __aMomentum[1];
	__aMomentum_last[2] = __aMomentum[2];
}

GLfloat Entity::x()
{
	return __pos[0];
}
GLfloat Entity::y()
{
	return __pos[1];
}
GLfloat Entity::z()
{
	return __pos[2];
}

GLfloat Entity::rotX()
{
	return __rotX;
}
GLfloat Entity::rotY()
{
	return __rotY;
}
GLfloat Entity::rotZ()
{
	return __rotZ;
}

GLfloat Entity::scaleX()
{
	return __scale[0];
}
GLfloat Entity::scaleY()
{
	return __scale[1];
}
GLfloat Entity::scaleZ()
{
	return __scale[2];
}
