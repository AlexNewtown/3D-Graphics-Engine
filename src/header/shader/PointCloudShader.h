#pragma once

#include "../../header/shader/CollectiveShader.h"
#include "../../header/openGL/Renderbuffer.h"
#include "../../header/shader/textureNormalizationShader.h"

#define SHADER_SOURCE_POINT_CLOUD_FRAG "../bin/shaders/pointCloudShader/pointCloudFrag.fs"
#define SHADER_SOURCE_POINT_CLOUD_GEOM "../bin/shaders/pointCloudShader/pointCloudGeom.gs"
#define SHADER_SOURCE_POINT_CLOUD_VERT "../bin/shaders/pointCloudShader/pointCloudVert.vs"

class PointCloudShader : public CollectiveShader
{
public:
	PointCloudShader();
	virtual ~PointCloudShader();

	virtual void render();
	TextureNormalizationShader* tns;

};