/**********************************************************************
* FILENAME :        Renderbuffer.h
*
* DESCRIPTION :
*       Contains a class 'Renderbuffer', which manages a custom render target 'Framebuffer'.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#pragma once

#include <GL/glew.h>
#include <GL/glut.h>
#include "openGL\Framebuffer.h"

/*
class Renderbuffer
creates and manages a renderbuffer objec and a framebuffer object.
*/
class Renderbuffer
{
public:
	/*constructor*/
	Renderbuffer(int w, int h, int colorFormat);
	/*deconstructor*/
	virtual ~Renderbuffer();
	
	/*
	void bindFramebuffer
	@param binds a renderbuffer object to a renderbuffer object
	*/
	void bindFramebuffer();
	
	/*
	void unbindFramebuffer
	@param unbinds a renderbuffer object from a renderbuffer object
	*/
	void unbindFramebuffer();

	GLuint rbo;
	GLuint width, height;

	Framebuffer* framebuffer;
};