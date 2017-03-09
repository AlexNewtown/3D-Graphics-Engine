#include "../../header/shader/InstantRadiosityShader.h"

InstantRadiosityShader::InstantRadiosityShader()
{
	shaderType = SHADER_TYPE_INSTANT_RADIOSITY;

	addMain(SHADER_SOURCE_INSTANT_RADIOSITY_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_INSTANT_RADIOSITY_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_INSTANT_RADIOSITY_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
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

InstantRadiosityShader::~InstantRadiosityShader()
{

}