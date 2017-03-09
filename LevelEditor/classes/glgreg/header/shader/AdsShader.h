#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_ADS_FRAG "../bin/shaders/ADS/renderRastFragADS.fs"
#define SHADER_SOURCE_ADS_GEOM "../bin/shaders/ADS/renderRastGeomADS.gs"
#define SHADER_SOURCE_ADS_VERT "../bin/shaders/ADS/renderRastVertexADS.vs"

class AdsShader: public CollectiveShader
{
public:
	AdsShader();
	virtual ~AdsShader();
	virtual void render();
};