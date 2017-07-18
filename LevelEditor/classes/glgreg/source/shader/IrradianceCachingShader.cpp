#include "../../header/shader/IrradianceCachingShader.h"

IrradianceCachingShader::IrradianceCachingShader()
{
	shaderType = SHADER_TYPE_IRRADIANCE_CACHING;

	addMain(SHADER_SOURCE_IRRADIANCE_CACHING_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_IRRADIANCE_CACHING_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_IRRADIANCE_CACHING_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[8];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "basisX";
	attributes[6] = "basisZ";
	attributes[7] = "irradiance";
	addAttributes(attributes, 8);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}

IrradianceCachingShader::~IrradianceCachingShader()
{

}