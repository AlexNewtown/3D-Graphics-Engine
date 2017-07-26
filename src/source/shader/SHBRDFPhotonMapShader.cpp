#include "shader/SHBRDFPhotonMapShader.h"

SHBRDFPhotonMapShader::SHBRDFPhotonMapShader()
{
	shaderType = SHADER_TYPE_SH_BRDF_PHOTON_MAP;

	addMain(SHADER_SOURCE_SH_BRDF_PHOTON_MAP_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_SH_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_SH_BRDF_PHOTON_MAP_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_SH_BRDF_PHOTON_MAP_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_SH_UTILS, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[9];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "basisX";
	attributes[6] = "basisZ";
	attributes[7] = "faceIndex";
	attributes[8] = "vertIndex";
	addAttributes(attributes, 9);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);
}
SHBRDFPhotonMapShader::~SHBRDFPhotonMapShader()
{
	CollectiveShader::~CollectiveShader();
}