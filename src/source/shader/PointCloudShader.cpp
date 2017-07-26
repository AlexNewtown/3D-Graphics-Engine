#include "../../header/shader/PointCloudShader.h"

PointCloudShader::PointCloudShader()
{
	this->shaderType = SHADER_TYPE_POINT_CLOUD;

	addMain(SHADER_SOURCE_POINT_CLOUD_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_POINT_CLOUD_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_POINT_CLOUD_FRAG, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[3];
	attributes[0] = "position";
	attributes[1] = "normal";
	attributes[2] = "color";
	addAttributes(attributes, 3);

	linkProgram();
	setPrimitiveType(GL_POINTS);
	
}

void PointCloudShader::render()
{
	CollectiveShader::render();
}

PointCloudShader::~PointCloudShader()
{
}