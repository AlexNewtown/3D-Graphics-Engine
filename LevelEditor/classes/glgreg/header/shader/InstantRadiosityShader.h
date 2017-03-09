#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_INSTANT_RADIOSITY_VERT "../bin/shaders/instantRadiosity/instantRadiosityVert.vs"
#define SHADER_SOURCE_INSTANT_RADIOSITY_GEOM "../bin/shaders/instantRadiosity/instantRadiosityGeom.gs"
#define SHADER_SOURCE_INSTANT_RADIOSITY_FRAG "../bin/shaders/instantRadiosity/instantRadiosityFrag.fs"

class InstantRadiosityShader : public CollectiveShader
{
public:
	InstantRadiosityShader();
	virtual ~InstantRadiosityShader();
};