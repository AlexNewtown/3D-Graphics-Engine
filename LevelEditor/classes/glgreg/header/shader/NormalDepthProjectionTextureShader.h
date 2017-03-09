#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_NORMAL_DEPTH_PROJECTION_TEXTURE_VERT "../bin/shaders/normalDepthProjectionTexture/normalDepthProjectionTextureVert.vs"
#define SHADER_SOURCE_NORMAL_DEPTH_PROJECTION_TEXTURE_GEOM "../bin/shaders/normalDepthProjectionTexture/normalDepthProjectionTextureGeom.gs"
#define SHADER_SOURCE_NORMAL_DEPTH_PROJECTION_TEXTURE_FRAG "../bin/shaders/normalDepthProjectionTexture/normalDepthProjectionTextureFrag.fs"

class NormalDepthProjectionTextureShader : public CollectiveShader
{
public:
	NormalDepthProjectionTextureShader();
	~NormalDepthProjectionTextureShader();
	virtual void render();
};