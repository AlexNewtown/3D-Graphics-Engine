/**********************************************************************
* FILENAME :        Renderbuffer.cpp
*
* DESCRIPTION :
*       Contains a class 'Renderbuffer', which manages a custom render target 'Framebuffer'.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#include "openGL\Renderbuffer.h"

/*
Renderbuffer constructor
@param int w : screen width
@param int h : screen height
*/
Renderbuffer::Renderbuffer(int w, int h, int colorFormat)
{
	width = w;
	height = h;
	/*instantiate a framebuffer object*/
	framebuffer = new Framebuffer(w, h, colorFormat);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
	
	glGenRenderbuffers(1,&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
void bindFramebuffer
@param binds a renderbuffer object to a renderbuffer object
*/
void Renderbuffer::bindFramebuffer()
{
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->fbo);
}

/*
void unbindFramebuffer
@param unbinds a renderbuffer object from a renderbuffer object
*/
void Renderbuffer::unbindFramebuffer()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*destructor*/
Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffers(1, &rbo);
	delete framebuffer;
}