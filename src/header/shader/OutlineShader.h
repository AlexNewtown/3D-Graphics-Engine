#pragma once

#include "CollectiveShader.h"
#include "..\data structures\Point.h"
#include "..\math\Matrix.h"

#define OUTLINE_SHADER_VERT_HEADER 

#define OUTLINE_SHADER_VERT_HEADER "../bin/shaders/renderRasterize/declarations/renderRastCollectiveShaderVertDeclaration.vs"
#define OUTLINE_SHADER_GEOM_HEADER "../bin/shaders/renderRasterize/declarations/renderRastCollectiveShaderGeomDeclaration.gs"
#define OUTLINE_SHADER_FRAG_HEADER "../bin/shaders/renderRasterize/declarations/renderRastCollectiveShaderFragDeclaration.fs"

#define OUTLINE_SHADER_VERT_SOURCE "../bin/shaders/renderRasterize/definitions/renderRastOutlineShaderVertDefinitions.vs"
#define OUTLINE_SHADER_GEOM_SOURCE "../bin/shaders/renderRasterize/definitions/renderRastOutlineShaderGeomDefinitions.gs"
#define OUTLINE_SHADER_FRAG_SOURCE "../bin/shaders/renderRasterize/definitions/renderRastOutlineShaderFragDefinitions.fs"

class OutlineShader : public CollectiveShader
{
public:
	OutlineShader(Bounds* b)
	{
		addDeclarations(OUTLINE_SHADER_VERT_HEADER, __shaderLinesVert);
		addDefinitions(SHADER_MATRIX_UTILS, __shaderLinesVert);
		addDefinitions(OUTLINE_SHADER_VERT_SOURCE, __shaderLinesVert);

		addDeclarations(OUTLINE_SHADER_GEOM_HEADER, __shaderLinesGeom);
		addDefinitions(OUTLINE_SHADER_GEOM_SOURCE, __shaderLinesGeom);

		addDeclarations(OUTLINE_SHADER_FRAG_HEADER, __shaderLinesFrag);
		addDefinitions(OUTLINE_SHADER_FRAG_SOURCE, __shaderLinesFrag);

		constructShader();

		GLchar** attributes = new GLchar*[2];
		attributes[0] = "position";
		attributes[1] = "color";
		addAttributes(attributes, 2);

		linkProgram();
		setPrimitiveType(GL_LINE_STRIP);
		glLineWidth(4.0);
		setVertexAttribute(b);

		setHighlightedColour(1.0, 1.0, 1.0);

		addUniform(__highlightedColour, "highlightedColour", UNIFORM_FLOAT_4, true);
	}

	virtual ~OutlineShader();
	void addMatrices(Matrix4x4* mvm, Matrix4x4* pm);
	GLfloat highlightedColour[4];
	void setHighlightedColour(GLfloat r, GLfloat g, GLfloat b);

protected:
	void setVertexAttribute(Bounds* b);
	GLfloat __highlightedColour[4];

};