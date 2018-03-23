#include "../../header/shader/textureNormalizationShader.h"

TextureNormalizationShader::TextureNormalizationShader() : CollectiveShader()
{
	shaderType = SHADER_TYPE_TEXTURE_NORMALIZATION;

	addMain(SHADER_SOURCE_TEXTURE_NORMALIZATION_VERT, __shaderLinesVert);
	addMain(SHADER_SOURCE_TEXTURE_NORMALIZATION_GEOM, __shaderLinesGeom);
	addMain(SHADER_SOURCE_TEXTURE_NORMALIZATION_FRAG, __shaderLinesFrag);
	constructShader();

	GLchar** attributes = new GLchar*[1];
	attributes[0] = "position";
	addAttributes(attributes, 1);

	linkProgram();
	setPrimitiveType(GL_TRIANGLES);

	GLfloat v[] = { -1.0, 1.0, -1.0, 1.0,
		1.0, -1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0,
		1.0, -1.0, -1.0, 1.0,
		-1.0, 1.0, -1.0, 1.0,
		1.0, 1.0, -1.0, 1.0 };

	addAttributeBuffer(v, 6 * 4, sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
}
TextureNormalizationShader::~TextureNormalizationShader()
{
	
}