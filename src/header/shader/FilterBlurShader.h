#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_FILTER_BLUR_VERT "../bin/shaders/FilterBlurShader/filterShaderVert.vs"
#define SHADER_SOURCE_FILTER_BLUR_GEOM "../bin/shaders/FilterBlurShader/filterShaderGeom.gs"
#define SHADER_SOURCE_FILTER_BLUR_FRAG "../bin/shaders/FilterBlurShader/filterShaderFrag.fs"

class FilterBlurShader : public CollectiveShader
{
public:
	FilterBlurShader();
	FilterBlurShader(int screenWidth, int screenHeight, char* filePath);
	~FilterBlurShader();
	virtual void render();
};