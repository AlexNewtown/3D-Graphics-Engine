#pragma once
#include "CollectiveShader.h"

#define SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_FRAG "../bin/shaders/MonteCarloBRDF/monteCarloBRDFShaderFrag.fs"
#define SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_GEOM "../bin/shaders/MonteCarloBRDF/monteCarloBRDFShaderGeom.gs"
#define SHADER_SOURCE_MONTE_CARLO_BRDF_SHADER_VERT "../bin/shaders/MonteCarloBRDF/monteCarloBRDFShaderVert.vs"

class MonteCarloBRDFShader : public CollectiveShader
{
public:
	MonteCarloBRDFShader();
	virtual ~MonteCarloBRDFShader();
	
};