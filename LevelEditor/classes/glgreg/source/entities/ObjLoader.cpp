#include "entities/ObjLoader.h"
#include <stdio.h>
#include <iostream>
#include "data structures/MeshStructures.h"
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <lodepng.h>
#include <opencv2/opencv.hpp>

#define VERTEX_IDENTIFIER 0
#define FACE_IDENTIFIER 1
#define NORMAL_IDENTIFIER 2
#define TEXTURE_COORD_IDENTIFIER 3
#define USE_MATERIAL_IDENTIFIER 4
#define MATERIAL_LIBRARY 5
#define OBJ_LINE_FEED -1
#define OBJ_END_OF_FILE -2
#define NOT_DEFINED -3

#define BLEND_OPTION_U 0
#define BLEND_OPTION_V 1
#define BOOST_OPTION 2
#define MODIFY_TEXTURE_MAP_OPTION 3
#define ORIGIN_OPTION 4
#define SCALE_OPTION 5
#define TURBULENCE_OPTION 6
#define TEXTURE_RES_OPTION 7
#define CLAMP_OPTION 8
#define BUMP_MULTIPLIER_OPTION 9
#define IMFCHAN_OPTION 10

const char* OBJ_IDENTIFIER_V = "v";

#define NUM_OBJ_IDENTIFIERS 6
const GLchar* OBJ_IDENTIFIERS[] = {"v", "f", "vn", "vt",  "usemtl", "mtllib"};

#define NUM_OPTION_IDENTIFIERS 11
const GLchar* OPTION_IDENTIFIERS[] = { "-blendu", "-blendv", "-boost", "-mm",  "-o", "-s", "-t", "-texres", "-clamp", "-bm", "-imfchan" };

#define MATERIAL_NEW_MATERIAL 0
#define MATERIAL_KA 1
#define MATERIAL_KD 2
#define MATERIAL_KS 3
#define MATERIAL_TF 4
#define MATERIAL_ILLUM 5
#define MATERIAL_D_FACTOR 6
#define MATERIAL_NS 7
#define MATERIAL_NI 8
#define MATERIAL_SHARPNESS 9
#define MATERIAL_MAP_KA 10
#define MATERIAL_MAP_KD 11
#define MATERIAL_MAP_D 12
#define MATERIAL_BUMP_MAP 13

const unsigned int NUM_MATERIAL_ELEMENTS = 14;
const GLchar* MATERIALS_ELEMENTS[] = {"newmtl","Ka","Kd","Ks","Tf","illum","d","Ns","Ni","Sharpness", "map_Ka", "map_Kd", "map_d", "map_bump"};

using namespace std;

ObjLoader::ObjLoader(const char* filePath)
{
	__filePath = std::string(filePath);
	__basePath = std::string(filePath);
	int basePathSize = __basePath.find_last_of('/',__basePath.length());
	__basePath = __basePath.substr(0,basePathSize+1);

	loadObjFile();



#if SMOOTH_NORMALS
	smoothNormals();
#endif
}

ObjLoader::~ObjLoader()
{
	__filePath.erase();
	__basePath.erase();
	__materialPath.erase();

	while (__materials.size() > 0)
	{
		Material* m = __materials[__materials.size() - 1];
		delete m;
		__materials.pop_back();
	}

	__vertexList.clear();
	__normalList.clear();
	__texCoordList.clear();
	__faceList.clear();
}

void ObjLoader::loadObjFile()
{
	ifstream myFile;
	myFile.open(__filePath.c_str());
	string current_line;

	while(getline(myFile, current_line))
	{
		switch(getIdentifier(current_line))
		{
			case VERTEX_IDENTIFIER:
			{
				addVertexCoord(current_line);
				break;
			}
			case FACE_IDENTIFIER:
			{
				addFace(current_line);
				break;
			}
			case NORMAL_IDENTIFIER:
			{
				addNormalCoord(current_line);
				break;
			}
			case TEXTURE_COORD_IDENTIFIER:
			{
				addTextureCoord(current_line);
				break;
			}
			case USE_MATERIAL_IDENTIFIER:
			{
				setCurrentMaterial(current_line);
				break;
			}
			case MATERIAL_LIBRARY:
			{
				loadMaterials(current_line);
				break;
			}
			case NOT_DEFINED:
			{

				break;
			}
		}
	}
}

