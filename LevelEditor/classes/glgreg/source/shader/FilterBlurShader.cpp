#include "shader\FilterBlurShader.h"

FilterBlurShader::FilterBlurShader()
{
	this->shaderType = SHADER_TYPE_FILTER_BLUR;
	addMain(SHADER_SOURCE_FILTER_BLUR_VERT, __shaderLinesVert);
	addMain(SHADER_SOURCE_FILTER_BLUR_GEOM, __shaderLinesGeom);
	addMain(SHADER_SOURCE_FILTER_BLUR_FRAG, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[1];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	GLfloat v[] = {-1.0, 1.0, -1.0, 1.0,
					1.0, -1.0, -1.0, 1.0,
					-1.0, -1.0, -1.0, 1.0,
					1.0, -1.0, -1.0, 1.0,
					-1.0, 1.0, -1.0, 1.0,
					1.0, 1.0, -1.0, 1.0 };
	

	addAttributeBuffer(v, 6*4, sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
}

FilterBlurShader::FilterBlurShader(int screenWidth, int screenHeight, char* filePath)
{
	this->shaderType = SHADER_TYPE_FILTER_BLUR;
	addMain(SHADER_SOURCE_FILTER_BLUR_VERT, __shaderLinesVert);
	addMain(SHADER_SOURCE_FILTER_BLUR_GEOM, __shaderLinesGeom);
	addMain(SHADER_SOURCE_FILTER_BLUR_FRAG, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[1];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	GLfloat v[] = { -1.0, 1.0, -1.0, 1.0,
		1.0, -1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0,
		1.0, -1.0, -1.0, 1.0,
		-1.0, 1.0, -1.0, 1.0,
		1.0, 1.0, -1.0, 1.0 };


	addAttributeBuffer(v, 6 * 4, sizeof(GLfloat), GL_FLOAT, "position", 0, 4);

	int w = screenWidth;
	int h = screenHeight;
	addUniform(&w, "texWidth", UNIFORM_INT_1, false);
	addUniform(&h, "texHeight", UNIFORM_INT_1, false);

	unsigned int kernelW;
	unsigned int kernelH;
	std::vector<unsigned char> kernel;
	lodepng::decode(kernel, kernelW, kernelH, filePath);

	GLfloat* kernelTex = new GLfloat[kernelW*kernelH * 3];
	int index = 0;
	int kernelIndex = 0;
	for (int x = 0; x < kernelW*kernelH; x++)
	{
		kernelTex[kernelIndex] = ((float)kernel[index]) / 255.0;
		kernelTex[kernelIndex + 1] = ((float)kernel[index + 1]) / 255.0;
		kernelTex[kernelIndex + 2] = ((float)kernel[index + 2]) / 255.0;
		kernelIndex = kernelIndex + 3;
		index = index + 4;
	}
	addBufferTexture(kernelTex, kernelW*kernelH * 3, sizeof(GLfloat), "kernel", GL_RGB32F, -1);
	addUniform((void*)&kernelW, "kernelW", UNIFORM_INT_1, false);
	addUniform((void*)&kernelH, "kernelH", UNIFORM_INT_1, false);

	delete kernelTex;
	kernel.clear();
}

FilterBlurShader::~FilterBlurShader()
{

}

void FilterBlurShader::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader::render();
}