#include "shader/SVDBrdfShader.h"

SVDBrdfShader::SVDBrdfShader()
{
	shaderType = SHADER_TYPE_SVD_BRDF;

	addMain(SHADER_SOURCE_SVD_BRDF_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_SVD_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_SVD_BRDF_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);
	include(SHADER_SVD_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_SVD_BRDF_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SVD_UTILS, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[7];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "basisX";
	attributes[6] = "basisZ";
 	addAttributes(attributes, 7);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}
SVDBrdfShader::~SVDBrdfShader()
{

}