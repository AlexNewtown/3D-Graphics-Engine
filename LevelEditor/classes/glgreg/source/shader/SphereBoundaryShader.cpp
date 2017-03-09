#include "shader\SphereBoundaryShader.h"

SphereBoundaryShader::SphereBoundaryShader() : CollectiveShader()
{
	this->shaderType = SHADER_TYPE_RASTERIZE;

	addMain(SHADER_SOURCE_SPHERE_BOUNDARY_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_SPHERE_BOUNDARY_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_SPHERE_BOUNDARY_FRAG, __shaderLinesFrag);
	//include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}

SphereBoundaryShader::~SphereBoundaryShader()
{

}