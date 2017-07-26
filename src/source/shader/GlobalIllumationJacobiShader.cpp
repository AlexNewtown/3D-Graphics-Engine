#include "shader\GlobalIlluminationJacobiShader.h"
#include "shader\RastShader.h"

GlobalIlluminationJacobiShader::GlobalIlluminationJacobiShader() : CollectiveShader()
{
	this->shaderType = SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI;

	addMain(SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_VERT, __shaderLinesVert);
	include(SHADER_MATRIX_UTILS, __shaderLinesVert);

	addMain(SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_GEOM, __shaderLinesGeom);
	include(SHADER_MATRIX_UTILS, __shaderLinesGeom);

	addMain(SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_FRAG, __shaderLinesFrag);
	include(SHADER_MATRIX_UTILS, __shaderLinesFrag);
	include(SHADER_GEOMETRY_UTILS, __shaderLinesFrag);
	//include(SHADER_BOUNDARY_UTILS, __shaderLinesFrag);
	include(SHADER_MATERIAL_UTILS, __shaderLinesFrag);
	include(SHADER_KDTREE_UTILS, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "color";
	attributes[2] = "normal";
	attributes[3] = "materialIndex";
	attributes[4] = "textureCoord";
	attributes[5] = "illumination";
	addAttributes(attributes, 6);

	GLchar* outlineFeedbackAttrib[2];
	outlineFeedbackAttrib[0] = "vectorReturn";
	addTransformFeedbackAttributes((const GLchar**)outlineFeedbackAttrib, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	addTransfromFeedbackBuffers(1);
	setTransformFeedback(true);
}

GlobalIlluminationJacobiShader::~GlobalIlluminationJacobiShader()
{

}