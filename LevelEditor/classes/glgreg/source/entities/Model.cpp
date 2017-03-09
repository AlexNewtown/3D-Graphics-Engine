#include "entities/Model.h"
#include "math/Matrix.h"
#include "Controller.h"


Model_obj::Model_obj() : Entity(), RigidBody(0.0)
{

}

Model_obj::Model_obj(const char* filePath, Shader* s, bool anchored): Entity(),RigidBody(10.0)
{
	__shader = s;
	__filePath = filePath;
	__anchored = anchored;
	__objLoader = new ObjLoader(filePath);
	indexOfRefraction = 1.44;
	brdfType = 0;
	
	phongN = 0.3;
	microfacetM = 0.1;
	minnaertK = 1.0;

	diffuseProbability = 1.0;
	specularProbability = 0.0;

	absorption[0] = 0.15;
	absorption[1] = 0.35;
	absorption[2] = 0.6;

	reducedScattering[0] = 3;
	reducedScattering[1] = 4.5;
	reducedScattering[2] = 6;

	std::string stringName = std::string(filePath);
	size_t endSub = stringName.find_last_of(".");
	size_t startSub = stringName.find_last_of("/");
	__entityName = stringName.substr(startSub+1, endSub - startSub - 1);

	//subdivideFaces(0.5);

	bounds = NULL;
	float frustumDepth = abs((float)getControllerInstance()->camera()->farPlane() - (float)getControllerInstance()->camera()->nearPlane());

	switch(__shader->getShaderType())
	{

	case SHADER_TYPE_RASTERIZE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		break;
	}
	case SHADER_TYPE_RAY_TRACE:
	{
		__treeTex = createKdTreeTextureBuffers(__meshTree);
		createFaceTexture();
		addTreeTextureBuffers();
		break;
	}
	case SHADER_TYPE_RADIOSITY:
		break;

	case SHADER_TYPE_TREE_BOUNDARY_OUTLINE:
	{
		__meshTree = new MeshHierarchyKd(__objLoader->faceList(), __objLoader->faceList().size());
		__treeTex = createKdTreeTextureBuffers(__meshTree);
		addTreeTextureBuffers();
		break;
	}
	case SHADER_TYPE_ADS:
	{
		//need to add the lights afterwards, done within the scene class
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		break;
	}
	case SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_MONTE_CARLO_BRDF:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}

	case SHADER_TYPE_DEPTH:
	{
		addVertices();

		addMatrixUniforms();
		int fp = getControllerInstance()->camera()->farPlane();
		int np = getControllerInstance()->camera()->nearPlane();
		__shader->addUniform(&fp, "farPlane", UNIFORM_INT_1, false);
		__shader->addUniform(&np, "nearPlane", UNIFORM_INT_1, false);
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_SHADOW_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_PHOTON_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}

	case SHADER_TYPE_SVD_BRDF:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addMatrixUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_SH_BRDF:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();
		addFaceIndices();
		//addSampledBrdf(0);
		
		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}

	case SHADER_TYPE_WAVELET_ENVIRONMENT_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();
		//addFaceIndices();
		//addVerticesIndex();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}
	case SHADER_TYPE_SH_BRDF_PHOTON_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();
		addFaceIndices();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);
		break;
	}
	case SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}
	case SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}
	case SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}
	case SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		float nearPlane = (float)getControllerInstance()->camera()->nearPlane();
		float farPlane = (float)getControllerInstance()->camera()->farPlane();
		__shader->addUniform((void*)&nearPlane, "nearPlane", UNIFORM_FLOAT_1,false);
		__shader->addUniform((void*)&farPlane, "farPlane", UNIFORM_FLOAT_1, false);

		break;
	}
	case SHADER_TYPE_POSITION_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		computeBounds();

		float boundsMin[] = { this->bounds->low->x,this->bounds->low->y,this->bounds->low->z };
		float boundsMax[] = { this->bounds->high->x,this->bounds->high->y,this->bounds->high->z };

		__shader->addUniform(boundsMin, "boundsMin", UNIFORM_FLOAT_3, false);
		__shader->addUniform(boundsMax, "boundsMax", UNIFORM_FLOAT_3, false);

		break;
	}

	case SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}

	case SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		float nearPlane = (float)getControllerInstance()->camera()->nearPlane();
		float farPlane = (float)getControllerInstance()->camera()->farPlane();
		__shader->addUniform((void*)&nearPlane, "nearPlane", UNIFORM_FLOAT_1, false);
		__shader->addUniform((void*)&farPlane, "farPlane", UNIFORM_FLOAT_1, false);

		break;
	}

	case SHADER_TYPE_NORMAL_PROJECTION_TEXTURE:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();

		break;
	}

	case SHADER_TYPE_REFLECTIVE_SHADOW_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}

	case SHADER_TYPE_RADIANCE_SHADOW_MAP:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}

	case SHADER_TYPE_INSTANT_RADIOSITY:
	{
		addVertices();
		addNormals();
		addTextureCoord();
		addMaterialIndex();
		addBasisXZ();

		if (__objLoader->materials().size() != 0)
		{
			addMaterialBuffer();
			addMaterialTextureArray();
			addMaterialBumpMapTextureArray();
		}
		addBoundingBox();
		addMatrixUniforms();
		addMaterialUniforms();
		__shader->addUniform((void*)(&frustumDepth), "frustumDepth", UNIFORM_FLOAT_1, false);

		break;
	}
			

	}
	__pos = __position;
	computeBounds();
#ifdef _EDITOR
	outlineShader = new OutlineShader(this->bounds);
	__shader->addUniform(&__shader->shaderType, "shaderType", UNIFORM_INT_1, true);
#endif
}

Model_obj::~Model_obj()
{
	delete bounds;
	delete outlineShader;

	delete __shader;
	delete __objLoader;
	if(__meshTree != NULL)
		delete __meshTree;
	if(__treeTex != NULL)
		delete __treeTex;
}

void Model_obj::copyParameters(Model_obj* m)
{
	this->brdfType = m->brdfType;
	this->indexOfRefraction = m->indexOfRefraction;
	this->phongN = m->phongN;
	this->microfacetM = m->microfacetM;
	this->minnaertK = m->minnaertK;

	this->diffuseProbability = m->diffuseProbability;
	this->specularProbability = m->specularProbability;
}

void Model_obj::render()
{
	switch (__shader->shaderType)
	{
	case SHADER_TYPE_RASTERIZE:
		((RastShader*)__shader)->render();
		break;

	case SHADER_TYPE_RAY_TRACE:
		break;

	case SHADER_TYPE_TREE_BOUNDARY_OUTLINE:
		break;

	case SHADER_TYPE_ADS:
		((AdsShader*)__shader)->render();
		break;

	case SHADER_TYPE_ADS_BRDF:
		break;

	case SHADER_TYPE_GLOBAL_ILLUMINATION_JACOBI:
		((GlobalIlluminationJacobiShader*)__shader)->render();
		break;
		
	case SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID:
		//((RadianceCacheUniformGridShader*)__shader)->render();
		break;

	case SHADER_TYPE_MONTE_CARLO_BRDF:
		((MonteCarloBRDFShader*)__shader)->render();
		break;
	case SHADER_TYPE_DEPTH:
		((DepthShader*)__shader)->render();
		break;
	case SHADER_TYPE_SHADOW_MAP:
		((ShadowMapShader*)__shader)->render();
		break;
	case SHADER_TYPE_SH_BRDF:
		((SHBRDFShader*)__shader)->render();
		break;
	case SHADER_TYPE_WAVELET_ENVIRONMENT_MAP:
		((WaveletEnvironmentMapShader*)__shader)->render();
		break;
	case SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE:
		((PhotonMapProjectionTextureShader*)__shader)->render();
		break;
	case SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE:
		((NormalDepthProjectionTextureShader*)__shader)->render();
		break;
	default:
		__shader->render();
		break;
	}

#ifdef _EDITOR
	if (highlighted || selected)
		outlineShader->render();
#endif
}

Shader* Model_obj::shader()
{
	return __shader;
}

unsigned int Model_obj::shaderType()
{
	return __shader->getShaderType();
}
void Model_obj::setShaderType(unsigned int st)
{
	__shader->setShaderType(st);
}

