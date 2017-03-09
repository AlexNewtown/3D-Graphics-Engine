#ifndef MECHANICS_H
#define MECHANICS_H

#include "GL/glew.h"

#define TIME_STEP 0.000333


void calculatePosition(GLfloat* position, GLfloat* velocity, GLfloat* acceleration, GLfloat timeStep);
void calculateVelocity(GLfloat* velocity, GLfloat* acceleration, GLfloat timeStep);

#endif