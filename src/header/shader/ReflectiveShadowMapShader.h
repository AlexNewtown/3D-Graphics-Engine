#pragma once
#include "CollectiveShader.h"

#define SHADER_SOURCE_REFLECTIVE_SHADOW_MAP_VERT "../bin/shaders/reflectiveShadowMap/renderReflectiveShadowMapVert.vs" 
#define SHADER_SOURCE_REFLECTIVE_SHADOW_MAP_GEOM "../bin/shaders/reflectiveShadowMap/renderReflectiveShadowMapGeom.gs" 
#define SHADER_SOURCE_REFLECTIVE_SHADOW_MAP_FRAG "../bin/shaders/reflectiveShadowMap/renderReflectiveShadowMapFrag.fs" 

class ReflectiveShadowMapShader : public CollectiveShader
{
public:
	ReflectiveShadowMapShader();
	~ReflectiveShadowMapShader();
};