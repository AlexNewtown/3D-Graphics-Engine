#pragma once

#include "CollectiveShader.h"

#define SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_VERT "../bin/shaders/photonMapProjectionTextureShader/renderPhotonMapProjectionTextureVert.vs"
#define SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_GEOM "../bin/shaders/photonMapProjectionTextureShader/renderPhotonMapProjectionTextureGeom.gs"
#define SHADER_SOURCE_PHOTON_MAP_PROJECTION_TEXTURE_FRAG "../bin/shaders/photonMapProjectionTextureShader/renderPhotonMapProjectionTextureFrag.fs"

class PhotonMapProjectionTextureShader : public CollectiveShader
{
public:
	PhotonMapProjectionTextureShader();
	~PhotonMapProjectionTextureShader();
	virtual void render();
};

class PhotonMapTransmittanceProjectionTextureShader : public CollectiveShader
{
public:
	PhotonMapTransmittanceProjectionTextureShader();
	~PhotonMapTransmittanceProjectionTextureShader();
	virtual void render();
};