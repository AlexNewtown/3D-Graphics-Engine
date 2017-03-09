#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_SHADOW_MAP_VERT "../bin/shaders/shadowMapShader/renderShadowMapVert.vs"
#define SHADER_SOURCE_SHADOW_MAP_GEOM "../bin/shaders/shadowMapShader/renderShadowMapGeom.gs"
#define SHADER_SOURCE_SHADOW_MAP_FRAG "../bin/shaders/shadowMapShader/renderShadowMapFrag.fs"

class ShadowMapShader : public CollectiveShader
{
public:
	ShadowMapShader();
	~ShadowMapShader();
};