int ObjLoader::getIdentifier(std::string &line)
{
	if(line.size() == 0)
		return NOT_DEFINED;

	int i=0;
	while(i < line.size() && (line[i] == ' ' || line[i] == 9))
		i++;

	int j=line.size() -1;
	while(j < line.size() && line[j] == ' ')
		j = j-1;
	line = line.substr(i,j+1);

	std::string sub = line.substr(0,line.find(' ',1));
	line = line.substr(line.find(' ',1) + 1,line.size());
	for(i=0; i<NUM_OBJ_IDENTIFIERS; i++)
	{
		if(sub.compare(std::string(OBJ_IDENTIFIERS[i])) == 0)
			return i;
	}

	return NOT_DEFINED;
}

void ObjLoader::addVertexCoord(std::string &line)
{
	GLfloat v[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	v[0] = atof( s1.c_str() );
	v[1] = atof( s2.c_str() );
	v[2] = atof( s3.c_str() );

	Vertex* vertex = new Vertex();
	vertex->pos[0] = v[0];
	vertex->pos[1] = v[1];
	vertex->pos[2] = v[2];

	__vertexList.push_back(vertex);

	s1.clear();
	s2.clear();
	s3.clear();
}

void ObjLoader::addNormalCoord(std::string &line)
{
	GLfloat n[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	n[0] = atof( s1.c_str() );
	n[1] = atof( s2.c_str() );
	n[2] = atof( s3.c_str() );

	Vertex* normal = new Vertex();
	normal->pos[0] = n[0];
	normal->pos[1] = n[1];
	normal->pos[2] = n[2];

	__normalList.push_back(normal);

	s1.clear();
	s2.clear();
	s3.clear();
}

void ObjLoader::addTextureCoord(std::string &line)
{
	GLfloat t[2];
	
	std::size_t found1 = line.find(' ',1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = line.substr(found1,line.size() - found1 );

	t[0] = atof( s1.c_str() );
	t[1] = atof( s2.c_str() );

	TexCoord* texCoord = new TexCoord();
	texCoord->pos[0] = t[0];
	texCoord->pos[1] = t[1];

	__texCoordList.push_back(texCoord);

	s1.clear();
	s2.clear();
}

void ObjLoader::addFace(std::string &line)
{
	// account for 4 vertex faces..

	Face* f = new Face();

	//get the three vertices of the face
	std::size_t foundV1delim = line.find(' ',1);
	std::size_t foundV2delim = line.find(' ',foundV1delim + 1);
	std::size_t foundV3delim = line.find(' ',foundV2delim + 1);

	std::string v1,v2,v3;

	//4 vertices found
	if(foundV3delim != -1)
	{
		addFace(line.substr(0,foundV3delim));
		//v1 = ( line.substr(foundV1delim + 1, foundV2delim-foundV1delim - 1) );
		v1 = line.substr(0, foundV1delim );
		v2 = line.substr(foundV2delim + 1,foundV3delim - foundV2delim -1);
		v3 = line.substr(foundV3delim + 1,line.size() - foundV3delim -1);
	}
	else
	{
		v1 = line.substr(0, foundV1delim );
		v2 = ( line.substr(foundV1delim + 1, foundV2delim-foundV1delim - 1) );
		v3 = line.substr(foundV2delim + 1,line.size() - foundV2delim );
	}

	std::size_t foundElement1Delim = v1.find('/',1);
	std::size_t foundElement2Delim = v1.find('/',foundElement1Delim + 1);

	unsigned int v,t,n;
	
	parseFaceVertexIndices(v1,&v,&t,&n);
	if (v != -1)
	{
		f->vertex[0] = __vertexList[v];
		f->vertIndex[0] = v;
	}
	if (t != -1)
	{
		f->texCoord[0] = __texCoordList[t];
		f->texCoordIndex[0] = t;
	}
	if (n != -1)
	{
		f->normal[0] = __normalList[n];
		f->normalIndex[0] = n;
	}

	parseFaceVertexIndices(v2,&v,&t,&n);
	if (v != -1)
	{
		f->vertex[1] = __vertexList[v];
		f->vertIndex[1] = v;
	}
	if (t != -1)
	{
		f->texCoord[1] = __texCoordList[t];
		f->texCoordIndex[1] = t;
	}
	if (n != -1)
	{
		f->normal[1] = __normalList[n];
		f->normalIndex[1] = n;
	}

	parseFaceVertexIndices(v3,&v,&t,&n);
	if (v != -1)
	{
		f->vertex[2] = __vertexList[v];
		f->vertIndex[2] = v;
	}
	if (t != -1)
	{
		f->texCoord[2] = __texCoordList[t];
		f->texCoordIndex[2] = t;
	}
	if (n != -1)
	{
		f->normalIndex[2] = n;
		f->normal[2] = __normalList[n];
	}
	// if the normals are non existstant,
	if(f->normal[0] == NULL || f->normal[1] == NULL || f->normal[2] == NULL)
		computeNormals(f);

	f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0])/3.0;
	f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1])/3.0;
	f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2])/3.0;

	f->materialIndex = __currentMaterial;
	f->index = __faceList.size();

	__faceList.push_back(f);

	v1.clear(); 
	v2.clear(); 
	v3.clear();
}