void Model_obj::computeBounds()
{
	int numVerts = __objLoader->vertexList().size();

	GLfloat bX, bY, bZ, tX, tY, tZ;
	Vertex* vert = __objLoader->vertexList()[0];
	bX = vert->pos[0];
	bY = vert->pos[1];
	bZ = vert->pos[2];
	tX = bX;
	tY = bY;
	tZ = bZ;

	for (int i = 0; i < numVerts; i++)
	{
		vert = __objLoader->vertexList()[i];
		bX = minimum(bX, vert->pos[0]);
		bY = minimum(bY, vert->pos[1]);
		bZ = minimum(bZ, vert->pos[2]);

		tX = maximum(tX, vert->pos[0]);
		tY = maximum(tY, vert->pos[1]);
		tZ = maximum(tZ, vert->pos[2]);
	}

	bounds = new Bounds(bX, bY, bZ, tX, tY, tZ);
}

void Model_obj::addVertices()
{
	if( __objLoader->vertexList().size() <= 0)
		return;

	int numVertices = __objLoader->faceList().size() * 3 * 4;
	GLfloat *vertexData = new GLfloat[numVertices];
	int vertexIndex = 0;
	Face* currentFace;
	for(int i = 0; i < __objLoader->faceList().size(); i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];
		
		vertexData[vertexIndex] = currentFace->vertex[0]->pos[0];
		vertexData[vertexIndex+1] = currentFace->vertex[0]->pos[1];
		vertexData[vertexIndex+2] = currentFace->vertex[0]->pos[2];
		vertexData[vertexIndex+3] = 1.0;

		vertexData[vertexIndex+4] = currentFace->vertex[1]->pos[0];
		vertexData[vertexIndex+5] = currentFace->vertex[1]->pos[1];
		vertexData[vertexIndex+6] = currentFace->vertex[1]->pos[2];
		vertexData[vertexIndex+7] = 1.0;

		vertexData[vertexIndex+8] = currentFace->vertex[2]->pos[0];
		vertexData[vertexIndex+9] = currentFace->vertex[2]->pos[1];
		vertexData[vertexIndex+10] = currentFace->vertex[2]->pos[2];
		vertexData[vertexIndex+11] = 1.0;

		vertexIndex = vertexIndex + 12;
	}

	__shader->addAttributeBuffer(vertexData,numVertices,sizeof(GLfloat), GL_FLOAT, "position", 0, 4);
	delete vertexData;
}

void Model_obj::addNormals()
{	
	int numVertices = __objLoader->faceList().size()*3*4;
	GLfloat *normData = new GLfloat[numVertices];
	int vertexIndex = 0;
	Face* currentFace;
	for(int i = 0; i < __objLoader->faceList().size(); i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];
		
		normData[vertexIndex] = currentFace->normal[0]->pos[0];
		normData[vertexIndex+1] = currentFace->normal[0]->pos[1];
		normData[vertexIndex+2] = currentFace->normal[0]->pos[2];
		normData[vertexIndex+3] = 1.0;

		normData[vertexIndex+4] = currentFace->normal[1]->pos[0];
		normData[vertexIndex+5] = currentFace->normal[1]->pos[1];
		normData[vertexIndex+6] = currentFace->normal[1]->pos[2];
		normData[vertexIndex+7] = 1.0;

		normData[vertexIndex+8] = currentFace->normal[2]->pos[0];
		normData[vertexIndex+9] = currentFace->normal[2]->pos[1];
		normData[vertexIndex+10] = currentFace->normal[2]->pos[2];
		normData[vertexIndex+11] = 1.0;

		vertexIndex = vertexIndex + 12;
	}

	__shader->addAttributeBuffer(normData,numVertices,sizeof(GLfloat), GL_FLOAT, "normal", 2, 4);
	delete normData;
}

void Model_obj::addBasisXZ()
{
	int numVertices = __objLoader->faceList().size() * 3 * 4;
	GLfloat *basisXdata = new GLfloat[numVertices];
	GLfloat *basisZdata = new GLfloat[numVertices];
	int vertexIndex = 0;
	Face* currentFace;
	float basisX[4];
	float basisZ[4];

	float basisY[4];
	Matrix4x4 mat;
	mat.loadIdentity();

	for (int i = 0; i < __objLoader->faceList().size(); i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];

		for (int j = 0; j < 3; j++)
		{
			mat.loadIdentity();

			basisY[0] = currentFace->normal[j]->pos[0];
			basisY[1] = currentFace->normal[j]->pos[1];
			basisY[2] = currentFace->normal[j]->pos[2];

			mat.rotate(asin(basisY[0]), 0.0, 1.0, 0.0);
			mat.rotate(asin(basisY[1]), 1.0, 0.0, 0.0);

			basisX[0] = mat.get(0, 0);
			basisX[1] = mat.get(1, 0);
			basisX[2] = mat.get(2, 0);

			basisZ[0] = mat.get(0, 1);
			basisZ[1] = mat.get(1, 1);
			basisZ[2] = mat.get(2, 1);

			basisXdata[vertexIndex] = basisX[0];
			basisXdata[vertexIndex + 1] = basisX[1];
			basisXdata[vertexIndex + 2] = basisX[2];
			basisXdata[vertexIndex + 3] = 0.0;

			basisZdata[vertexIndex] = basisZ[0];
			basisZdata[vertexIndex + 1] = basisZ[1];
			basisZdata[vertexIndex + 2] = basisZ[2];
			basisZdata[vertexIndex + 3] = 0.0;

			vertexIndex = vertexIndex + 4;
		}
	}

	__shader->addAttributeBuffer(basisXdata, numVertices, sizeof(GLfloat), GL_FLOAT, "basisX", 5, 4);
	__shader->addAttributeBuffer(basisZdata, numVertices, sizeof(GLfloat), GL_FLOAT, "basisZ", 6, 4);
	delete basisXdata;
	delete basisZdata;
}


void Model_obj::addTextureCoord()
{
	if( __objLoader->texCoordList().size() <= 0)
		return;

	int numVertices = __objLoader->faceList().size()*3*2;	// numberofFaces*3verticesPerFace*4floatsPerVertex
	GLfloat *tcData = new GLfloat[numVertices];
	int vertexIndex = 0;
	Face* currentFace;
	for(int i = 0; i < __objLoader->faceList().size(); i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];
		
		tcData[vertexIndex] = currentFace->texCoord[0]->pos[0];
		tcData[vertexIndex+1] = currentFace->texCoord[0]->pos[1];
		
		tcData[vertexIndex+2] = currentFace->texCoord[1]->pos[0];
		tcData[vertexIndex+3] = currentFace->texCoord[1]->pos[1];

		tcData[vertexIndex+4] = currentFace->texCoord[2]->pos[0];
		tcData[vertexIndex+5] = currentFace->texCoord[2]->pos[1];

		vertexIndex = vertexIndex + 6;
	}

	__shader->addAttributeBuffer(tcData,numVertices,sizeof(GLfloat), GL_FLOAT, "textureCoord", 4, 2);
	delete tcData;
}

void Model_obj::addMaterialIndex()
{
	int numVertices = __objLoader->faceList().size();
	GLfloat *matIndexData = new GLfloat[numVertices*3];
	
	
	int vertexIndex = 0;
	Face* currentFace;
	int matDataIndex = 0;
	for(int i=0; i< numVertices; i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];
		for(int j=0; j< 3; j++)
		{
			matIndexData[matDataIndex] = (GLfloat)currentFace->materialIndex;
			matDataIndex++;
		}
	}
	
	__shader->addAttributeBuffer(matIndexData, numVertices*3, sizeof(GLfloat), GL_FLOAT, "materialIndex", 3, 1);
	delete matIndexData;
}

