#pragma once

#include "Model.h"
#include "shader\RastShader.h"

#define POINT_LIGHT 0
#define SURFACE_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define FRUSTUM_LIGHT 3

#define SPHERE_MODEL_SPHERE "../bin/assets/obj/sphere/sphere.obj"
#define SPHERE_MODEL_ICOS "../bin/assets/obj/icos/icos.obj"
#define FRUSTUM_MODEL "../bin/assets/obj/frustum/frustum.obj"

#define FRUSTUM_LIGHT_NEAR_PLANE -1
#define FRUSTUM_LIGHT_FAR_PLANE -3000

const char* getLightModelPath(unsigned int lightType);

class LightEntity : public Model_obj
{
public:
	LightEntity(int lightType);
	virtual ~LightEntity();
	virtual void update(Camera* cam);
	float intensity;
	int lightType;
	GLfloat globalPosition[4];
};

class FrustumLight : public LightEntity
{
public:
	FrustumLight(GLfloat fov, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);
	virtual ~FrustumLight();
	virtual void update(Camera* cam);
	Matrix4x4* projectionMatrix;
	float fov, aspectRatio, nearPlane, farPlane;
	float projectionWidth;
	float projectionHeight;
};
