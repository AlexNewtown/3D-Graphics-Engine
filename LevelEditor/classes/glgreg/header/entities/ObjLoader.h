#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <iostream>
#include "data structures/MeshStructures.h"
#include <stdio.h>
#include <vector>
#ifndef _WEB
#endif
#include "data structures/Point.h"

#define SMOOTH_NORMALS 1

template <class T>
struct Texture_obj
{
	Texture_obj();
	virtual ~Texture_obj();
	void resize(int w,int h);
	T* buf;
	int width;
	int height;
	int channelSize;
	unsigned int bufferIndex;
	unsigned int activeTexture;
};

void downsample(float* image, int width, int height, int downsamplingRate);


struct Material
{
	Material();
	~Material();

	std::string materialName;
	int materialIndex;
	
	GLfloat ka[3];
	void setKa(GLfloat* val);

	GLfloat kd[3];
	void setKd(GLfloat* val);

	GLfloat ks[3];
	void setKs(GLfloat* val);

	GLfloat tf[3];
	void setTf(GLfloat* val);

	GLfloat ni;
	GLfloat ns;
	GLfloat d;
	GLfloat sharpness;
	int illum;
	GLfloat bumpMapMult;

	std::string kdTextureName;
	Texture_obj<GLubyte>* texture;

	std::string bumpMapName;
	Texture_obj<GLubyte>* bumpMapTexture;
};

struct Material_std
{
	GLfloat ka[3];
	GLint materialIndex;

	GLfloat kd[3];
	GLfloat ni;

	GLfloat ks[3];
	GLfloat ns;

	GLfloat tf[3];
	GLfloat d;

	GLfloat sharpness;
	GLint illum;

	GLint padding1;
	GLint padding2;
};

/*
class ObjLoader
loads a '.obj' from the filePath specified.
*/
class ObjLoader
{
public:
	ObjLoader(const char* filePath);
	~ObjLoader();

	/* GETTERS AND SETTERS */
	std::vector<Face*>& faceList();
	std::vector<Vertex*>& vertexList();
	std::vector<Vertex*>& normalList();
	std::vector<TexCoord*>& texCoordList();
	std::vector<Material*>& materials();

	/*
	
	*/
	void addFace(Face* f);
	void removeFace(int index);

private:
	std::string __filePath;
	std::string __basePath;

	std::string __materialPath;
	std::vector<Material*> __materials;
	int __currentMaterial;

	void loadObjFile();
	void loadMaterials(std::string materialPath);
	void initLists();

	std::vector<Vertex*> __vertexList;
	std::vector<Vertex*> __normalList;
	std::vector<TexCoord*> __texCoordList;
	std::vector<Face*> __faceList;

	int getIdentifier(std::string &line);
	void addOption(std::string &line);

	void addVertexCoord(std::string &line);
	void addNormalCoord(std::string &line);
	void addTextureCoord(std::string &line);
	void addFace(std::string &line);
	void parseFaceVertexIndices(std::string &elements, unsigned int* v, unsigned int* t, unsigned int* n);
	void computeNormals(Face* f);

	int getMaterialIdentifier(std::string &line);
	int getOptionIdentifier(std::string &line);
	void addNewMaterial(std::string &line);
	void addNewMaterialKa(std::string &line);
	void addNewMaterialKd(std::string &line);
	void addNewMaterialKs(std::string &line);
	void addNewMaterialTf(std::string &line);
	void addNewMaterialIllum(std::string &line);
	void addNewMaterialDFactor(std::string &line);
	void addNewMaterialNs(std::string &line);
	void addNewMaterialNi(std::string &line);
	void addNewMaterialSharpness(std::string &line);
	void addNewMaterialMapKd(std::string &line);
	void setCurrentMaterial(std::string &current_line);
	void addNewBumbMapTexture(std::string &line);

	void addBumpMapMultiplier(std::string &line);
	void smoothNormals();

	Texture_obj<GLubyte>* loadTexture(std::string textureName);
	void resizeTexturesToMatch();
};

#endif