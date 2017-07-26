#include "shader/PhotonMapProjectionTextureShader.h"

PhotonMapProjectionTextureShader::PhotonMapProjectionTextureShader()
{
	shaderType = SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE;

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_VERT, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_BRDF_UTILS, __shaderLinesVert);
	include(SHADER_ENVIRONMENT_MAP_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	//include(SHADER_KDTREE_UTILS, __shaderLinesFrag);
	include(SHADER_PHOTON_MAP_UTILS, __shaderLinesFrag);
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
PhotonMapProjectionTextureShader::~PhotonMapProjectionTextureShader()
{

}

void PhotonMapProjectionTextureShader::render()
{
	renderFlushPrimitive();
}


PhotonMapTransmittanceProjectionTextureShader::PhotonMapTransmittanceProjectionTextureShader()
{
	shaderType = SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE;

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_VERT, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_BRDF_UTILS, __shaderLinesVert);
	include(SHADER_ENVIRONMENT_MAP_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_SHADOW_MAP_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	//include(SHADER_KDTREE_UTILS, __shaderLinesFrag);
	include(SHADER_PHOTON_MAP_UTILS, __shaderLinesFrag);
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
PhotonMapTransmittanceProjectionTextureShader::~PhotonMapTransmittanceProjectionTextureShader()
{

}
void PhotonMapTransmittanceProjectionTextureShader::render()
{
	renderFlushPrimitive();
}
