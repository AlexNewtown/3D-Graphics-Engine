#include "shader/WaveletEnvironmentMapShader.h"

WaveletEnvironmentMapShader::WaveletEnvironmentMapShader()
{
	shaderType = SHADER_TYPE_WAVELET_ENVIRONMENT_MAP;

	addMain(SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_VERT, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	//include(SHADER_SH_UTILS, __shaderLinesVert);
	//include(SHADER_RANDOM_UTILS, __shaderLinesVert);
	include(SHADER_BRDF_UTILS, __shaderLinesVert);
	include(SHADER_ENVIRONMENT_MAP_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	include(SHADER_ENVIRONMENT_MAP_UTILS, __shaderLinesFrag);
	//include(SHADER_SH_UTILS, __shaderLinesFrag);
	
	constructShader();

	GLchar** attributes = new GLchar*[8];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "basisX";
	attributes[6] = "basisZ";
	attributes[7] = "vertIndex";
	addAttributes(attributes, 8);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}

WaveletEnvironmentMapShader::~WaveletEnvironmentMapShader()
{

}