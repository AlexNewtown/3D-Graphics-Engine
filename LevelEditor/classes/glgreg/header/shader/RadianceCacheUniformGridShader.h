#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_FRAG "../bin/shaders/RadianceCacheUniformGrid/renderRadianceCacheUniformGridFrag.fs"
#define SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_GEOM "../bin/shaders/RadianceCacheUniformGrid/renderRadianceCacheUniformGridGeom.gs"
#define SHADER_SOURCE_RADIANCE_CACHE_UNIFORM_GRID_VERT "../bin/shaders/RadianceCacheUniformGrid/renderRadianceCacheUniformGridVert.vs"

class RadianceCacheUniformGridShader : public CollectiveShader
{
public:
	RadianceCacheUniformGridShader();
	virtual ~RadianceCacheUniformGridShader();
};