void ObjLoader::computeNormals(Face* f)
{
	Vertex* norm1 = new Vertex();
	Vertex* norm2 = new Vertex();
	Vertex* norm3 = new Vertex();
	
	GLfloat v1[3];
	v1[0] = f->vertex[1]->pos[0] - f->vertex[0]->pos[0];
	v1[1] = f->vertex[1]->pos[1] - f->vertex[0]->pos[1];
	v1[2] = f->vertex[1]->pos[2] - f->vertex[0]->pos[2];

	GLfloat v2[3];
	v2[0] = f->vertex[2]->pos[0] - f->vertex[1]->pos[0];
	v2[1] = f->vertex[2]->pos[1] - f->vertex[1]->pos[1];
	v2[2] = f->vertex[2]->pos[2] - f->vertex[1]->pos[2];
	
	normalize(v1);
	normalize(v2);

	GLfloat n[3];
	n[0] = v1[1]*v2[2] - v2[1]*v1[2];
	n[1] = -v1[0]*v2[2] + v2[0]*v1[2];
	n[2] = v1[0]*v2[1] - v2[0]*v1[1];

	normalize(n);

	norm1->pos[0] = n[0];
	norm1->pos[1] = n[1];
	norm1->pos[2] = n[2];

	norm2->pos[0] = n[0];
	norm2->pos[1] = n[1];
	norm2->pos[2] = n[2];

	norm3->pos[0] = n[0];
	norm3->pos[1] = n[1];
	norm3->pos[2] = n[2];

	f->normal[0] = norm1;
	f->normal[1] = norm2;
	f->normal[2] = norm3;

}

void ObjLoader::parseFaceVertexIndices(std::string &elements, unsigned int* v, unsigned int* t, unsigned int* n)
{
	for (int i = 0; i < elements.size(); i++)
	{
		if (elements.at(i) == '-')
		{
			elements[i] = '0';
		}
	}
	std::size_t foundElement1Delim = elements.find('/',1);
	std::size_t foundElement2Delim = elements.find('/',foundElement1Delim + 1);

	std::string elements1;
	std::string elements2;
	std::string elements3;

	if(foundElement1Delim == -1)
	{
		*v = stoi(elements.c_str()) - 1;
		*t = -1;
		*n = -1;
	}
	else if(foundElement2Delim == -1)
	{
		elements1 = elements.substr(0, foundElement1Delim );
		elements2 = elements.substr(foundElement1Delim + 1,elements.size() - foundElement1Delim ); 

		*v = stoi(elements1.c_str()) - 1;
		*t = stoi(elements2.c_str()) - 1;
		*n = -1;
	}
	else
	{
		elements1 = elements.substr(0, foundElement1Delim );
		elements2 = ( elements.substr(foundElement1Delim + 1, foundElement2Delim-foundElement1Delim - 1) );
		elements3 = elements.substr(foundElement2Delim + 1,elements.size() - foundElement2Delim );

		*v = stoi(elements1.c_str()) - 1;
		*t = stoi(elements2.c_str()) - 1;
		*n = stoi(elements3.c_str()) - 1;
	}

	elements1.clear();
	elements2.clear();
	elements3.clear();
}

