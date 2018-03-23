#include "../../header/entities/PointCloudEntity.h"

PointCloud::PointCloud()
{
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;

	normal[0] = 0.0;
	normal[1] = 0.0;
	normal[2] = 0.0;

	color[0] = 0.0;
	color[1] = 0.0;
	color[2] = 0.0;
}

PointCloud::PointCloud(float* pos, float* norm, float* col)
{
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];

	normal[0] = norm[0];
	normal[1] = norm[1];
	normal[2] = norm[2];

	color[0] = col[0];
	color[1] = col[1];
	color[2] = col[2];

	aPhi = 0.0;
	bPhi = 0.0;
	cPhi = 0.0;
	dPhi = 0.0;

	aTheta = 0.0;
	bTheta = 0.0;
	cTheta = 0.0;
	dTheta = 0.0;

	aRad = 0.0;
	bRad = 0.0;
	cRad = 0.0;
	dRad = 0.0;
}

PointCloudEntity::PointCloudEntity(const char* filePath, Shader* shader) : Model_obj()
{
	this->__shader = shader;
	this->__filePath = (char*)filePath;
	__objLoader = nullptr;
	__meshTree = nullptr;
	__treeTex = nullptr;

	loadPointCloud(filePath);

	std::string stringName = std::string(filePath);
	size_t endSub = stringName.find_last_of(".");
	size_t startSub = stringName.find_last_of("/");
	__entityName = stringName.substr(startSub + 1, endSub - startSub - 1);

	this->bounds = new Bounds(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	computeBounds();
	outlineShader = nullptr;
	
	addVertices();
	addNormals();
	addColors();
	addMatrixUniforms();
	addQuadraticCoefficients();

	__pos = new GLfloat[3];
	__pos[0] = 0.0;
	__pos[1] = 0.0;
	__pos[2] = 0.0;

#ifdef _EDITOR
	outlineShader = new OutlineShader(this->bounds);
	__shader->addUniform(&__shader->shaderType, "shaderType", UNIFORM_INT_1, true);
#endif
}

PointCloudEntity::~PointCloudEntity()
{
	pointCloud.clear();
	delete __shader;
	delete bounds;
	delete outlineShader;
	delete[] __pos;
}

void PointCloudEntity::loadPointCloud(const char* filePath)
{
	std::fstream file;
	file.open(filePath, std::fstream::in | std::fstream::binary);
	if (!file)
		return;

	int numPoints;
	file.read((char*)&numPoints, 4);
	for (int i = 0; i < numPoints; i++)
	{
		PointCloud* pc = new PointCloud();
		
		//position
		file.read((char*)&(pc->position[0]), 4);
		file.read((char*)&(pc->position[1]), 4);
		file.read((char*)&(pc->position[2]), 4);

		//normal
		file.read((char*)&(pc->normal[0]), 4);
		file.read((char*)&(pc->normal[1]), 4);
		file.read((char*)&(pc->normal[2]), 4);

		//color
		file.read((char*)&(pc->color[0]), 4);
		file.read((char*)&(pc->color[1]), 4);
		file.read((char*)&(pc->color[2]), 4);


		file.read((char*)&(pc->aPhi), 4);
		file.read((char*)&(pc->bPhi), 4);
		file.read((char*)&(pc->cPhi), 4);
		file.read((char*)&(pc->dPhi), 4);

		file.read((char*)&(pc->aTheta), 4);
		file.read((char*)&(pc->bTheta), 4);
		file.read((char*)&(pc->cTheta), 4);
		file.read((char*)&(pc->dTheta), 4);

		file.read((char*)&(pc->aRad), 4);
		file.read((char*)&(pc->bRad), 4);
		file.read((char*)&(pc->cRad), 4);
		file.read((char*)&(pc->dRad), 4);

		pointCloud.push_back(pc);
	}
}

void PointCloudEntity::render()
{
	((PointCloudShader*)__shader)->render();
}
void PointCloudEntity::update(Camera* cam)
{
	Entity::update(cam);
}

void PointCloudEntity::addVertices()
{
	if (pointCloud.size() <= 0)
		return;

	int numVertices = pointCloud.size();

	GLfloat *vertexData = new GLfloat[numVertices*4];
	int vertexIndex = 0;
	PointCloud* pc;
	for (int i = 0; i < numVertices; i++)
	{
		pc = pointCloud[i];
		vertexData[i * 4] = pc->position[0];
		vertexData[i * 4 + 1] = pc->position[1];
		vertexData[i * 4 + 2] = pc->position[2];
		vertexData[i * 4 + 3] = 1.0;
	}

	__shader->addAttributeBuffer(vertexData, numVertices*4, sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
	delete vertexData;
}
void PointCloudEntity::addNormals()
{
	if (pointCloud.size() <= 0)
		return;

	int numNormals = pointCloud.size();

	GLfloat *normalData = new GLfloat[numNormals * 4];
	PointCloud* pc;
	for (int i = 0; i < numNormals; i++)
	{
		pc = pointCloud[i];
		normalData[i * 4] = pc->normal[0];
		normalData[i * 4 + 1] = pc->normal[1];
		normalData[i * 4 + 2] = pc->normal[2];
		normalData[i * 4 + 3] = 0.0;
	}

	__shader->addAttributeBuffer(normalData, numNormals * 4, sizeof(GLfloat), GL_FLOAT, "normal", 1, 4);
	delete normalData;
}
void PointCloudEntity::addColors()
{
	if (pointCloud.size() <= 0)
		return;

	int numColors = pointCloud.size();

	GLfloat *colorData = new GLfloat[numColors * 4];
	PointCloud* pc;
	for (int i = 0; i < numColors; i++)
	{
		pc = pointCloud[i];
		colorData[i * 4] = pc->color[0];
		colorData[i * 4 + 1] = pc->color[1];
		colorData[i * 4 + 2] = pc->color[2];
		colorData[i * 4 + 3] = 1.0;
	}

	__shader->addAttributeBuffer(colorData, numColors * 4, sizeof(GLfloat), GL_FLOAT, "color", 2, 4);
	delete colorData;
}


void PointCloudEntity::computeBounds()
{
	if (pointCloud.size() <= 0)
		return;

	bounds->low->x = pointCloud[0]->position[0];
	bounds->low->y = pointCloud[0]->position[1];
	bounds->low->z = pointCloud[0]->position[2];
	bounds->high->x = pointCloud[0]->position[0];
	bounds->high->y = pointCloud[0]->position[1];
	bounds->high->z = pointCloud[0]->position[2];

	for (int i = 0; i < pointCloud.size(); i++)
	{
		PointCloud* pc = pointCloud[i];

		bounds->low->x = minimum(bounds->low->x, pc->position[0]);
		bounds->low->y = minimum(bounds->low->y, pc->position[1]);
		bounds->low->z = minimum(bounds->low->z, pc->position[2]);

		bounds->high->x = maximum(bounds->high->x, pc->position[0]);
		bounds->high->y = maximum(bounds->high->y, pc->position[1]);
		bounds->high->z = maximum(bounds->high->z, pc->position[2]);
	}
}

void PointCloudEntity::addQuadraticCoefficients()
{
	if (pointCloud.size() <= 0)
		return;

	int numPoints = pointCloud.size();

	GLfloat *coefficientPhiData = new GLfloat[numPoints * 4];
	PointCloud* pc;
	for (int i = 0; i < numPoints; i++)
	{
		pc = pointCloud[i];
		coefficientPhiData[i * 4] = pc->aPhi;
		coefficientPhiData[i * 4 + 1] = pc->bPhi;
		coefficientPhiData[i * 4 + 2] = pc->cPhi;
		coefficientPhiData[i * 4 + 3] = pc->dPhi;
	}
	__shader->addAttributeBuffer(coefficientPhiData, numPoints * 4, sizeof(GLfloat), GL_FLOAT, "quadraticPhiCoef", 3, 4);
	delete coefficientPhiData;


	GLfloat *coefficientThetaData = new GLfloat[numPoints * 4];
	for (int i = 0; i < numPoints; i++)
	{
		pc = pointCloud[i];
		coefficientThetaData[i * 4] = pc->aTheta;
		coefficientThetaData[i * 4 + 1] = pc->bTheta;
		coefficientThetaData[i * 4 + 2] = pc->cTheta;
		coefficientThetaData[i * 4 + 3] = pc->dTheta;
	}
	__shader->addAttributeBuffer(coefficientThetaData, numPoints * 4, sizeof(GLfloat), GL_FLOAT, "quadraticThetaCoef", 4, 4);
	delete coefficientThetaData;

	GLfloat *coefficientRadData = new GLfloat[numPoints * 4];
	for (int i = 0; i < numPoints; i++)
	{
		pc = pointCloud[i];
		coefficientRadData[i * 4] = pc->aRad;
		coefficientRadData[i * 4 + 1] = pc->bRad;
		coefficientRadData[i * 4 + 2] = pc->cRad;
		coefficientRadData[i * 4 + 3] = pc->dRad;
	}
	__shader->addAttributeBuffer(coefficientRadData, numPoints * 4, sizeof(GLfloat), GL_FLOAT, "quadraticRadCoef", 5, 4);
	delete coefficientRadData;
}