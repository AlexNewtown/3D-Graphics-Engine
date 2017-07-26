#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_SVD_BRDF_VERT "../bin/shaders/SVDBrdf/svdBrdfVert.vs"
#define SHADER_SOURCE_SVD_BRDF_GEOM "../bin/shaders/SVDBrdf/svdBrdfGeom.gs"
#define SHADER_SOURCE_SVD_BRDF_FRAG "../bin/shaders/SVDBrdf/svdBrdfFrag.fs"

class SVDBrdfShader : public CollectiveShader
{
public:
	SVDBrdfShader();
	~SVDBrdfShader();
};