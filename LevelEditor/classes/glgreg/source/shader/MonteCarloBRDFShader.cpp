#include "shader\MonteCarloBRDFShader.h"

MonteCarloBRDFShader::MonteCarloBRDFShader() : CollectiveShader()
{
	shaderType = SHADER_TYPE_MONTE_CARLO_BRDF;

	addMain(SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_VERT, __shaderLinesVert);
	include(SHADER_RANDOM_UTILS, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_GEOM, __shaderLinesGeom);
	include(SHADER_RANDOM_UTILS, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	//include(SHADER_BOUNDARY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	//include(SHADER_KDTREE_UTILS, __shaderLinesFrag);
	//include(SHADER_RADIANCE_CACHE_UTILS, __shaderLinesFrag);
	include(SHADER_RANDOM_UTILS, __shaderLinesFrag);
	include(SHADER_LIGHT_UTILS, __shaderLinesFrag);
	include(SHADER_BRDF_UTILS, __shaderLinesFrag);
	

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	addAttributes(attributes, 5);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

}
MonteCarloBRDFShader::~MonteCarloBRDFShader()
{

}

