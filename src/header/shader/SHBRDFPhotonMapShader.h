#pragma once
#include "CollectiveShader.h"

#define SHADER_SOURCE_SH_BRDF_PHOTON_MAP_VERT "../bin/shaders/SHBRDFPhotonMapShader/shBrdfPhotonMapVert.vs"
#define SHADER_SOURCE_SH_BRDF_PHOTON_MAP_GEOM "../bin/shaders/SHBRDFPhotonMapShader/shBrdfPhotonMapGeom.gs"
#define SHADER_SOURCE_SH_BRDF_PHOTON_MAP_FRAG "../bin/shaders/SHBRDFPhotonMapShader/shBrdfPhotonMapFrag.fs"

class SHBRDFPhotonMapShader : public CollectiveShader
{
public:
	SHBRDFPhotonMapShader();
	virtual ~SHBRDFPhotonMapShader();
};