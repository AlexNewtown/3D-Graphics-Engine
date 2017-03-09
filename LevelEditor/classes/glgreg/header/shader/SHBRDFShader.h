#pragma once

#include "shader\CollectiveShader.h"


#define SHADER_SOURCE_SH_BRDF_VERT "../bin/shaders/SHBRDF/shBrdfVert.vs"
#define SHADER_SOURCE_SH_BRDF_GEOM "../bin/shaders/SHBRDF/shBrdfGeom.gs"
#define SHADER_SOURCE_SH_BRDF_FRAG "../bin/shaders/SHBRDF/shBrdfFrag.fs"


class SHBRDFShader : public CollectiveShader
{
public:
	SHBRDFShader();
	~SHBRDFShader();

};