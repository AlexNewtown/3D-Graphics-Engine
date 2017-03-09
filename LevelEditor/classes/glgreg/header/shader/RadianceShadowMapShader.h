#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_RADIANCE_SHADOW_MAP_VERT "../bin/shaders/radianceShadowMapShader/radianceShadowMapVert.vs"
#define SHADER_SOURCE_RADIANCE_SHADOW_MAP_GEOM "../bin/shaders/radianceShadowMapShader/radianceShadowMapGeom.gs"
#define SHADER_SOURCE_RADIANCE_SHADOW_MAP_FRAG "../bin/shaders/radianceShadowMapShader/radianceShadowMapFrag.fs"

class RadianceShadowMapShader : public CollectiveShader
{
public:
	RadianceShadowMapShader();
	virtual ~RadianceShadowMapShader();
};