void Model_obj::addMaterialBuffer()
{
	Array* materialFloat = createMaterialTexFloatArray(__objLoader->materials());
	Array* materialInt = createMaterialTexIntArray(__objLoader->materials());
	int sizeofMaterialTexInt = 1;
	int sizeofMaterialTexFloat = 4;

	__shader->addBufferTexture(materialFloat->buf, materialFloat->n, sizeof(Material_tex_float), "materialTexFloat", GL_RGBA32F, -1);
	__shader->addUniform(&sizeofMaterialTexFloat, "materialTexFloatSize", UNIFORM_INT_1, false);


	__shader->addBufferTexture(materialInt->buf, materialInt->n, sizeof(Material_tex_int), "materialTexInt", GL_RGBA32I, -1);
	__shader->addUniform(&sizeofMaterialTexInt, "materialTexIntSize", UNIFORM_INT_1, false);
	delete materialFloat;
	delete materialInt;
}
GLint texIndex = 0;
void Model_obj::addMaterialTextureArray()
{
	int numMaterials = __objLoader->materials().size();

	std::vector<Texture_obj<GLubyte>*> texArray;
	for(int i = 0 ; i< numMaterials; i++)
	{
		if ((__objLoader->materials()[i])->texture != NULL)
			texArray.push_back((__objLoader->materials()[i])->texture);
	}
	
	if (texArray.size() > 0)
	{
		float texBound = 1.0;
		__shader->addUniform(&texBound, "isKdTexBound", UNIFORM_FLOAT_1, false);
	}
	else
	{
		float texNotBound = 0.0;
		__shader->addUniform(&texNotBound, "isKdTexBound", UNIFORM_FLOAT_1, false);
		return;
	}

	__shader->addTextureArray(texArray, texArray.size(), "kdTexture", -1);
	texIndex++;
}

void Model_obj::addMaterialBumpMapTextureArray()
{
	int numMaterials = __objLoader->materials().size();

	if (numMaterials > 0)
	{
		float texBound = 1.0;
		__shader->addUniform(&texBound, "isBumpMapTextureBound", UNIFORM_FLOAT_1, false);
	}
	else
	{
		float texNotBound = 0.0;
		__shader->addUniform(&texNotBound, "isBumpMapTextureBound", UNIFORM_FLOAT_1, false);
		return;
	}

	std::vector<Texture_obj<GLubyte>*> texArray;
	for (int i = 0; i < numMaterials; i++)
	{
		if ((__objLoader->materials()[i])->bumpMapTexture != NULL)
			texArray.push_back((__objLoader->materials()[i])->bumpMapTexture);
	}
	__shader->addTextureArray(texArray, texArray.size(), "bumpMapTexture", -1);
	texIndex++;
	if (__objLoader->materials()[0]->texture != NULL)
	{
		float bumpMapEpsX = 1.0 / (__objLoader->materials()[0]->texture->width);
		float bumpMapEpsY = 1.0 / (__objLoader->materials()[0]->texture->height);
		__shader->addUniform(&bumpMapEpsX, "bumpMapEpsX", UNIFORM_FLOAT_1, false);
		__shader->addUniform(&bumpMapEpsY, "bumpMapEpsY", UNIFORM_FLOAT_1, false);
	}
}

void Model_obj::cloneMaterialTexInt(Material_tex_int *mStd, Material* m)
{
	mStd->materialIndex = m->materialIndex;
	mStd->illum = m->illum;
}

void Model_obj::cloneMaterialTexFloat(Material_tex_float *mStd, Material* m)
{
	mStd->ka[0] = m->ka[0];
	mStd->ka[1] = m->ka[1];
	mStd->ka[2] = m->ka[2];

	mStd->kd[0] = m->kd[0];
	mStd->kd[1] = m->kd[1];
	mStd->kd[2] = m->kd[2];

	mStd->ni = m->ni;

	mStd->ks[0] = m->ks[0];
	mStd->ks[1] = m->ks[1];
	mStd->ks[2] = m->ks[2];

	mStd->ns = m->ns;

	mStd->tf[0] = m->tf[0];
	mStd->tf[1] = m->tf[1];
	mStd->tf[2] = m->tf[2];

	mStd->d = m->d;

	mStd->sharpness = m->sharpness;
}

Array* Model_obj::createMaterialTexFloatArray(std::vector<Material*> materialList)
{
	Array* arr = new Array();

	Material_tex_float* matArray = new Material_tex_float[materialList.size()];

	for(int i=0; i< materialList.size(); i++)
	{
		cloneMaterialTexFloat(&matArray[i],(Material*)materialList[i]);
	}

	arr->n = materialList.size();
	arr->buf = matArray;
	return arr;
}
Array* Model_obj::createMaterialTexIntArray(std::vector<Material*> materialList)
{
	Array* arr = new Array();

	Material_tex_int* matArray = new Material_tex_int[materialList.size()];

	for(int i=0; i< materialList.size(); i++)
	{
		cloneMaterialTexInt(&matArray[i],(Material*)materialList[i]);
	}

	arr->n = materialList.size();
	arr->buf = matArray;
	return arr;
}

void Model_obj::addTreeTextureBuffers()
{
	GLint* renderIndex = new GLint[__treeTex->treeSize];
	for(int i=0; i< __treeTex->treeSize; i++)
	{
		renderIndex[i] = i;
	}
	__shader->addAttributeBuffer(renderIndex, __treeTex->treeSize, sizeof(GLint), GL_INT, "index", 0, 1);
	delete renderIndex;

	__shader->addBufferTexture(__treeTex->texInt, __treeTex->treeSize, sizeof(KdTreeTextureInt), "treeTextureInt", GL_RGBA32I, -1);
	__shader->addBufferTexture(__treeTex->texFloat, __treeTex->treeSize, sizeof(KdTreeTextureFloat), "treeTextureFloat", GL_RGBA32F, -1);
	__shader->addBufferTexture(createFaceTexture(), __objLoader->faceList().size(), sizeof(FaceTexture), "vertexFaceArrayTexture", GL_RGBA32F, -1);

	int treeSize = __treeTex->treeSize;
	int treeTextureFloatSize = 2;
	int treeTextureIntSize = 3;
	int treeFaceSize = 8;

	__shader->addUniform(&treeSize, "treeSize", UNIFORM_INT_1, false);
	__shader->addUniform(&treeTextureFloatSize, "treeTexutreFloatDataSize", UNIFORM_INT_1, false);
	__shader->addUniform(&treeTextureIntSize, "treeTexutreIntDataSize", UNIFORM_INT_1, false);
	__shader->addUniform(&treeFaceSize, "faceSize", UNIFORM_INT_1, false);
}

void Model_obj::addLight(std::vector<Light*> lights)
{
	if(lights.size() == 0)
		return;

	int lightBuffSize = sizeof(Light);
	int numLights = lights.size();
	GLfloat* lightBuf = (GLfloat*)malloc(lightBuffSize*numLights);

	for (int i = 0; i < numLights; i++)
	{
		int j = i*(lightBuffSize/4);
		lightBuf[j] = lights[i]->lightPosition[0];
		lightBuf[j+1] = lights[i]->lightPosition[1];
		lightBuf[j+2] = lights[i]->lightPosition[2];
		lightBuf[j+3] = lights[i]->lightPosition[3];

		lightBuf[j+4] = lights[i]->lightDirection[0];
		lightBuf[j+5] = lights[i]->lightDirection[1];
		lightBuf[j+6] = lights[i]->lightDirection[2];
		lightBuf[j+7] = lights[i]->lightDirection[3];

		lightBuf[j + 8] = lights[i]->intensity[0];
		lightBuf[j + 9] = lights[i]->intensity[1];
		lightBuf[j + 10] = lights[i]->intensity[2];
		lightBuf[j + 11] = lights[i]->intensity[3];
	}

	int lightTexFloatSize = lightBuffSize / (16);
	__shader->addBufferTexture((void*)lightBuf, numLights, sizeof(Light), "lights", GL_RGBA32F, -1);
	__shader->addUniform(&lightTexFloatSize, "lightTexFloatSize", UNIFORM_INT_1, false);
	__shader->addUniform(&numLights, "numLights", UNIFORM_INT_1, false);

	free(lightBuf);

}

void Model_obj::addLight(GLfloat* lightPos)
{
	__shader->addUniform((void*)lightPos, "lPosition", UNIFORM_FLOAT_4, true);
}

void Model_obj::addBoundaryTextures(float* boundaryTreeTexFloat, int boundaryTreeTexFloatSize, int* boundaryTreeTexInt, int boundaryTreeTexIntSize, int numBoundaries)
{
	__shader->addBufferTexture((void*)boundaryTreeTexFloat, numBoundaries, boundaryTreeTexFloatSize*16, "boundaryTexFloat", GL_RGBA32F, -1);
	__shader->addBufferTexture((void*)boundaryTreeTexInt, numBoundaries, boundaryTreeTexIntSize*16, "boundaryTexInt", GL_RGBA32F, -1);
	__shader->addUniform(&boundaryTreeTexFloatSize, "boundaryTexFloatSize", UNIFORM_INT_1, false);
	__shader->addUniform(&boundaryTreeTexIntSize, "boundaryTexIntSize", UNIFORM_INT_1, false);
}

