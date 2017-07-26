#pragma once

#include "shader/CollectiveShader.h"

#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_VERT "../bin/shaders/subsurfaceScatteringDiffuse/renderSubsurfaceScatteringDiffuseVert.vs"
#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_GEOM "../bin/shaders/subsurfaceScatteringDiffuse/renderSubsurfaceScatteringDiffuseGeom.gs"
#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_FRAG "../bin/shaders/subsurfaceScatteringDiffuse/renderSubsurfaceScatteringDiffuseFrag.fs"

class SSSDiffuseShader : public CollectiveShader
{
public:
	SSSDiffuseShader();
	~SSSDiffuseShader();
};

#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_PROJECTION_TEXTURE_VERT "../bin/shaders/subsurfaceScatteringDiffuseProjectionTexture/renderSubsurfaceScatteringDiffuseProjectionTextureVert.vs"
#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_PROJECTION_TEXTURE_GEOM "../bin/shaders/subsurfaceScatteringDiffuseProjectionTexture/renderSubsurfaceScatteringDiffuseProjectionTextureGeom.gs"
#define SHADER_SOURCE_SUBSURFACE_SCATTERING_DIFFUSE_PROJECTION_TEXTURE_FRAG "../bin/shaders/subsurfaceScatteringDiffuseProjectionTexture/renderSubsurfaceScatteringDiffuseProjectionTextureFrag.fs"

class SSSDiffuseProjectionTextureShader : public CollectiveShader
{
public:
	SSSDiffuseProjectionTextureShader();
	~SSSDiffuseProjectionTextureShader();
	virtual void render();
};