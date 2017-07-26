#ifndef MODEL_H
#define MODEL_H

#include "shading utils/ShaderUtils.h"
#include "entities/ObjLoader.h"
#include "data structures/MeshHierarchyKd.h"
#include "entities/Entity.h"
#include "Camera.h"
#include "data structures/AABBTree.h"
#include "shader\OutlineShader.h"
#include "shader\RastShader.h"
#include "shader\AdsShader.h"
#include "shader\GlobalIlluminationJacobiShader.h"
#include "shader\RadianceCacheUniformGridShader.h"
#include "shader\MonteCarloBRDFShader.h"
#include "shader\DepthShader.h"
#include "shader\ShadowMapShader.h"
#include "shader\SHBRDFShader.h"
#include "shader\WaveletEnvironmentMapShader.h"
#include "data structures/Complex.h"
#include "data structures/SphericalHarmonics.h"
#include "shading utils/ProgressivePhotonMap.h"
#include "data structures/SphericalHarmonics.h"
#include "data structures/Complex.h"
#include "shading utils/SVDBrdfLoader.h"
#include "data structures/SphericalHarmonics.h"
#include "data structures/Image.h"
#include "shader\PhotonMapProjectionTextureShader.h"
#include "shader\NormalDepthProjectionTextureShader.h"

/* WHAT THIS CLASS NEEDS TO DO 

	load the object and initialize the shader..

	shader needs : 
		Attributes
			- vertices. 3 per face: vec4
			- normals. 3 per face: vec4
			- texture Coords. 2 per face: vec2
			- material Index: a hash lookup that returns the Material_obj.

		Uniforms:
			- the number of indices ( 4 bytes ) in the Material_obj
			- 1 texture that contains the list of all the Material_obj


		Textures:
			kd texture, 1 for each.  Our engine does not support more than
			the max textures per model.
	
	the class needs functions:
		render(), which invokes shader->render();



*/

class Model_obj;

struct Material_tex_int
{
	GLint materialIndex;
	GLint illum;
	GLint isTexBound;
	GLint isBumpTexBound;
};

struct Material_tex_float
{
	GLfloat ka[3];
	GLfloat ni;
	GLfloat kd[3];
	GLfloat ns;
	GLfloat ks[3];
	GLfloat d;
	GLfloat tf[3];
	GLfloat sharpness;
};

struct FaceTexture
{
	GLfloat vertex[3][4];
	GLfloat normal[3][4];
	GLfloat texCoord[3][2];
	GLint materialIndex;
	GLint iPad;
};

struct Light
{
	Light();
	void normalize();
	GLfloat lightPosition[4];
	GLfloat lightDirection[4];
	GLfloat intensity[4];
};

struct FaceLight
{
	FaceLight();
	void normalize();
	GLfloat vertex1[4];
	GLfloat vertex2[4];
	GLfloat vertex3[4];

	GLfloat lightDirection[4];
	GLfloat lightDiffuseColor[4];
	GLfloat lightSpecularColor[4];

	float radiance;
	float pad1;
	float pad2;
	float pad3;
};

class Model_obj;

class Model_obj : public Entity,public RigidBody
{
public:
	Model_obj();
	Model_obj(const char* filePath, Shader* s, bool anchored);
	virtual ~Model_obj();
	void addLight(std::vector<Light*> lights);
	void addLight(GLfloat* lightPos);
	void addFaceLight(FaceLight* faceLight);
	void addBoundaryTextures(float* boundaryTreeTexFloat, int boundaryTreeTexFloatSize, int* boundaryTreeTexInt, int boundaryTreeTexIntSize, int numBoundaries);
	Shader* shader();
	virtual void render();
	virtual void update(Camera* cam);
	char* filePath();
	unsigned int shaderType();
	void setShaderType(unsigned int st);
	std::vector<Face*> faceList();
	std::vector<Vertex*> vertexList();
	std::vector<Vertex*> normalList();
	std::vector<TexCoord*> texCoordList();
	void addFace(Face* f);
	void removeFace(int index);
	void addIrradianceVector();
	void addIrradianceSH();
	void addVertexIrradianceSH();
	void addSampledBrdfSH(int brdfIndex);
	void addSampledBrdf(int brdfIndex);
	void addEnvironmentMap(std::vector<EnvironmentMap*>& environmentMap);
	void addFaceListTexture();
	void copyParameters(Model_obj* m);
	ObjLoader* objloader();
	void subdivideFaces(float maxArea);

	/*BRDF variables*/
	int brdfType;
	float indexOfRefraction;
	float phongN;
	float microfacetM;
	float minnaertK;
	float absorption[3];
	float reducedScattering[3];

	float diffuseProbability;
	float specularProbability;

	std::vector<std::vector<float>> irradianceVector;
	std::vector<HitPointFace*> photonHits;
	std::vector<VertexRadiance*> vertexRadiance;

	Bounds* bounds;
	OutlineShader* outlineShader;

protected:
	const char* __filePath;
	Shader* __shader;
	ObjLoader* __objLoader;
	MeshHierarchyKd* __meshTree;
	KdTreeTexture* __treeTex;
	bool __anchored;
	virtual void computeBounds();

	virtual void addVertices();
	virtual void addNormals();
	virtual void addTextureCoord();
	virtual void addMaterialIndex();
	virtual void addMaterialBuffer();
	virtual void addMaterialTextureArray();
	virtual void addMaterialBumpMapTextureArray();
	virtual void addTreeTextureBuffers();
	virtual void addBasisXZ();
	virtual void addFaceIndices();
	virtual void addVerticesIndex();
	virtual void addMaterialUniforms();
	virtual void addMatrixUniforms();
	virtual void addVerticesPointCloud();

	void cloneMaterialTexInt(Material_tex_int *mStd, Material* m);
	void cloneMaterialTexFloat(Material_tex_float *mStd, Material* m);
	FaceTexture* createFaceTexture();

	Array* createMaterialTexFloatArray(std::vector<Material*> materialList);
	Array* createMaterialTexIntArray(std::vector<Material*> materialList);

	void addBoundingBox();
	

};

#endif