void Model_obj::addFaceLight(FaceLight* faceLight)
{
	if(faceLight == NULL)
		return;

	__shader->addUniformBlock((void*)faceLight, 28, sizeof(GLfloat), "faceLight", 1);
}

FaceTexture* Model_obj::createFaceTexture()
{
	int numFaces = __objLoader->faceList().size();
	FaceTexture* faceTex = new FaceTexture[numFaces];
	Face* currentFace;
	for(int i=0; i< numFaces; i++)
	{
		currentFace = (Face*)__objLoader->faceList()[i];

		if(currentFace->vertex != NULL)
		{
			if(currentFace->vertex[0] != NULL)
			{
				faceTex[i].vertex[0][0] = currentFace->vertex[0]->pos[0];
				faceTex[i].vertex[0][1] = currentFace->vertex[0]->pos[1];
				faceTex[i].vertex[0][2] = currentFace->vertex[0]->pos[2];
				faceTex[i].vertex[0][3] = 1.0;
			}

			if(currentFace->vertex[1] != NULL)
			{
				faceTex[i].vertex[1][0] = currentFace->vertex[1]->pos[0];
				faceTex[i].vertex[1][1] = currentFace->vertex[1]->pos[1];
				faceTex[i].vertex[1][2] = currentFace->vertex[1]->pos[2];
				faceTex[i].vertex[1][3] = 1.0;
			}

			if(currentFace->vertex[2] != NULL)
			{
				faceTex[i].vertex[2][0] = currentFace->vertex[2]->pos[0];
				faceTex[i].vertex[2][1] = currentFace->vertex[2]->pos[1];
				faceTex[i].vertex[2][2] = currentFace->vertex[2]->pos[2];
				faceTex[i].vertex[2][3] = 1.0;
			}
		}

		if(currentFace->normal != NULL)
		{
			if(currentFace->normal[0] != NULL)
			{
				faceTex[i].normal[0][0] = currentFace->normal[0]->pos[0];
				faceTex[i].normal[0][1] = currentFace->normal[0]->pos[1];
				faceTex[i].normal[0][2] = currentFace->normal[0]->pos[2];
				faceTex[i].normal[0][3] = 1.0;
			}

			if(currentFace->normal[1] != NULL)
			{
				faceTex[i].normal[1][0] = currentFace->normal[1]->pos[0];
				faceTex[i].normal[1][1] = currentFace->normal[1]->pos[1];
				faceTex[i].normal[1][2] = currentFace->normal[1]->pos[2];
				faceTex[i].normal[1][3] = 1.0;
			}

			if(currentFace->normal[2] != NULL)
			{
				faceTex[i].normal[2][0] = currentFace->normal[2]->pos[0];
				faceTex[i].normal[2][1] = currentFace->normal[2]->pos[1];
				faceTex[i].normal[2][2] = currentFace->normal[2]->pos[2];
				faceTex[i].normal[2][3] = 1.0;
			}
		}

		if(currentFace->texCoord != NULL)
		{
			if(currentFace->texCoord[0] != NULL)
			{
				faceTex[i].texCoord[0][0] = currentFace->texCoord[0]->pos[0];
				faceTex[i].texCoord[0][1] = currentFace->texCoord[0]->pos[1];
			}

			if(currentFace->texCoord[1] != NULL)
			{
				faceTex[i].texCoord[1][0] = currentFace->texCoord[1]->pos[0];
				faceTex[i].texCoord[1][1] = currentFace->texCoord[1]->pos[1];
			}

			if(currentFace->texCoord[2] != NULL)
			{
				faceTex[i].texCoord[2][0] = currentFace->texCoord[2]->pos[0];
				faceTex[i].texCoord[2][1] = currentFace->texCoord[2]->pos[1];
			}
		}

		faceTex[i].materialIndex = currentFace->materialIndex;
		
		faceTex[i].iPad = 7;
	}
	return faceTex;
}

void Model_obj::addBoundingBox()
{
	int numVerts = __objLoader->vertexList().size();
	GLfloat* bounds = new GLfloat[8];
	Vertex* currentVert = (Vertex*)__objLoader->vertexList()[0];
	
	bounds[0] = currentVert->pos[0];
	bounds[1] = currentVert->pos[1];
	bounds[2] = currentVert->pos[2];
	bounds[3] = 1.0;
	bounds[4] = currentVert->pos[0];
	bounds[5] = currentVert->pos[1];
	bounds[6] = currentVert->pos[2];
	bounds[7] = 1.0;

	for(int i=1; i< numVerts; i++)
	{
		currentVert = (Vertex*)__objLoader->vertexList()[i];
		if(currentVert->pos[0] < bounds[0])
		{
			bounds[0] = currentVert->pos[0];
		}
		if(currentVert->pos[1] < bounds[1])
		{
			bounds[1] = currentVert->pos[1];
		}
		if(currentVert->pos[2] < bounds[2])
		{
			bounds[2] = currentVert->pos[2];
		}


		if(currentVert->pos[0] > bounds[4])
		{
			bounds[4] = currentVert->pos[0];
		}
		if(currentVert->pos[1] > bounds[5])
		{
			bounds[5] = currentVert->pos[1];
		}
		if(currentVert->pos[2] > bounds[6])
		{
			bounds[6] = currentVert->pos[2];
		}

	}

	__shader->addUniformBlock((void*)bounds, 8, sizeof(GLfloat), "boundingBox", 1);
	delete bounds;
}

