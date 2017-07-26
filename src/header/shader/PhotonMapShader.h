#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_PHOTON_MAP_VERT "../bin/shaders/photonMapShader/renderPhotonMapVert.vs"
#define SHADER_SOURCE_PHOTON_MAP_GEOM "../bin/shaders/photonMapShader/renderPhotonMapGeom.gs"
#define SHADER_SOURCE_PHOTON_MAP_FRAG "../bin/shaders/photonMapShader/renderPhotonMapFrag.fs"

class PhotonMapShader : public CollectiveShader
{
public:
	PhotonMapShader();
	virtual ~PhotonMapShader();
};