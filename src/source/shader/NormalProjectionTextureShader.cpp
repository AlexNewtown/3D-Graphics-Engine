#include "../../header/shader/NormalProjectionTextureShader.h"

NormalProjectionTextureShader::NormalProjectionTextureShader()
{
	shaderType = SHADER_TYPE_NORMAL_PROJECTION_TEXTURE;

	addMain(SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
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

NormalProjectionTextureShader::~NormalProjectionTextureShader()
{

}