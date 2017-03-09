#include "shader\RastShader.h"

RastShader::RastShader()
{
	this->shaderType = SHADER_TYPE_RASTERIZE;
	init();
}


RastShader::~RastShader()
{

}

void RastShader::init()
{
	addMain(SHADER_NO_LIGHTING_VERT_SOURCE, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_NO_LIGHTING_GEOM_SOURCE, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesGeom);

	addMain(SHADER_NO_LIGHTING_FRAG_SOURCE, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);

	
	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	addAttributes(attributes, 5);

	//GLchar* outlineFeedbackAttrib[2];
	//outlineFeedbackAttrib[0] = "vectorReturn";
	//addTransformFeedbackAttributes((const GLchar**)outlineFeedbackAttrib, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	//addTransfromFeedbackBuffers(1);
	//setTransformFeedback(true);
}