void Model_obj::subdivideFaces(float maxArea)
{
	bool done = false;
	int startingSearch = 0;
	while (!done)
	{
		for (int i = startingSearch; i < faceList().size(); i++)
		{
			Face* currentFace = faceList()[i];
			if (currentFace->area() > maxArea)
			{
				Face* fsub1 = new Face();
				Face* fsub2 = new Face();
				Face* fsub3 = new Face();
				Face* fsub4 = new Face();

				fsub1->vertex[0] = new Vertex();
				fsub1->vertex[1] = new Vertex();
				fsub1->vertex[2] = new Vertex();

				fsub2->vertex[0] = new Vertex();
				fsub2->vertex[1] = new Vertex();
				fsub2->vertex[2] = new Vertex();

				fsub3->vertex[0] = new Vertex();
				fsub3->vertex[1] = new Vertex();
				fsub3->vertex[2] = new Vertex();

				fsub4->vertex[0] = new Vertex();
				fsub4->vertex[1] = new Vertex();
				fsub4->vertex[2] = new Vertex();



				fsub1->normal[0] = new Vertex();
				fsub1->normal[1] = new Vertex();
				fsub1->normal[2] = new Vertex();

				fsub2->normal[0] = new Vertex();
				fsub2->normal[1] = new Vertex();
				fsub2->normal[2] = new Vertex();

				fsub3->normal[0] = new Vertex();
				fsub3->normal[1] = new Vertex();
				fsub3->normal[2] = new Vertex();

				fsub4->normal[0] = new Vertex();
				fsub4->normal[1] = new Vertex();
				fsub4->normal[2] = new Vertex();


				float midPos1[] = { 0.5*(currentFace->vertex[0]->pos[0] + currentFace->vertex[1]->pos[0]),
					0.5*(currentFace->vertex[0]->pos[1] + currentFace->vertex[1]->pos[1]),
					0.5*(currentFace->vertex[0]->pos[2] + currentFace->vertex[1]->pos[2]) };

				float midPos2[] = { 0.5*(currentFace->vertex[0]->pos[0] + currentFace->vertex[2]->pos[0]),
					0.5*(currentFace->vertex[0]->pos[1] + currentFace->vertex[2]->pos[1]),
					0.5*(currentFace->vertex[0]->pos[2] + currentFace->vertex[2]->pos[2]) };

				float midPos3[] = { 0.5*(currentFace->vertex[1]->pos[0] + currentFace->vertex[2]->pos[0]),
					0.5*(currentFace->vertex[1]->pos[1] + currentFace->vertex[2]->pos[1]),
					0.5*(currentFace->vertex[1]->pos[2] + currentFace->vertex[2]->pos[2]) };



				if (currentFace->texCoord[0] != NULL)
				{
					fsub1->texCoord[0] = new TexCoord();
					fsub1->texCoord[1] = new TexCoord();
					fsub1->texCoord[2] = new TexCoord();

					fsub2->texCoord[0] = new TexCoord();
					fsub2->texCoord[1] = new TexCoord();
					fsub2->texCoord[2] = new TexCoord();

					fsub3->texCoord[0] = new TexCoord();
					fsub3->texCoord[1] = new TexCoord();
					fsub3->texCoord[2] = new TexCoord();

					fsub4->texCoord[0] = new TexCoord();
					fsub4->texCoord[1] = new TexCoord();
					fsub4->texCoord[2] = new TexCoord();

					float midTex1[] = { 0.5*(currentFace->texCoord[0]->pos[0] + currentFace->texCoord[1]->pos[0]),
						0.5*(currentFace->texCoord[0]->pos[1] + currentFace->texCoord[1]->pos[1]) };

					float midTex2[] = { 0.5*(currentFace->texCoord[0]->pos[0] + currentFace->texCoord[2]->pos[0]),
						0.5*(currentFace->texCoord[0]->pos[1] + currentFace->texCoord[2]->pos[1]) };

					float midTex3[] = { 0.5*(currentFace->texCoord[1]->pos[0] + currentFace->texCoord[2]->pos[0]),
						0.5*(currentFace->texCoord[1]->pos[1] + currentFace->texCoord[2]->pos[1]) };

					fsub1->texCoord[0]->pos[0] = currentFace->texCoord[0]->pos[0];
					fsub1->texCoord[0]->pos[1] = currentFace->texCoord[0]->pos[1];

					fsub1->texCoord[1]->pos[0] = midTex1[0];
					fsub1->texCoord[1]->pos[1] = midTex1[1];

					fsub1->texCoord[2]->pos[0] = midTex2[0];
					fsub1->texCoord[2]->pos[1] = midTex2[1];


					fsub2->texCoord[0]->pos[0] = currentFace->texCoord[1]->pos[0];
					fsub2->texCoord[0]->pos[1] = currentFace->texCoord[1]->pos[1];

					fsub2->texCoord[1]->pos[0] = midTex3[0];
					fsub2->texCoord[1]->pos[1] = midTex3[1];

					fsub2->texCoord[2]->pos[0] = midTex1[0];
					fsub2->texCoord[2]->pos[1] = midTex1[1];


					fsub3->texCoord[0]->pos[0] = currentFace->texCoord[2]->pos[0];
					fsub3->texCoord[0]->pos[1] = currentFace->texCoord[2]->pos[1];

					fsub3->texCoord[1]->pos[0] = midTex2[0];
					fsub3->texCoord[1]->pos[1] = midTex2[1];

					fsub3->texCoord[2]->pos[0] = midTex3[0];
					fsub3->texCoord[2]->pos[1] = midTex3[1];


					fsub4->texCoord[0]->pos[0] = midTex1[0];
					fsub4->texCoord[0]->pos[1] = midTex1[1];

					fsub4->texCoord[1]->pos[0] = midTex2[0];
					fsub4->texCoord[1]->pos[1] = midTex2[1];

					fsub4->texCoord[2]->pos[0] = midTex3[0];
					fsub4->texCoord[2]->pos[1] = midTex3[1];
				}
				

				/* COPY THE MATERIAL INDEX*/
				fsub1->materialIndex = currentFace->materialIndex;
				fsub2->materialIndex = currentFace->materialIndex;
				fsub3->materialIndex = currentFace->materialIndex;
				fsub4->materialIndex = currentFace->materialIndex;

				/* COPY THE NORMALS */
				fsub1->normal[0]->pos[0] = currentFace->normal[0]->pos[0];
				fsub1->normal[0]->pos[1] = currentFace->normal[0]->pos[1];
				fsub1->normal[0]->pos[2] = currentFace->normal[0]->pos[2];
				fsub1->normal[1]->pos[0] = currentFace->normal[1]->pos[0];
				fsub1->normal[1]->pos[1] = currentFace->normal[1]->pos[1];
				fsub1->normal[1]->pos[2] = currentFace->normal[1]->pos[2];
				fsub1->normal[2]->pos[0] = currentFace->normal[2]->pos[0];
				fsub1->normal[2]->pos[1] = currentFace->normal[2]->pos[1];
				fsub1->normal[2]->pos[2] = currentFace->normal[2]->pos[2];

				fsub2->normal[0]->pos[0] = currentFace->normal[0]->pos[0];
				fsub2->normal[0]->pos[1] = currentFace->normal[0]->pos[1];
				fsub2->normal[0]->pos[2] = currentFace->normal[0]->pos[2];
				fsub2->normal[1]->pos[0] = currentFace->normal[1]->pos[0];
				fsub2->normal[1]->pos[1] = currentFace->normal[1]->pos[1];
				fsub2->normal[1]->pos[2] = currentFace->normal[1]->pos[2];
				fsub2->normal[2]->pos[0] = currentFace->normal[2]->pos[0];
				fsub2->normal[2]->pos[1] = currentFace->normal[2]->pos[1];
				fsub2->normal[2]->pos[2] = currentFace->normal[2]->pos[2];

				fsub3->normal[0]->pos[0] = currentFace->normal[0]->pos[0];
				fsub3->normal[0]->pos[1] = currentFace->normal[0]->pos[1];
				fsub3->normal[0]->pos[2] = currentFace->normal[0]->pos[2];
				fsub3->normal[1]->pos[0] = currentFace->normal[1]->pos[0];
				fsub3->normal[1]->pos[1] = currentFace->normal[1]->pos[1];
				fsub3->normal[1]->pos[2] = currentFace->normal[1]->pos[2];
				fsub3->normal[2]->pos[0] = currentFace->normal[2]->pos[0];
				fsub3->normal[2]->pos[1] = currentFace->normal[2]->pos[1];
				fsub3->normal[2]->pos[2] = currentFace->normal[2]->pos[2];

				fsub4->normal[0]->pos[0] = currentFace->normal[0]->pos[0];
				fsub4->normal[0]->pos[1] = currentFace->normal[0]->pos[1];
				fsub4->normal[0]->pos[2] = currentFace->normal[0]->pos[2];
				fsub4->normal[1]->pos[0] = currentFace->normal[1]->pos[0];
				fsub4->normal[1]->pos[1] = currentFace->normal[1]->pos[1];
				fsub4->normal[1]->pos[2] = currentFace->normal[1]->pos[2];
				fsub4->normal[2]->pos[0] = currentFace->normal[2]->pos[0];
				fsub4->normal[2]->pos[1] = currentFace->normal[2]->pos[1];
				fsub4->normal[2]->pos[2] = currentFace->normal[2]->pos[2];

				/* 0, mid1, mid2*/
				fsub1->vertex[0]->pos[0] = currentFace->vertex[0]->pos[0];
				fsub1->vertex[0]->pos[1] = currentFace->vertex[0]->pos[1];
				fsub1->vertex[0]->pos[2] = currentFace->vertex[0]->pos[2];

				fsub1->vertex[1]->pos[0] = midPos1[0];
				fsub1->vertex[1]->pos[1] = midPos1[1];
				fsub1->vertex[1]->pos[2] = midPos1[2];

				fsub1->vertex[2]->pos[0] = midPos2[0];
				fsub1->vertex[2]->pos[1] = midPos2[1];
				fsub1->vertex[2]->pos[2] = midPos2[2];

				


				/*1, mid3, mid1*/
				fsub2->vertex[0]->pos[0] = currentFace->vertex[1]->pos[0];
				fsub2->vertex[0]->pos[1] = currentFace->vertex[1]->pos[1];
				fsub2->vertex[0]->pos[2] = currentFace->vertex[1]->pos[2];

				fsub2->vertex[1]->pos[0] = midPos3[0];
				fsub2->vertex[1]->pos[1] = midPos3[1];
				fsub2->vertex[1]->pos[2] = midPos3[2];

				fsub2->vertex[2]->pos[0] = midPos1[0];
				fsub2->vertex[2]->pos[1] = midPos1[1];
				fsub2->vertex[2]->pos[2] = midPos1[2];

				

				/*2, mid2, mid3*/
				fsub3->vertex[0]->pos[0] = currentFace->vertex[2]->pos[0];
				fsub3->vertex[0]->pos[1] = currentFace->vertex[2]->pos[1];
				fsub3->vertex[0]->pos[2] = currentFace->vertex[2]->pos[2];

				fsub3->vertex[1]->pos[0] = midPos2[0];
				fsub3->vertex[1]->pos[1] = midPos2[1];
				fsub3->vertex[1]->pos[2] = midPos2[2];

				fsub3->vertex[2]->pos[0] = midPos3[0];
				fsub3->vertex[2]->pos[1] = midPos3[1];
				fsub3->vertex[2]->pos[2] = midPos3[2];

				

				/*mid1,mid2,mid3*/
				fsub4->vertex[0]->pos[0] = midPos1[0];
				fsub4->vertex[0]->pos[1] = midPos1[1];
				fsub4->vertex[0]->pos[2] = midPos1[2];

				fsub4->vertex[1]->pos[0] = midPos2[0];
				fsub4->vertex[1]->pos[1] = midPos2[1];
				fsub4->vertex[1]->pos[2] = midPos2[2];

				fsub4->vertex[2]->pos[0] = midPos3[0];
				fsub4->vertex[2]->pos[1] = midPos3[1];
				fsub4->vertex[2]->pos[2] = midPos3[2];

				

				fsub1->avg[0] = (fsub1->vertex[0]->pos[0] + fsub1->vertex[1]->pos[0] + fsub1->vertex[2]->pos[0]) / 3;
				fsub1->avg[1] = (fsub1->vertex[0]->pos[1] + fsub1->vertex[1]->pos[1] + fsub1->vertex[2]->pos[1]) / 3;
				fsub1->avg[2] = (fsub1->vertex[0]->pos[2] + fsub1->vertex[1]->pos[2] + fsub1->vertex[2]->pos[2]) / 3;

				fsub2->avg[0] = (fsub2->vertex[0]->pos[0] + fsub2->vertex[1]->pos[0] + fsub2->vertex[2]->pos[0]) / 3;
				fsub2->avg[1] = (fsub2->vertex[0]->pos[1] + fsub2->vertex[1]->pos[1] + fsub2->vertex[2]->pos[1]) / 3;
				fsub2->avg[2] = (fsub2->vertex[0]->pos[2] + fsub2->vertex[1]->pos[2] + fsub2->vertex[2]->pos[2]) / 3;

				fsub3->avg[0] = (fsub3->vertex[0]->pos[0] + fsub3->vertex[1]->pos[0] + fsub3->vertex[2]->pos[0]) / 3;
				fsub3->avg[1] = (fsub3->vertex[0]->pos[1] + fsub3->vertex[1]->pos[1] + fsub3->vertex[2]->pos[1]) / 3;
				fsub3->avg[2] = (fsub3->vertex[0]->pos[2] + fsub3->vertex[1]->pos[2] + fsub3->vertex[2]->pos[2]) / 3;

				fsub4->avg[0] = (fsub4->vertex[0]->pos[0] + fsub4->vertex[1]->pos[0] + fsub4->vertex[2]->pos[0]) / 3;
				fsub4->avg[1] = (fsub4->vertex[0]->pos[1] + fsub4->vertex[1]->pos[1] + fsub4->vertex[2]->pos[1]) / 3;
				fsub4->avg[2] = (fsub4->vertex[0]->pos[2] + fsub4->vertex[1]->pos[2] + fsub4->vertex[2]->pos[2]) / 3;


				removeFace(i);
				addFace(fsub1);
				addFace(fsub2);
				addFace(fsub3);
				addFace(fsub4);

			}

			for (int j = 0; j < faceList().size(); j++)
			{
				if (faceList()[i]->area()  > maxArea)
				{
					done = false;
					startingSearch = j;
					break;
				}
				else
				{
					done = true;
				}
			}
		}
	}
}