void ObjLoader::loadMaterials(std::string materialPath)
{
	ifstream materialFile;
	std::string path = __basePath + materialPath;
	materialFile.open(path.c_str());
	string current_line;

	while(getline(materialFile, current_line))
	{
		switch(getMaterialIdentifier(current_line))
		{
			case MATERIAL_NEW_MATERIAL:
			{
				addNewMaterial(current_line);
				break;
			}
			case MATERIAL_KA:
			{
				addNewMaterialKa(current_line);
				break;
			}
			case MATERIAL_KD:
			{
				addNewMaterialKd(current_line);
				break;
			}
			case MATERIAL_KS:
			{
				addNewMaterialKs(current_line);
				break;
			}
			case MATERIAL_TF :
			{
				addNewMaterialTf(current_line);
				break;
			}
			case MATERIAL_ILLUM :
			{
				addNewMaterialIllum(current_line);
				break;
			}
			case MATERIAL_D_FACTOR :
			{
				addNewMaterialDFactor(current_line);
				break;
			}
			case MATERIAL_NS :
			{
				addNewMaterialNs(current_line);
				break;
			}
			case MATERIAL_NI :
			{
				addNewMaterialNi(current_line);
				break;
			}
			case MATERIAL_SHARPNESS :
			{
				addNewMaterialSharpness(current_line);
				break;
			}
			case MATERIAL_MAP_KD :
			{
				addNewMaterialMapKd(current_line);
				break;
			}
			case MATERIAL_BUMP_MAP:
			{
				addOption(current_line);
				addNewBumbMapTexture(current_line);
			}

		}
	}

	materialFile.close();
}

void ObjLoader::addOption(std::string &line)
{
	int option = getOptionIdentifier(line);
	switch (option)
	{
		case BUMP_MULTIPLIER_OPTION:
		{
			addBumpMapMultiplier(line);
			break;
		}
	default:
		break;
	}
}

int ObjLoader::getMaterialIdentifier(std::string &line)
{
	if(line.size() == 0)
		return NOT_DEFINED;

	int i=0;
	while(i < line.size() && (line[i] == ' ' || line[i] == 9))
		i++;

	std::string sub = line.substr(i,line.find(' ',1)-i);
	line = line.substr(line.find(' ',1) + 1,line.size());
	for(i=0; i<NUM_MATERIAL_ELEMENTS; i++)
	{
		if(sub.compare(std::string(MATERIALS_ELEMENTS[i])) == 0)
			return i;
	}
	sub.clear();
	return NOT_DEFINED;
}

int ObjLoader::getOptionIdentifier(std::string &line)
{
	if (line.size() == 0)
		return NOT_DEFINED;

	int i = 0;
	while (i < line.size() && (line[i] == ' ' || line[i] == 9))
		i++;

	std::string sub = line.substr(i, line.find(' ', 1) - i);
	line = line.substr(line.find(' ', 1) + 1, line.size());

	for (i = 0; i<NUM_OPTION_IDENTIFIERS; i++)
	{
		if (sub.compare(std::string(OPTION_IDENTIFIERS[i])) == 0)
			return i;
	}
	sub.clear();
	return NOT_DEFINED;
}

void ObjLoader::addBumpMapMultiplier(std::string &line)
{
	std::size_t found1 = line.find(' ', 1);
	std::string s1 = line.substr(0, found1);
	(__materials[__materials.size() - 1])->bumpMapMult = atof(s1.c_str());
	line = line.substr(line.find(' ', 1) + 1, line.size());
	s1.clear();
}

