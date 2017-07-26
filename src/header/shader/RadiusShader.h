#pragma once

#include "CollectiveShader.h"
#include "../shading utils/ProgressivePhotonMap.h"

#define SHADER_SOURCE_RADIUS_VERT "../bin/shaders/RadiusShader/radiusShaderVert.vs"
#define SHADER_SOURCE_RADIUS_GEOM "../bin/shaders/RadiusShader/radiusShaderGeom.gs"
#define SHADER_SOURCE_RADIUS_FRAG "../bin/shaders/RadiusShader/radiusShaderFrag.fs"

class RadiusShader : public CollectiveShader
{
public:
	RadiusShader(std::vector<VertexRadiance*> vertexRadiance);
	virtual ~RadiusShader();
	virtual void render();

	std::vector<VertexRadiance*> vertexRadiance;
	Matrix4x4* mvm;
	Matrix4x4* pm;
};