char* Model_obj::filePath()
{
	return (char*)__filePath;
}

std::vector<Face*> Model_obj::faceList()
{
	return __objLoader->faceList();
}

std::vector<Vertex*> Model_obj::vertexList()
{
	return __objLoader->vertexList();
}

void Model_obj::addFace(Face* f)
{
	this->__objLoader->addFace(f);
}
void Model_obj::removeFace(int index)
{
	this->__objLoader->removeFace(index);
}

FaceLight::FaceLight()
{
	vertex1[0] = 0.0;
	vertex1[1] = 0.0;
	vertex1[2] = 0.0;
	vertex1[3] = 1.0;

	vertex2[0] = 0.0;
	vertex2[1] = 0.0;
	vertex2[2] = 0.0;
	vertex2[3] = 1.0;

	vertex3[0] = 0.0;
	vertex3[1] = 0.0;
	vertex3[2] = 0.0;
	vertex3[3] = 1.0;

	lightDirection[0] = 0.0;
	lightDirection[1] = 0.0;
	lightDirection[2] = -1.0;
	lightDirection[3] = 0.0;

	lightDiffuseColor[0] = 1.0;
	lightDiffuseColor[1] = 1.0;
	lightDiffuseColor[2] = 1.0;
	lightDiffuseColor[3] = 1.0;

	lightSpecularColor[0] = 1.0;
	lightSpecularColor[1] = 1.0;
	lightSpecularColor[2] = 1.0;
	lightSpecularColor[3] = 1.0;

	radiance = 30.0;
	pad1 = 0.0;
	pad2 = 0.0;
	pad3 = 0.0;
}

void FaceLight::normalize()
{
	float mag = lightDirection[0]*lightDirection[0] + lightDirection[1]*lightDirection[1] + lightDirection[2]*lightDirection[2];
	mag = sqrt(mag);
	lightDirection[0] =  lightDirection[0]/mag;
	lightDirection[1] =  lightDirection[1]/mag;
	lightDirection[2] =  lightDirection[2]/mag;
}

Light::Light()
{
	lightPosition[0] = 0.0;
	lightPosition[1] = 0.0;
	lightPosition[2] = 0.0;
	lightPosition[3] = 0.0;

	lightDirection[0] = 0.0;
	lightDirection[1] = -0.1;
	lightDirection[2] = -1.0;
	lightDirection[3] = 0.0;

	intensity[0] = 1.0;
	intensity[1] = 1.0;
	intensity[2] = 1.0;
	intensity[3] = 1.0;

	normalize();
}

void Light::normalize()
{
	float mag = lightDirection[0]*lightDirection[0] + lightDirection[1]*lightDirection[1] + lightDirection[2]*lightDirection[2];
	mag = sqrt(mag);
	lightDirection[0] =  lightDirection[0]/mag;
	lightDirection[1] =  lightDirection[1]/mag;
	lightDirection[2] =  lightDirection[2]/mag;
}

void Model_obj::update(Camera* cam)
{
	Entity::update(cam);
	RigidBody::update(0.000333);
}

void Model_obj::addIrradianceVector()
{
	int n = irradianceVector.size()*irradianceVector[0].size();
	float* irr = new float[n];
	int index = 0;
	for (int i = 0; i < irradianceVector.size(); i++)
	{
		for (int j = 0; j < irradianceVector[i].size(); j++)
		{
			irr[index] = irradianceVector[i][j];
			index++;
		}
	}
	
	__shader->addBufferTexture(irr, n, sizeof(float), "irradianceTexture", GL_R32F, -1);
	int directionsPerVertex = irradianceVector[0].size();
	delete irr;
}

void Model_obj::addIrradianceSH()
{
	int N = SPHERICAL_HARMONICS_ACCURACY;
	int index = 0;
	float* irradianceShTex = new float[N*photonHits.size() * 9 * 2];
	for (int i = 0; i < photonHits.size(); i++)
	{
		HitPointFace* ph = photonHits[i];
		for (int j = 0; j < N; j++)
		{
			irradianceShTex[index]     = (ph->sphericalHarmonics[j])[0]->real;
			irradianceShTex[index + 1] = (ph->sphericalHarmonics[j])[1]->real;
			irradianceShTex[index + 2] = (ph->sphericalHarmonics[j])[2]->real;
			irradianceShTex[index + 3] = (ph->sphericalHarmonics[j])[0]->imag;
			irradianceShTex[index + 4] = (ph->sphericalHarmonics[j])[1]->imag;
			irradianceShTex[index + 5] = (ph->sphericalHarmonics[j])[2]->imag;

			index = index + 6;
		}

	}

	shader()->addBufferTexture((void*)irradianceShTex, N*photonHits.size() * 9 * 2, sizeof(float), "irradianceSH", GL_RGB32F, -1);
	delete irradianceShTex;
}

