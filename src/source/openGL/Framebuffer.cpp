/**********************************************************************
* FILENAME :        Framebuffer.cpp
*
* DESCRIPTION :
*       Contains a class 'Framebuffer', which creates a custom render target.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#include "openGL\Framebuffer.h"
#include "GL\glew.h"
#include <stdio.h>

Framebuffer::Framebuffer()
{

}

/*
Framebuffer constructor
@param int w: screen width
@param int h: screen height
@param int cIndex: color index
*/
Framebuffer::Framebuffer(int w, int h, int colorFormat)
{
	width = w;
	height = h;
	colorTex = -1;
	depthTex = -1;
	fbo = createFBO(w,h);
	colorTex = createRGBATexture(w,h,colorFormat);
	colorTex1 = createRGBATexture(w, h, colorFormat);
	colorTex2 = createRGBATexture(w, h, colorFormat);
	depthTex = createDepthTexture(w,h);

	addTextures();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
Framebuffer destructor
*/
Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &colorTex);
	glDeleteTextures(1, &colorTex1);
	glDeleteTextures(1, &colorTex2);

	glDeleteTextures(1, &depthTex);
	glDeleteFramebuffers(1, &fbo);
}

/*
GLuint createFBO
@param int w
@param int h
creates a framebuffer object, returned as an unsigned int.
*/
GLuint Framebuffer::createFBO(int w, int h)
{
	GLuint fbo;

	glGenFramebuffers(1, &fbo);
	return fbo;
}

/*
GLuint createLuminanceTexture
creates a luminance Texture, returned as an unsigned int
@param int w
@param int h
*/
GLuint Framebuffer::createLuminanceTexture(int w, int h)
{
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, h, 0, GL_LUMINANCE_ALPHA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

/*
GLuint createRGBATexture
creates an RGBA Texture, returned as an unsigned int
@param int w
@param int h
*/
GLuint Framebuffer::createRGBATexture(int w, int h, int colorFormat)
{
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, colorFormat, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

/*
GLuint createDepthTexture
creates a depth Texture, returned as an unsigned int
@param int w
@param int h
*/
GLuint Framebuffer::createDepthTexture(int w, int h)
{
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

/*
void addTextures
adds the color and depth textures to a framebuffer object. The color texture, depth texture
and framebuffer object have previously been created.
*/
void Framebuffer::addTextures()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(colorTex >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

	if (colorTex1 >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorTex1, 0);

	if (colorTex2 >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTex2, 0);

	if(depthTex >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

	GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(e)
	{
		case GL_FRAMEBUFFER_UNDEFINED:
			printf("FBO Undefined\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
			printf("FBO Incomplete Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
			printf("FBO Missing Attachment\n");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :
			printf("FBO Incomplete Draw Buffer\n");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED :
			printf("FBO Unsupported\n");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			printf("FBO OK\n");
			break;
		default:
			printf("FBO Problem?\n");
	}
}

FramebufferMS::FramebufferMS(int w, int h, int colorFormat)
{
	width = w;
	height = h;
	colorTex = -1;
	depthTex = -1;
	fbo = createFBO(w, h);
	colorTex = createRGBATexture(w, h, colorFormat);
	colorTex1 = createRGBATexture(w, h, colorFormat);
	colorTex2 = createRGBATexture(w, h, colorFormat);
	depthTex = createDepthTexture(w, h);

	addTextures();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FramebufferMS::~FramebufferMS()
{
	
}

GLuint FramebufferMS::createRGBATexture(int w, int h, int colorFormat)
{
	GLuint tex;
	glGenTextures(1, &tex);

	//glBindTexture(GL_TEXTURE_2D, tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, w, h, GL_TRUE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, colorFormat, NULL);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	return tex;
}

GLuint FramebufferMS::createLuminanceTexture(int w, int h)
{
	return Framebuffer::createLuminanceTexture(w, h);
}

GLuint FramebufferMS::createDepthTexture(int w, int h)
{
	
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);


	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT, w, h, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	return tex;
	
	//return Framebuffer::createDepthTexture(w, h);
}

void FramebufferMS::addTextures()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (colorTex >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTex, 0);

	if (colorTex1 >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, colorTex1, 0);

	if (colorTex2 >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, colorTex2, 0);

	if (depthTex >= 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthTex, 0);

	GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (e)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		printf("FBO Undefined\n");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		printf("FBO Incomplete Attachment\n");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		printf("FBO Missing Attachment\n");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		printf("FBO Incomplete Draw Buffer\n");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		printf("FBO Unsupported\n");
		break;
	case GL_FRAMEBUFFER_COMPLETE:
		printf("FBO OK\n");
		break;
	default:
		printf("FBO Problem?\n");
	}
}