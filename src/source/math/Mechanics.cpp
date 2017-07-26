#include "math/Mechanics.h"

void calculatePosition(GLfloat* position, GLfloat* velocity, GLfloat* acceleration, GLfloat timeStep)
{
	position[0] = position[0] + velocity[0]*timeStep + acceleration[0]*timeStep*timeStep*0.5;
	position[1] = position[1] + velocity[1]*timeStep + acceleration[1]*timeStep*timeStep*0.5;
	position[2] = position[2] + velocity[2]*timeStep + acceleration[2]*timeStep*timeStep*0.5;
}

void calculateVelocity(GLfloat* velocity, GLfloat* acceleration, GLfloat timeStep)
{
	velocity[0] = velocity[0] + 0.5*acceleration[0]*timeStep;
	velocity[1] = velocity[1] + 0.5*acceleration[1]*timeStep;
	velocity[2] = velocity[2] + 0.5*acceleration[2]*timeStep;
}