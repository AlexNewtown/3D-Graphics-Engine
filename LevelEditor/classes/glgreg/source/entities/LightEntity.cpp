#include "entities/LightEntity.h"
#include "Controller.h"

const char* getLightModelPath(unsigned int lightType)
{
	switch (lightType)
	{
	case POINT_LIGHT :
		return FRUSTUM_MODEL;
		break;
	case SURFACE_LIGHT :
		return FRUSTUM_MODEL;
		break;
	case DIRECTIONAL_LIGHT :
		return NULL;
		break;
	case FRUSTUM_LIGHT:
		return FRUSTUM_MODEL;
		break;
	}
	return FRUSTUM_MODEL;

}

LightEntity::LightEntity(int lightType) : Model_obj(getLightModelPath(lightType),new RastShader(),false)
{
	this->lightType = lightType;
	intensity = 1.0;
	moveTo(0, 0, 0);
	scaleTo(1, 1, 1);
	this->__entityName = std::string("LIGHT");
}

void LightEntity::update(Camera* cam)
{
	globalPosition[0] = __pos[0];
	globalPosition[1] = __pos[1];
	globalPosition[2] = __pos[2];
	Model_obj::update(cam);
}

LightEntity::~LightEntity()
{

}

FrustumLight::FrustumLight(GLfloat fov, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane) : LightEntity(FRUSTUM_LIGHT)
{
	projectionMatrix = new Matrix4x4();
	projectionMatrix->perspective(fov, aspectRatio, nearPlane, farPlane);
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	projectionWidth = 100.0;
	projectionHeight = 100.0;
}


FrustumLight::~FrustumLight()
{
	delete projectionMatrix;
}

void FrustumLight::update(Camera* cam)
{
	LightEntity::update(cam);
}
