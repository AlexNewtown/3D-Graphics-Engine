#include "shader/RadiusShader.h"
#include <stdio.h>
#include <vector>
#include "entities/Model.h"
#include "Controller.h"

RadiusShader::RadiusShader(std::vector<VertexRadiance*> vertexRadiance)
{
	this->shaderType = SHADER_TYPE_RADIUS_SHADER;
	addMain(SHADER_SOURCE_RADIUS_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_RADIUS_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_RADIUS_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[1];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	mvm = new Matrix4x4();
	pm = getControllerInstance()->camera()->projectionMatrix();
	this->vertexRadiance = vertexRadiance;
}

RadiusShader::~RadiusShader()
{
	CollectiveShader::~CollectiveShader();
}

void RadiusShader::render()
{
	
	Shader::render();
}