#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_POSITION_PROJECTION_TEXTURE_VERT "../bin/shaders/positionProjectionTexture/positionProjectionTextureVert.vs"
#define SHADER_SOURCE_POSITION_PROJECTION_TEXTURE_GEOM "../bin/shaders/positionProjectionTexture/positionProjectionTextureGeom.gs"
#define SHADER_SOURCE_POSITION_PROJECTION_TEXTURE_FRAG "../bin/shaders/positionProjectionTexture/positionProjectionTextureFrag.fs"

class PositionProjectionTextureShader : public CollectiveShader
{
public:
	PositionProjectionTextureShader();
	~PositionProjectionTextureShader();
	virtual void render();
};