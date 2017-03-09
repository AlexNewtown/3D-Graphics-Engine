#include "shader\RadianceMapShader.h"

RadianceMapShader::RadianceMapShader(std::vector<PhotonMap*> photonMap)
{
	shaderType = SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID;

	addMain(SHADER_SOURCE_RADIANCE_MAP_VERT, __shaderLinesVert);
	addMain(SHADER_SOURCE_RADIANCE_MAP_GEOM, __shaderLinesGeom);
	addMain(SHADER_SOURCE_RADIANCE_MAP_FRAG, __shaderLinesFrag);

	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "illum";
	addAttributes(attributes, 2);

	linkProgram();
	setPrimitiveType(GL_POINTS);
	glPointSize(2.5);

	GLfloat* position = new GLfloat[photonMap.size() * 4];
	GLfloat* illum = new GLfloat[photonMap.size()];

	int positionIndex = 0;
	for (int i = 0; i < photonMap.size(); i++)
	{
		position[positionIndex] = photonMap[i]->pos[0];
		position[positionIndex + 1] = photonMap[i]->pos[1];
		position[positionIndex + 2] = photonMap[i]->pos[2];
		position[positionIndex + 3] = 1.0;
		positionIndex = positionIndex + 4;

		illum[i] = 1.0;

	}

	this->addAttributeBuffer(position, 4 * photonMap.size(), sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
	this->addAttributeBuffer(illum, photonMap.size(), sizeof(GLfloat), GL_FLOAT, "illum", 1, 1);
	addUniform((void*)getControllerInstance()->camera()->projectionMatrix()->data(), "projectionMatrix", UNIFORM_MAT_4, false);
	addUniform((void*)getControllerInstance()->camera()->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, true);
}

RadianceMapShader::RadianceMapShader(std::vector<RadianceCache*> radianceCache):CollectiveShader()
{
	shaderType = SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID;

	addMain(SHADER_SOURCE_RADIANCE_MAP_VERT, __shaderLinesVert);
	addMain(SHADER_SOURCE_RADIANCE_MAP_GEOM, __shaderLinesGeom);
	addMain(SHADER_SOURCE_RADIANCE_MAP_FRAG, __shaderLinesFrag);
	
	constructShader();

	GLchar** attributes = new GLchar*[5];
	attributes[0] = "position";
	attributes[1] = "illum";
	addAttributes(attributes, 2);

	linkProgram();
	setPrimitiveType(GL_POINTS);
	glPointSize(1.0);

	GLfloat* position = new GLfloat[radianceCache.size()*4];
	GLfloat* illum = new GLfloat[radianceCache.size()];

	int positionIndex = 0;
	for (int i = 0; i < radianceCache.size(); i++)
	{
		position[positionIndex] = radianceCache[i]->pos[0];
		position[positionIndex+1] = radianceCache[i]->pos[1];
		position[positionIndex+2] = radianceCache[i]->pos[2];
		position[positionIndex + 3] = 1.0;
		positionIndex = positionIndex + 4;
		 
		illum[i] = radianceCache[i]->radiance;
		
	}

	this->addAttributeBuffer(position, 4*radianceCache.size(), sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
	this->addAttributeBuffer(illum, radianceCache.size(), sizeof(GLfloat), GL_FLOAT, "illum", 1, 1);
	addUniform((void*)getControllerInstance()->camera()->projectionMatrix()->data(), "projectionMatrix", UNIFORM_MAT_4, false);
	addUniform((void*)getControllerInstance()->camera()->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, true);
}

RadianceMapShader::~RadianceMapShader()
{

}