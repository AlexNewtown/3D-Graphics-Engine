#include "../../header/shader/StochasticRayTraceShader.h"

StochasticRayTraceShader::StochasticRayTraceShader()
{
	shaderType = SHADER_TYPE_STOCHASTIC_RAY_TRACE;

	addMain(SHADER_SOURCE_STOCHASTIC_RAY_TRACE_VERT, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_BRDF_UTILS, __shaderLinesVert);
	include(SHADER_ENVIRONMENT_MAP_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_STOCHASTIC_RAY_TRACE_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_STOCHASTIC_RAY_TRACE_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_POINT_LIGHT_TEXTURE_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	include(SHADER_KDTREE_UTILS, __shaderLinesFrag);
	include(SHADER_BVH_UTILS, __shaderLinesFrag);
	include(SHADER_FACE_HIERARCHY_UTILS, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[8];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "basisX";
	attributes[6] = "basisZ";
	attributes[7] = "vertIndex";
	addAttributes(attributes, 8);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
	
	// NEED TO REMOVE HARDCODING
	//rbo = new Renderbuffer(getControllerInstance()->screenWidth(), getControllerInstance()->screenHeight(), GL_FLOAT);
	rb = new Renderbuffer(getControllerInstance()->screenWidth(), getControllerInstance()->screenHeight(), GL_FLOAT);
}

StochasticRayTraceShader::~StochasticRayTraceShader()
{

}

void StochasticRayTraceShader::render()
{
	/*
	GLuint texIndex = rbms->framebuffer->colorTex;
	
	glUseProgram(__program);

	GLuint texLocation = glGetUniformLocation(__program, "renderedImage");

	unsigned int activeTex = currentActiveTexture();

	glActiveTexture(GL_TEXTURE0 + activeTex);
	glBindTexture(GL_TEXTURE_2D, texIndex);

	glUniform1i(texLocation, activeTex);
	*/
	int* currentIteration = new int();

	int imagePixels = getControllerInstance()->screenWidth()*getControllerInstance()->screenHeight();
	int totalTextureSize = imagePixels * 4;

	GLfloat* finalImage = new GLfloat[totalTextureSize];
	GLfloat* tempImage = new GLfloat[totalTextureSize];
	for (int i = 0; i < totalTextureSize; i++)
	{
		finalImage[i] = 0.0;
	}
	std::vector<unsigned char> bufout;

	this->addUniform(currentIteration, "rSeedTime", UNIFORM_INT_1, true);

	for (int iter = 0; iter < STOCHASTIC_RAY_TRACE_ITERATIONS; iter++)
	{
		*currentIteration = time(0) % 50;
		bindUniforms();

		rb->bindFramebuffer();

		GLenum drawBufsRB[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBufsRB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderFlushPrimitive();

		rb->unbindFramebuffer();


		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, tempImage);

		
		/*int getPixelIndex(x,y)
		{
			return (y*w + x)*4 + c
		}
		*/

		int w = getControllerInstance()->screenWidth();
		int h = getControllerInstance()->screenHeight();
		int index = 0;
		int indexFinal = 0;
		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				for (int c = 0; c < 4; c++)
				{
					//w*h*4
					index = (j*w + i)*4 + c;
					indexFinal = ((h - 1 - j)*w + i) * 4 + c;
					finalImage[indexFinal] = finalImage[indexFinal] + (tempImage[index]) / (float)STOCHASTIC_RAY_TRACE_ITERATIONS;
				}
			}
		}
		

	}


	unsigned char* returnTex8U = new unsigned char[totalTextureSize];
	for (int i = 0; i < totalTextureSize; i++)
	{
		returnTex8U[i] = round(255.0*abs(finalImage[i]));
	}
	std::string fpTransmittedRadiance("../bin/exports/rayTracedImage.png");
	lodepng::encode(bufout, returnTex8U,getControllerInstance()->screenWidth(), getControllerInstance()->screenHeight());
	lodepng::save_file(bufout, fpTransmittedRadiance);
	bufout.clear();

	delete returnTex8U;
	delete tempImage;
	delete finalImage;

	GLenum drawBufs[] = { GL_BACK };
	glDrawBuffers(1, drawBufs);
}