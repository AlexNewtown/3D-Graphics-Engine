#pragma once
#include "CollectiveShader.h"

#define SHADER_SOURCE_IRRADIANCE_CACHING_VERT "../bin/shaders/irradianceCaching/irradianceCachingVert.vs" 
#define SHADER_SOURCE_IRRADIANCE_CACHING_GEOM "../bin/shaders/irradianceCaching/irradianceCachingGeom.gs" 
#define SHADER_SOURCE_IRRADIANCE_CACHING_FRAG "../bin/shaders/irradianceCaching/irradianceCachingFrag.fs" 

class IrradianceCachingShader : public CollectiveShader
{
public:
	IrradianceCachingShader();
	~IrradianceCachingShader();
};