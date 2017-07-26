#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_FRAG "../bin/shaders/GlobalIlluminationJacobiShader/GlobalIlluminationJacobiFrag.fs"
#define SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_GEOM "../bin/shaders/GlobalIlluminationJacobiShader/GlobalIlluminationJacobiGeom.gs"
#define SHADER_SOURCE_GLOBAL_ILLUMINATION_JACOBI_VERT "../bin/shaders/GlobalIlluminationJacobiShader/GlobalIlluminationJacobiVert.vs"

class GlobalIlluminationJacobiShader : public CollectiveShader
{
public:
	GlobalIlluminationJacobiShader();
	virtual ~GlobalIlluminationJacobiShader();
};