void ObjLoader::addNewMaterial(std::string &line)
{
	Material* mat = new Material();
	mat->materialName = string(line.c_str());
	__materials.push_back(mat);
}
void ObjLoader::addNewMaterialKa(std::string &line)
{
	GLfloat ka[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	ka[0] = atof( s1.c_str() );
	ka[1] = atof( s2.c_str() );
	ka[2] = atof( s3.c_str() );

	(__materials[__materials.size() - 1])->setKa(ka);

	s1.clear();
	s2.clear();
	s3.clear();
}
void ObjLoader::addNewMaterialKd(std::string &line)
{
	GLfloat kd[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	kd[0] = atof( s1.c_str() );
	kd[1] = atof( s2.c_str() );
	kd[2] = atof( s3.c_str() );

	(__materials[__materials.size() - 1])->setKd(kd);

	s1.clear();
	s2.clear();
	s3.clear();
}
void ObjLoader::addNewMaterialKs(std::string &line)
{
	GLfloat ks[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	ks[0] = atof( s1.c_str() );
	ks[1] = atof( s2.c_str() );
	ks[2] = atof( s3.c_str() );

	(__materials[__materials.size() - 1])->setKs(ks);

	s1.clear();
	s2.clear();
	s3.clear();
}
void ObjLoader::addNewMaterialTf(std::string &line)
{
	GLfloat tf[3];
	
	std::size_t found1 = line.find(' ',1);
	std::size_t found2 = line.find(' ',found1 + 1);

	std::string s1 = line.substr(0, found1 );
	std::string s2 = ( line.substr(found1, found2-found1) );
	std::string s3 = line.substr(found2,line.size() - found2 );

	tf[0] = atof( s1.c_str() );
	tf[1] = atof( s2.c_str() );
	tf[2] = atof( s3.c_str() );

	(__materials[__materials.size() - 1])->setTf(tf);

	s1.clear();
	s2.clear();
	s3.clear();
}
void ObjLoader::addNewMaterialIllum(std::string &line)
{
	(__materials[__materials.size() - 1])->illum = atoi(line.c_str());
}
void ObjLoader::addNewMaterialDFactor(std::string &line)
{
	(__materials[__materials.size() - 1])->d = atof(line.c_str());
}
void ObjLoader::addNewMaterialNs(std::string &line)
{
	(__materials[__materials.size() - 1])->ns = atof(line.c_str());
}
void ObjLoader::addNewMaterialNi(std::string &line)
{
	(__materials[__materials.size() - 1])->ni = atof(line.c_str());
}
void ObjLoader::addNewMaterialSharpness(std::string &line)
{
	(__materials[__materials.size() - 1])->sharpness = atof(line.c_str());
}
void ObjLoader::addNewMaterialMapKd(std::string &line)
{
	(__materials[__materials.size() - 1])->texture = loadTexture(line);
	(__materials[__materials.size() - 1])->kdTextureName = line;
	(__materials[__materials.size() - 1])->isTexBound = 1;
}
void ObjLoader::addNewBumbMapTexture(std::string &line)
{
	(__materials[__materials.size() - 1])->bumpMapTexture = loadTexture(line);
	(__materials[__materials.size() - 1])->bumpMapName = line;
	(__materials[__materials.size() - 1])->isBumpTexBound = 1;
}

Texture_obj<GLfloat>* ObjLoader::loadTexture(std::string textureName)
{
	GLfloat* texData;
	int width;
	int height;
	int channelSize;
	std::string filePath = __basePath;
	filePath += textureName;
	
	for (int i = 0; i < __materials.size(); i++)
	{
		if (__materials[i]->kdTextureName.compare(textureName) == 0)
		{
			return __materials[i]->texture;
		}
	}

	/*
	std::vector<unsigned char> image;
	unsigned int w;
	unsigned int h;
	lodepng::decode(image, w, h, filePath.c_str());
	

	width = w;
	height = h;
	channelSize = 4;
	texData = new GLubyte[image.size()];
	for (int i = 0; i < image.size(); i++)
	{
		texData[i] = (GLubyte)image[i];
	}
	if (channelSize == 3)
	{
		GLubyte* texDataTest = new GLubyte[4 * width * height];
		int i = 0;
		int j = 0;
		for (int n = 0; n < height; n++)
		{
			for (int m = 0; m < width; m++)
			{
				texDataTest[j] = texData[i];
				texDataTest[j + 1] = texData[i + 1];
				texDataTest[j + 2] = texData[i + 2];
				texDataTest[j + 3] = 255;
				i = i + 3;
				j = j + 4;
			}
		}
		delete texData;
		texData = texDataTest;
		channelSize = 4;
	}
	*/

	cv::Mat im = cv::imread(filePath, cv::IMREAD_UNCHANGED);
	//cv::resize(im, im, cv::Size(512, 512), 0.0, 0.0, CV_INTER_CUBIC);

	width = im.rows;
	height = im.cols;
	int cs = im.channels();
	channelSize = 4;
	texData = new GLfloat[width*height*channelSize];
	int texDataIndex = 0;
	int matTexDataIndex = 0;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			for (int i = 0; i < 3; i++)
			{
				texData[texDataIndex + i] = 0.0;
			}
			texData[texDataIndex + 3] = 1.0;

			for (int i = 0; i < cs; i++)
			{
				unsigned char sample = im.data[matTexDataIndex + i];
				texData[texDataIndex + i] = ((GLfloat)sample) / 255.0;
			}
			if (cs == 1)
			{
				texData[texDataIndex + 1] = texData[texDataIndex];
				texData[texDataIndex + 2] = texData[texDataIndex];
			}
			else
			{
				float temp = texData[texDataIndex];
				texData[texDataIndex] = texData[texDataIndex + 2];
				texData[texDataIndex + 2] = temp;
			}
			texDataIndex = texDataIndex + 4;
			matTexDataIndex = matTexDataIndex + cs;
		}
	}

	Texture_obj<GLfloat>* tex = NULL;
	if (texData != NULL)
	{
		tex = new Texture_obj<GLfloat>();
		tex->width = width;
		tex->height = height;
		tex->channelSize = channelSize;
		tex->buf = texData;
	}

	return tex;
}

void ObjLoader::setCurrentMaterial(std::string &current_line)
{
	for(int i=0; i< __materials.size(); i++)
	{
		if((__materials[i]->materialName).compare(current_line) == 0)
		{
			__currentMaterial = i;
			return;
		}
	}
}

void ObjLoader::resizeTexturesToMatch()
{
	unsigned int largestWidth = 0;
	unsigned int largestHeight = 0;
	for(int i=0; i<__materials.size(); i++)
	{
		if(__materials[i]->texture != NULL)
		{
			if(__materials[i]->texture->width > largestWidth)
				largestWidth = __materials[i]->texture->width;

			if(__materials[i]->texture->height > largestHeight)
				largestHeight = __materials[i]->texture->height;
		}
	}

	for(int i=0; i<__faceList.size(); i++)
	{
		Face* currentFace = __faceList[i];

		if(__materials[currentFace->materialIndex]->texture == NULL)
		{
			__materials[currentFace->materialIndex]->texture = new Texture_obj<GLfloat>();
			__materials[currentFace->materialIndex]->texture->width = largestWidth;
			__materials[currentFace->materialIndex]->texture->height = largestHeight;
			__materials[currentFace->materialIndex]->texture->channelSize = 3;
			
			__materials[currentFace->materialIndex]->texture->buf = (GLfloat*)malloc(sizeof(GLfloat)*3*largestWidth*largestHeight);

			for(int j=0; j<3*largestWidth*largestHeight; j = j+3)
			{
				__materials[currentFace->materialIndex]->texture->buf[j] = floor(__materials[currentFace->materialIndex]->kd[0]*256 + 0.5);
				__materials[currentFace->materialIndex]->texture->buf[j+1] = floor(__materials[currentFace->materialIndex]->kd[1]*256 + 0.5);
				__materials[currentFace->materialIndex]->texture->buf[j+2] = floor(__materials[currentFace->materialIndex]->kd[2]*256 + 0.5);
			}
		}
	}

	return;
}

std::vector<Face*>& ObjLoader::faceList()
{
	return __faceList;
}

std::vector<Vertex*>& ObjLoader::vertexList()
{
	return __vertexList;
}

std::vector<Vertex*>& ObjLoader::normalList()
{
	return __normalList;
}

std::vector<TexCoord*>& ObjLoader::texCoordList()
{
	return __texCoordList;
}

std::vector<Material*>& ObjLoader::materials()
{
	return __materials;
}

void ObjLoader::addFace(Face* f)
{
	__faceList.push_back(f);
}
void ObjLoader::removeFace(int index)
{
	Face* f = __faceList[index];
	this->__faceList.erase(__faceList.begin() + index);
	//delete f;
}

Material::Material()
{
	materialIndex = 0;
	ka[0] = 0.0;
	ka[1] = 0.0;
	ka[2] = 0.0;

	kd[0] = 0.0;
	kd[1] = 0.0;
	kd[2] = 0.0;

	ks[0] = 0.0;
	ks[1] = 0.0;
	ks[2] = 0.0;

	tf[0] = 0.0;
	tf[1] = 0.0;
	tf[2] = 0.0;

	ni = 0.0;
	ns = 0.0;
	d = 0.0;
	sharpness = 0.0;
	illum = 0;
	bumpMapMult = 0.0;
	isTexBound = 0;

	texture = NULL;
	bumpMapTexture = NULL;
}

Material::~Material()
{
	materialName.erase();
	kdTextureName.erase();
	bumpMapName.erase();
	
	kdTextureName.erase();
	//delete texture;

	//bumpMapName.erase();
	//delete bumpMapTexture;
}

template <class T>
Texture_obj<T>::Texture_obj()
{
	buf = (T*)NULL;
	width = 0;
	height = 0;
	channelSize = 0;
	bufferIndex = -1;
	activeTexture = -1;
}

template <class T>
Texture_obj<T>::~Texture_obj()
{
	delete buf;
}

void Material::setKa(GLfloat* val)
{
	ka[0] = val[0];
	ka[1] = val[1];
	ka[2] = val[2];
}

void Material::setKd(GLfloat* val)
{
	kd[0] = val[0];
	kd[1] = val[1];
	kd[2] = val[2];
}


void Material::setKs(GLfloat* val)
{
	ks[0] = val[0];
	ks[1] = val[1];
	ks[2] = val[2];
}


void Material::setTf(GLfloat* val)
{
	tf[0] = val[0];
	tf[1] = val[1];
	tf[2] = val[2];
}

template <class T>
void Texture_obj<T>::resize(int w,int h)
{

}

void ObjLoader::smoothNormals()
{
	std::vector<Vertex*> smoothNormals;
	for (int i = 0; i < __vertexList.size(); i++)
	{
		Vertex* n = new Vertex();
		n->pos[0] = 0.0;
		n->pos[1] = 0.0;
		n->pos[2] = 0.0;
		smoothNormals.push_back(n);
	}

	for (int i = 0; i < __faceList.size(); i++)
	{
		Face* f = __faceList[i];
		for (int j = 0; j < 3; j++)
		{
			int nIndex = f->vertIndex[j];
			Vertex* faceNormal = f->normal[j];
			Vertex* nInterp = smoothNormals[nIndex];
			nInterp->pos[0] += faceNormal->pos[0];
			nInterp->pos[1] += faceNormal->pos[1];
			nInterp->pos[2] += faceNormal->pos[2];
			normalize(nInterp->pos);
		}
	}

	for (int i = 0; i < __faceList.size(); i++)
	{
		Face* f = __faceList[i];
		for (int j = 0; j < 3; j++)
		{
			int nIndex = f->vertIndex[j];
			Vertex* sn = smoothNormals[nIndex];
			f->normal[j]->pos[0] = sn->pos[0];
			f->normal[j]->pos[1] = sn->pos[1];
			f->normal[j]->pos[2] = sn->pos[2];
		}
	}
	smoothNormals.clear();
}
#define GAUSSIAN_FILTER_SIZE 25
void downsample(float* image, int width, int height, int downsamplingRate)
{
	/*
	float gaussianRadius = 0.1325 / (1.0 / (float)downsamplingRate);

	float filter[GAUSSIAN_FILTER_SIZE];
	int filterMid = GAUSSIAN_FILTER_SIZE / 2;
	float filterSum = 0.0;
	for (int i = 0; i < GAUSSIAN_FILTER_SIZE; i++)
	{
		float r = abs(float(i) - float(filterMid));
		filter[i] = exp(-(r*r) / (2 * gaussianRadius*gaussianRadius));
		filterSum += filter[i];
	}
	for (int i = 0; i < GAUSSIAN_FILTER_SIZE; i++)
	{
		filter[i] = filter[i] / filterSum;
	}

	int filteredWidth = (width + GAUSSIAN_FILTER_SIZE - 1);
	int filteredHeight = (height + GAUSSIAN_FILTER_SIZE - 1);
	float* imageFiltered = new float[filteredWidth*filteredHeight];
	for (int i = 0; i < filterMid; i++)
	{
		for (int j = 0; j < filterMid; j++)
		{
			int index = j*filteredWidth + i;
			imageFiltered[]
		}
	}
	*/
}

template class Texture_obj<GLubyte>;
template class Texture_obj<GLfloat>;