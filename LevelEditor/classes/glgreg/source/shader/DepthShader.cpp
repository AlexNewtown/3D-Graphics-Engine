#include "shader/DepthShader.h"

DepthShader::DepthShader()
{
	this->shaderType = SHADER_TYPE_DEPTH;

	addMain(SHADER_SOURCE_DEPTH_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_DEPTH_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_DEPTH_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}

DepthShader::~DepthShader()
{

}