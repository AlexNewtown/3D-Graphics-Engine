#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_SHADOW_MAP_PROJECTION_TEXTURE_VERT "../bin/shaders/shadowMapProjectionTextureShader/renderShadowMapProjectionTextureVert.vs"
#define SHADER_SOURCE_SHADOW_MAP_PROJECTION_TEXTURE_GEOM "../bin/shaders/shadowMapProjectionTextureShader/renderShadowMapProjectionTextureGeom.gs"
#define SHADER_SOURCE_SHADOW_MAP_PROJECTION_TEXTURE_FRAG "../bin/shaders/shadowMapProjectionTextureShader/renderShadowMapProjectionTextureFrag.fs"

class ShadowMapProjectionTextureShader : public CollectiveShader
{
public:
	ShadowMapProjectionTextureShader();
	~ShadowMapProjectionTextureShader();
};