#include "shader\RadianceCacheUniformGridShader.h"

RadianceCacheUniformGridShader::RadianceCacheUniformGridShader() : CollectiveShader()
{
	shaderType = SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID;

	addMain(SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesVert);
	include(SHADER_MATERIAL_UTILS, __shaderLinesVert);
	include(SHADER_KDTREE_UTILS, __shaderLinesVert);
	include(SHADER_RADIANCE_CACHE_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	//include(SHADER_BOUNDARY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_KDTREE_UTILS, __shaderLinesFrag);
	include(SHADER_RADIANCE_CACHE_UTILS, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	addAttributes(attributes, 5);

	GLchar* outlineFeedbackAttrib[2];
	outlineFeedbackAttrib[0] = "vectorReturn";
	addTransformFeedbackAttributes((const GLchar**)outlineFeedbackAttrib, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	addTransfromFeedbackBuffers(1);
	setTransformFeedback(true);

}

RadianceCacheUniformGridShader::~RadianceCacheUniformGridShader()
{

}