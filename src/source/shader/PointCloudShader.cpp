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

	GLchar** attributes = new GLchar*[6];
	attributes[0] = "position";
	attributes[1] = "normal";
	attributes[2] = "color";
	attributes[3] = "quadraticPhiCoef";
	attributes[4] = "quadraticThetaCoef";
	attributes[5] = "quadraticRadCoef";
	addAttributes(attributes, 6);

	linkProgram();
	setPrimitiveType(GL_POINTS);

	tns = new TextureNormalizationShader();
}

void PointCloudShader::render()
{
	bool debug = false;
	if(!debug)
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glPointSize(0.1);
		CollectiveShader::render();
	}
	else
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glPointSize(0.1);
		CollectiveShader::render();
	}

	glDrawBuffer(GL_BACK);
}

PointCloudShader::~PointCloudShader()
{
	delete tns;
}