void Model_obj::addVerticesIndex()
{
	float* vertIndexAtt = new float[faceList().size() * 3];
	int vIndex = 0;
	for (int i = 0; i < this->faceList().size(); i++)
	{
		Face* f = faceList()[i];
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = f->vertIndex[j];
			vertIndexAtt[vIndex] = (float)vertexIndex;
			vIndex++;
		}
	}
	shader()->addAttributeBuffer((void*)vertIndexAtt, faceList().size() * 3, sizeof(float), GL_FLOAT, "vertIndex", 7, 1);
	delete vertIndexAtt;
}

void Model_obj::addVertexIrradianceSH()
{
	int N = SPHERICAL_HARMONICS_ACCURACY;
	int texSize = vertexRadiance.size() * N * 3;
	float* irradianceShTex = new float[texSize];
	float* vertIndexAtt = new float[faceList().size()*3];
	int vIndex = 0;

	for (int i = 0; i < this->faceList().size(); i++)
	{
		Face* f = faceList()[i];
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = f->vertIndex[j];
			VertexRadiance* vr = vertexRadiance[j];

			vertIndexAtt[vIndex] = (float)vertexIndex;
			vIndex++;
		}
	}

	int index = 0;
	for (int i = 0; i < vertexRadiance.size(); i++)
	{
		VertexRadiance* vr = vertexRadiance[i];
		for (int k = 0; k < N; k++)
		{

			irradianceShTex[index] = sqrt(pow((vr->shIrradiance[k])[0]->real, 2.0) + pow((vr->shIrradiance[k])[0]->imag, 2.0));
			irradianceShTex[index + 1] = sqrt(pow((vr->shIrradiance[k])[1]->real, 2.0) + pow((vr->shIrradiance[k])[1]->imag, 2.0));
			irradianceShTex[index + 2] = sqrt(pow((vr->shIrradiance[k])[2]->real, 2.0) + pow((vr->shIrradiance[k])[2]->imag, 2.0));

			index = index + 3;

		}
	}

	shader()->addBufferTexture((void*)irradianceShTex, texSize, sizeof(float), "irradianceVertexSH", GL_RGB32F, -1);
	shader()->addAttributeBuffer((void*)vertIndexAtt, faceList().size() * 3, sizeof(float), GL_FLOAT, "vertIndex", 8, 1);

	delete irradianceShTex;
	delete vertIndexAtt;
}

void Model_obj::addFaceIndices()
{
	float* faceIndex = new float[faceList().size()*3];
	for (int i = 0; i < faceList().size(); i++)
	{
		faceIndex[3*i] = (float)i;
		faceIndex[3 * i + 1] = (float)i;
		faceIndex[3 * i + 2] = (float)i;
	}

	__shader->addAttributeBuffer(faceIndex, faceList().size() * 3, sizeof(GLfloat), GL_FLOAT, "faceIndex", 7, 1);
	delete faceIndex;
}

void Model_obj::addSampledBrdf(int brdfIndex)
{
	BrdfSampled* b = brdfSampled[brdfIndex];

	int matSize = 4 * b->n*b->n;
	int brdfSampledSHSize = matSize*matSize + 2 * b->n*b->n*b->n*b->n;
	float* brdfSampledSH = new float[brdfSampledSHSize];
	int index = 0;
	for (int i = 0; i < matSize; i++)
	{
		for (int j = 0; j < matSize; j++)
		{
			brdfSampledSH[index] = b->brdf[0]->get(i, j);
			index++;
		}
	}

	for (int i = 0; i < b->n*b->n; i++)
	{
		for (int j = 0; j < b->n*b->n; j++)
		{
			brdfSampledSH[index] = b->brdf[1]->get(i, j);
			index++;
		}
	}

	for (int i = 0; i < b->n*b->n; i++)
	{
		for (int j = 0; j < b->n*b->n; j++)
		{
			brdfSampledSH[index] = b->brdf[2]->get(i, j);
			index++;
		}
	}
	this->shader()->addBufferTexture((void*)brdfSampledSH, brdfSampledSHSize, sizeof(float), "sampledBrdfSH", GL_R32F, -1);
	delete brdfSampledSH;
}

void Model_obj::addSampledBrdfSH(int brdfIndex)
{
	BrdfSampled* b = brdfSampled[brdfIndex];

	int matSize = 4 * b->n * b->n;
	int brdfSampledSHSize = matSize*SPHERICAL_HARMONICS_ACCURACY * 2 + matSize*SPHERICAL_HARMONICS_ACCURACY/2 + matSize*SPHERICAL_HARMONICS_ACCURACY/2;
	float* brdfSampledSH = new float[brdfSampledSHSize];
	int shIndex = 0;

	std::vector<Complex*> sh;
	std::vector<SphericalCoords*> sc;
	std::vector<float> f;
	for (int i = 0; i < matSize; i++)
	{
		SphericalCoords* s = new SphericalCoords();
		sc.push_back(s);
		f.push_back(0);
	}
	for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
	{
		Complex* c = new Complex();
		sh.push_back(c);
	}


	
	int index = 0;
	int inIndex = 0;
	int outIndex = 0;
	for (int i = 0; i < b->n; i++)
	{
		float theta_out = i * 0.5 * M_PI / b->n;
		for (int j = 0; j < 4 * b->n; j++)
		{
			inIndex = 0;
			float phi_out = j * 2.0 * M_PI / (4 * b->n);
			for (int k = 0; k < b->n; k++)
			{

				float theta_in = k * 0.5 * M_PI / b->n;
				for (int l = 0; l < 4 * b->n; l++)
				{
					float phi_in = l * 2.0 * M_PI / (4 * b->n);

					sc[inIndex]->phi = phi_in;
					sc[inIndex]->theta = theta_in;
					f[inIndex] = b->brdf[0]->get(outIndex, inIndex);
					inIndex++;
				}
			}

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				sh[i]->real = 0;
				sh[i]->imag = 0;
			}

			computeSphericalHarmonics(sc, f, sh);

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				brdfSampledSH[shIndex] = sh[i]->real;
				brdfSampledSH[shIndex + 1] = sh[i]->imag;
				shIndex = shIndex + 2;
			}
			

			outIndex++;
		}
	}



	inIndex = 0;
	outIndex = 0;
	for (int i = 0; i < b->n/2; i++)
	{
		float phi_out = i * 0.5 * M_PI / (b->n/2);
		for (int j = 0; j < 2 * b->n; j++)
		{
			inIndex = 0;
			float theta_out = j * 2.0 * M_PI / (2 * b->n);
			for (int k = 0; k < b->n/2; k++)
			{

				float phi_in = k * 0.5 * M_PI / (b->n/2);
				for (int l = 0; l < 4 * b->n/2; l++)
				{
					float theta_in = l * 2.0 * M_PI / (4 * b->n/2);

					sc[inIndex]->phi = phi_in;
					sc[inIndex]->theta = theta_in;
					f[inIndex] = b->brdf[1]->get(outIndex, inIndex);
					inIndex++;
				}
			}

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				sh[i]->real = 0;
				sh[i]->imag = 0;
			}

			computeSphericalHarmonics(sc, f, sh);

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				brdfSampledSH[shIndex] = sh[i]->real;
				brdfSampledSH[shIndex + 1] = sh[i]->imag;
				shIndex = shIndex + 2;
			}


			outIndex++;
		}
	}



	inIndex = 0;
	outIndex = 0;
	for (int i = 0; i < b->n / 2; i++)
	{
		float phi_out = i * 0.5 * M_PI / (b->n / 2);
		for (int j = 0; j < 4 * b->n/2; j++)
		{
			inIndex = 0;
			float theta_out = j * 2.0 * M_PI / (4 * b->n/2);
			for (int k = 0; k < b->n / 2; k++)
			{

				float phi_in = k * 0.5 * M_PI / (b->n / 2);
				for (int l = 0; l < 4 * b->n/2; l++)
				{
					float theta_in = l * 2.0 * M_PI / (4 * b->n/2);

					sc[inIndex]->phi = phi_in;
					sc[inIndex]->theta = theta_in;
					f[inIndex] = b->brdf[1]->get(outIndex, inIndex);
					inIndex++;
				}
			}

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				sh[i]->real = 0;
				sh[i]->imag = 0;
			}

			computeSphericalHarmonics(sc, f, sh);

			for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
			{
				brdfSampledSH[shIndex] = sh[i]->real;
				brdfSampledSH[shIndex + 1] = sh[i]->imag;
				shIndex = shIndex + 2;
			}

			outIndex++;
		}
	}
	this->shader()->addBufferTexture((void*)brdfSampledSH, brdfSampledSHSize, sizeof(float), "sampledBrdfSH", GL_RG32F, -1);

	delete brdfSampledSH;
	sh.clear();
	sc.clear();
	f.clear();
}

