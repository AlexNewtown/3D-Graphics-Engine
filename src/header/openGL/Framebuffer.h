/**********************************************************************
* FILENAME :        Framebuffer.h
*
* DESCRIPTION :
*       Contains a class 'Framebuffer', which creates a custom render target.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "GL\glew.h"


/*
class Framebuffer
contains an instance of a rendering target.
-color texture
-depth texture

*/
class Framebuffer
{
public:
	/*default constructor*/
	Framebuffer();

	/*
	Framebuffer constructor
	@param int w: screen width
	@param int h: screen height
	@param int cIndex: color index
	*/
	Framebuffer(int w, int h, int colorFormat);

	/*destructor*/
	virtual ~Framebuffer();

	/*framebuffer object*/
	GLuint fbo;

	/*texture objects*/
	GLuint colorTex;
	GLuint colorTex1;
	GLuint colorTex2;

	GLuint depthTex;

	int width;
	int height;
	
	/*
	GLuint createRGBATexture
	creates an RGBA Texture, returned as an unsigned int
	@param int w
	@param int h
	*/
	virtual GLuint createRGBATexture(int w, int h, int colorFormat);

	/*
	GLuint createLuminanceTexture
	creates a luminance Texture, returned as an unsigned int
	@param int w
	@param int h
	*/
	virtual GLuint createLuminanceTexture(int w, int h);

	/*
	GLuint createDepthTexture
	creates a depth Texture, returned as an unsigned int
	@param int w
	@param int h
	*/
	virtual GLuint createDepthTexture(int w, int h);
protected:

	/*
	GLuint createFBO
	@param int w
	@param int h
	creates a framebuffer object, returned as an unsigned int.
	*/
	GLuint createFBO(int w, int h);
	virtual void addTextures();
};

class FramebufferMS : public Framebuffer
{
public:
	FramebufferMS(int w, int h, int colorFormat);
	virtual ~FramebufferMS();

	virtual GLuint createRGBATexture(int w, int h, int colorFormat);
	virtual GLuint createLuminanceTexture(int w, int h);
	virtual GLuint createDepthTexture(int w, int h);

	virtual void addTextures();
};


#endif