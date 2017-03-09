#ifndef RAST_SHADER_H
#define RAST_SHADER_H

#include "CollectiveShader.h"

#define SHADER_NO_LIGHTING_VERT_SOURCE "../bin/shaders/noLighting/renderRastVert.vs"
#define SHADER_NO_LIGHTING_GEOM_SOURCE "../bin/shaders/noLighting/renderRastGeom.gs"
#define SHADER_NO_LIGHTING_FRAG_SOURCE "../bin/shaders/noLighting/renderRastFrag.fs"

class RastShader: public CollectiveShader
{
public:
	RastShader();
	
	virtual ~RastShader();
private:
	void init();
};

#endif