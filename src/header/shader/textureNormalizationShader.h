#pragma once

#include "../../header/shader/CollectiveShader.h"

#define SHADER_SOURCE_TEXTURE_NORMALIZATION_FRAG "../bin/shaders/textureNormalizationShader/textureNormalizationFrag.fs"
#define SHADER_SOURCE_TEXTURE_NORMALIZATION_GEOM "../bin/shaders/textureNormalizationShader/textureNormalizationGeom.gs"
#define SHADER_SOURCE_TEXTURE_NORMALIZATION_VERT "../bin/shaders/textureNormalizationShader/textureNormalizationVert.vs"

class TextureNormalizationShader : public CollectiveShader
{
public:
	TextureNormalizationShader();
	virtual ~TextureNormalizationShader();

};