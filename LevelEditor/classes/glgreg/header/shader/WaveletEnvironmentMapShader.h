#pragma once
#include "shader/CollectiveShader.h"

#define SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_VERT "../bin/shaders/waveletEnvironmentMap/wemVert.vs"
#define SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_GEOM "../bin/shaders/waveletEnvironmentMap/wemGeom.gs"
#define SHADER_SOURCE_WAVELET_ENVIRONMENT_MAP_FRAG "../bin/shaders/waveletEnvironmentMap/wemFrag.fs"

class WaveletEnvironmentMapShader : public CollectiveShader
{
public:
	WaveletEnvironmentMapShader();
	~WaveletEnvironmentMapShader();
};