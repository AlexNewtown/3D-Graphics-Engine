#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_VERT "../bin/shaders/normalProjectionTexture/normalProjectionTextureVert.vs"
#define SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_GEOM "../bin/shaders/normalProjectionTexture/normalProjectionTextureGeom.gs"
#define SHADER_SOURCE_NORMAL_PROJECTION_TEXTURE_FRAG "../bin/shaders/normalProjectionTexture/normalProjectionTextureFrag.fs"

class NormalProjectionTextureShader : public CollectiveShader
{
public:
	NormalProjectionTextureShader();
	~NormalProjectionTextureShader();
};