#define WAVELET_ENCODING_THRESHOLD 0.01
void Model_obj::addEnvironmentMap(std::vector<EnvironmentMap*>& environmentMap)
{
	if (environmentMap.size() > 0)
	{
		// number of verticies * 5 textures * 3 channels * texture size N *texture size M;
		int N = environmentMap[0]->map[0]->channel[0]->n;
		int M = environmentMap[0]->map[0]->channel[0]->m;
		
		int environmentIndexSize = 0;
		int environmentFaceIndexSize = environmentMap.size();
		int entironmentMapSize = 0;
		int index = 0;
		int totalSamplesPerMap = 0;
		int statingSample = 0;
		float w[3];
		float dir[3];

		std::vector<float> texColor;
		std::vector<int> texFaceIndex;
		std::vector<float> texDir;

		for (int vertIndex = 0; vertIndex < environmentMap.size(); vertIndex++)
		{
			totalSamplesPerMap = 0;
			EnvironmentMap* em = environmentMap[vertIndex];
			for (int imageIndex = 0; imageIndex < 5; imageIndex++)
			{
				Image* tex = em->map[imageIndex];
				index = 0;
				for (int x = 0; x < N; x++)
				{
					for (int y = 0; y < M; y++)
					{
						float channelSum = 0.0;
						for (int channelIndex = 0; channelIndex < 3; channelIndex++)
						{
							float f = tex->channel[channelIndex]->get(x, y);
							channelSum = channelSum + f;
						}
						if (channelSum >= WAVELET_ENCODING_THRESHOLD)
						{
							texColor.push_back(tex->channel[0]->get(x, y));
							texColor.push_back(tex->channel[1]->get(x, y));
							texColor.push_back(tex->channel[2]->get(x, y));

							if (imageIndex == 0)
							{
								w[0] = -1.0 + 2.0*(float)x / (float)N;
								w[1] = (float)y / (float)M;
								w[2] = 1.0;
							}
							else if (imageIndex == 1)
							{
								w[0] = 1.0;
								w[1] = (float)y / (float)N;
								w[2] = 1.0 - 2.0*(float)x / (float)M;
							}
							else if (imageIndex == 2)
							{
								w[0] = 1.0 - 2.0*(float)x / (float)N;
								w[1] = (float)y / (float)M;
								w[2] = -1.0;
							}
							else if (imageIndex == 3)
							{
								w[0] = -1.0;
								w[1] = (float)y / (float)N;
								w[2] = -1.0 + 2.0*(float)x / (float)M;
							}
							else if (imageIndex == 4)
							{
								w[0] = -1.0 + 2.0*(float)x / (float)N;
								w[1] = 1.0;
								w[2] = -1.0 + 2.0*(float)y / (float)M;
							}


							dir[0] = em->basisX[0] * w[0] + em->basisY[0] * w[1] + em->basisZ[0] * w[2];
							dir[1] = em->basisX[1] * w[0] + em->basisY[1] * w[1] + em->basisZ[1] * w[2];
							dir[2] = em->basisX[2] * w[0] + em->basisY[2] * w[1] + em->basisZ[2] * w[2];
							normalize(dir);

							//texIndex.push_back(index);
							texDir.push_back(dir[0]);
							texDir.push_back(dir[1]);
							texDir.push_back(dir[2]);

							totalSamplesPerMap++;
						}

						index++;
					}
				}


			}

			texFaceIndex.push_back(statingSample);
			texFaceIndex.push_back(totalSamplesPerMap);
			statingSample = statingSample + totalSamplesPerMap;

		}
		float* environmentMapTex = new float[texColor.size()];
		int* environmentMapFaceIndexTex = new int[texFaceIndex.size()];
		float* environmentMapIndexTex = new float[texDir.size()];
		
		for (int i = 0; i < texColor.size(); i++)
		{
			environmentMapTex[i] = texColor[i];
		}
		for (int i = 0; i < texFaceIndex.size(); i++)
		{
			environmentMapFaceIndexTex[i] = texFaceIndex[i];
		}
		for (int i = 0; i < texDir.size(); i++)
		{
			environmentMapIndexTex[i] = texDir[i];
		}

		__shader->addBufferTexture((void*)environmentMapTex, texColor.size(), sizeof(float), "environmentMap", GL_RGB32F, -1);
		__shader->addBufferTexture((void*)environmentMapIndexTex, texDir.size(), sizeof(float), "environmentMapIndex", GL_RGB32F, -1);
		__shader->addBufferTexture((void*)environmentMapFaceIndexTex, texFaceIndex.size(), sizeof(float), "environmentMapFaceIndex", GL_RG32I, -1);
		delete environmentMapTex;
		delete environmentMapIndexTex;
		delete environmentMapFaceIndexTex;
	}
}

void Model_obj::addFaceListTexture()
{
	int numFaces = this->faceList().size();
	float* faceTex = new float[numFaces * 3 * 4];

	int texIndex = 0;
	for (int i = 0; i < numFaces; i++)
	{		
		faceTex[texIndex] = this->faceList()[i]->vertex[0]->pos[0];
		faceTex[texIndex + 1] = this->faceList()[i]->vertex[0]->pos[1];
		faceTex[texIndex + 2] = this->faceList()[i]->vertex[0]->pos[2];
		faceTex[texIndex + 3] = 1.0;

		faceTex[texIndex + 4] = this->faceList()[i]->vertex[1]->pos[0];
		faceTex[texIndex + 5] = this->faceList()[i]->vertex[1]->pos[1];
		faceTex[texIndex + 6] = this->faceList()[i]->vertex[1]->pos[2];
		faceTex[texIndex + 7] = 1.0;

		faceTex[texIndex + 8] = this->faceList()[i]->vertex[2]->pos[0];
		faceTex[texIndex + 9] = this->faceList()[i]->vertex[2]->pos[1];
		faceTex[texIndex + 10] = this->faceList()[i]->vertex[2]->pos[2];
		faceTex[texIndex + 11] = 1.0;
		
		texIndex = texIndex + 12;
	}

	this->shader()->addBufferTexture((void*)faceTex, numFaces * 3 * 4, sizeof(float), "faceList", GL_RGBA32F, -1);
	this->shader()->addUniform(&numFaces, "numFaces", UNIFORM_INT_1, false);

	delete faceTex;
}

void Model_obj::addMaterialUniforms()
{
	__shader->addUniform((void*)(&this->brdfType), "brdfType", UNIFORM_INT_1, true);
	__shader->addUniform((void*)(&this->indexOfRefraction), "indexOfRefraction", UNIFORM_FLOAT_1, true);
	__shader->addUniform((void*)(&this->phongN), "specularN", UNIFORM_FLOAT_1, true);
	__shader->addUniform((void*)(&this->microfacetM), "microfacetM", UNIFORM_FLOAT_1, true);
	__shader->addUniform((void*)(&this->minnaertK), "minnaertBRDF_k", UNIFORM_FLOAT_1, true);
	__shader->addUniform((void*)(&this->absorption), "absorption", UNIFORM_FLOAT_3, true);
	__shader->addUniform((void*)(&this->reducedScattering), "reducedScattering", UNIFORM_FLOAT_3, true);
}
void Model_obj::addMatrixUniforms()
{
	__shader->addUniform((void*)this->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, true);
	__shader->addUniform((void*)getControllerInstance()->camera()->projectionMatrix()->data(), "projectionMatrix", UNIFORM_MAT_4, false);
	__shader->addUniform((void*)this->nm()->data(), "normalMatrix", UNIFORM_MAT_4, true);
	__shader->addUniform((void*)this->localMvm()->data(), "entityMatrix", UNIFORM_MAT_4, true);
	__shader->addUniform((void*)this->localNm()->data(), "entityNormalMatrix", UNIFORM_MAT_4, true);
	__shader->addUniform((void*)this->mvmInverse()->data(), "modelViewMatrixInverse", UNIFORM_MAT_4, true);
}