#pragma once

#include "..\entities\Entity.h"
#include <fstream>
#include "../shading utils/ShaderUtils.h"
#include "../shader/OutlineShader.h"
#include "../Controller.h"
#include "../shader/PointCloudShader.h"
#include "../entities/Model.h"

class PointCloud
{
public:
	PointCloud();
	PointCloud(float* pos, float* norm, float* col);

	float position[3];
	float normal[3];
	float color[3];
	
	float aPhi, bPhi, cPhi, dPhi;
	float aTheta, bTheta, cTheta, dTheta;
	float aRad, bRad, cRad, dRad;
};

class PointCloudEntity : public Model_obj
{
public:
	PointCloudEntity(const char* filePath, Shader* shader);
	virtual ~PointCloudEntity();

	virtual void render();
	virtual void update(Camera* cam);

	//char* filePath();
	//Shader* shader();
	//Bounds* bounds;
	//OutlineShader* outlineShader;

private:
	void loadPointCloud(const char* filePath);
	std::vector<PointCloud*> pointCloud;

protected:
	virtual void addVertices();
	virtual void addNormals();
	virtual void addColors();
	virtual void computeBounds();
	virtual void addQuadraticCoefficients();
};