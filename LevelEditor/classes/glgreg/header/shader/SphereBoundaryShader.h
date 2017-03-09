#pragma once
#include "CollectiveShader.h"

#define SHADER_SOURCE_SPHERE_BOUNDARY_VERT "../bin/shaders/boundaryShader/renderRastBoundaryVert.vs"
#define SHADER_SOURCE_SPHERE_BOUNDARY_GEOM "../bin/shaders/boundaryShader/renderRastBoundaryGeom.gs"
#define SHADER_SOURCE_SPHERE_BOUNDARY_FRAG "../bin/shaders/boundaryShader/renderRastBoundaryFrag.fs"

class SphereBoundaryShader : public CollectiveShader
{
public:
	SphereBoundaryShader();
	virtual ~SphereBoundaryShader();
};