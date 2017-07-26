#pragma once
#include "CollectiveShader.h"
#include "../openGL/Renderbuffer.h"
#include "../Controller.h"
#include <time.h>

#define STOCHASTIC_RAY_TRACE_ITERATIONS 1

#define SHADER_SOURCE_STOCHASTIC_RAY_TRACE_VERT "../bin/shaders/StochasticRayTrace/stochasticRayTraceVert.vs"
#define SHADER_SOURCE_STOCHASTIC_RAY_TRACE_GEOM "../bin/shaders/StochasticRayTrace/stochasticRayTraceGeom.gs"
#define SHADER_SOURCE_STOCHASTIC_RAY_TRACE_FRAG "../bin/shaders/StochasticRayTrace/stochasticRayTraceFrag.fs"

class StochasticRayTraceShader : public CollectiveShader
{
public:
	StochasticRayTraceShader();
	~StochasticRayTraceShader();
	virtual void render();
	
	//Renderbuffer* rbo;
	Renderbuffer* rb;
};