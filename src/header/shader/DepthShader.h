#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_DEPTH_FRAG "../bin/shaders/depth/depthRastFrag.fs"
#define SHADER_SOURCE_DEPTH_GEOM "../bin/shaders/depth/depthRastGeom.gs"
#define SHADER_SOURCE_DEPTH_VERT "../bin/shaders/depth/depthRastVert.vs"

class DepthShader : public CollectiveShader
{
public:
	DepthShader();
	virtual ~DepthShader();
};