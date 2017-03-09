#pragma once
#include "CollectiveShader.h"
#include "..\data structures\RadianceCacheOctree.h"
#include <stdio.h>
#include <vector>
#include "..\Controller.h"
#include "..\shading utils\PhotonMap.h"

#define SHADER_SOURCE_RADIANCE_MAP_FRAG "../bin/shaders/RadianceMap/radianceMapShaderFrag.fs"
#define SHADER_SOURCE_RADIANCE_MAP_GEOM "../bin/shaders/RadianceMap/radianceMapShaderGeom.gs"
#define SHADER_SOURCE_RADIANCE_MAP_VERT "../bin/shaders/RadianceMap/radianceMapShaderVert.vs"

class RadianceMapShader : public CollectiveShader
{
public:
	RadianceMapShader(std::vector<RadianceCache*> radianceCache);
	RadianceMapShader(std::vector<PhotonMap*> photonMap);
	virtual ~RadianceMapShader();

private:

};