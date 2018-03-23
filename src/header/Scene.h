/**********************************************************************
* FILENAME :        Scene.h
*
* DESCRIPTION :
*       Manages, renders and updates all lights and entites in the scene.
*
* AUTHOR :    Greg Smith       START DATE :    Jan '13
*
**********************************************************************/

#ifndef SCENE_H
#define SCENE_H

#include "entities/Entity.h"
#include "Camera.h"
#include <stdio.h>
#include <vector>
#include "data structures/Point.h"
#include "entities/Model.h"
#include "shading utils/ShaderUtils.h"
#include "shader\RastShader.h"
#include "shader\AdsShader.h"
#include "entities/LightEntity.h"
#include "entities/BoundaryEntity.h"
#include "shader\GlobalIlluminationJacobiShader.h"
#include "data structures/RadianceCacheOctree.h"
#include "data structures/RadianceCacheUniformGrid.h"
#include "shader\RadianceMapShader.h"
#include "shader\MonteCarloBRDFShader.h"
#include "shader\ShadowMapShader.h"
#include "openGL/Renderbuffer.h"
#include "shading utils/PhotonMap.h"
#include "shader/PhotonMapShader.h"
#include "shader\SVDBrdfShader.h"
#include "shading utils/SVDBrdfLoader.h"
#include "shading utils/ProgressivePhotonMap.h"
#include "data structures/Image.h"
#include "shader\SHBRDFPhotonMapShader.h"
#include "shader\FilterBlurShader.h"
#include "shader\ShadowMapProjectionTextureShader.h"
#include "shader\SSSDiffuseShader.h"
#include "shader\PhotonMapProjectionTextureShader.h"
#include "shader\NormalDepthProjectionTextureShader.h"
#include "shader\PositionProjectionTextureShader.h"
#include "shader\NormalProjectionTextureShader.h"
#include "shader\ReflectiveShadowMapShader.h"
#include "shader\RadianceShadowMapShader.h"
#include "shader\InstantRadiosityShader.h"
#include "shader\StochasticRayTraceShader.h"
#include "shader\PointCloudShader.h"
#include <random>
//#include "openCL\RayTracerKernel.h"
#include "shader/IrradianceCachingShader.h"
#include "openCL/NNKernel.h"
#include "entities/PointCloudEntity.h"

class Scene;

class Scene
{
public:
	
	/*
	@function Scene(Camera* camera)
	@params: camera: pointer to the scene's camera instance
	
	Scene Constructor.
	*/
	Scene(Camera* camera);

	/*
	@function ~Scene()
	
	destructor routine.
	*/
	~Scene();

	/*
	@function updateScene()
	
	updateScene, invokes the update routines for each entity in the scene.
	*/
	void updateScene();


	/*
	@function render()
	
	render, invokes the render routines for each entity in the scene.
	*/
	void render();

	/*
	@function exportScene()
	@param const char* filePath

	exports the current scene to a scene file 'filePath'
	*/
	bool exportScene(const char* filePath);

	/*
	@function addEntity(Entity* e)
	@params Entity* e: entity to be added to the scene.
	
	adds entity e to the vector __entityList
	*/
	void addEntity(Entity* e);


	/*
	@function addLightEntity(LightEntity* e)
	@params LightEntity* e: LightEntity to be added to the scene.
	
	adds entity e to the vector __lightEntityList
	*/
	void addLightEntity(LightEntity* le);


	/*
	@function addBoundaryEntity(BoundaryEntity* e)
	@param BoundaryEntity* e: BoundaryEntity to be added to the scene.
	
	adds entity e to the vector __boundaryEntityList
	*/
	void addBoundaryEntity(BoundaryEntity* be);


	void addPointCloudEntity(PointCloudEntity* pce);

	/*
	@function detectCollisions()
	
	detects collisions between entites in the scene.
	*/
	void detectCollisions();

	/*
	@function mousePick()

	@param float x: normalized screen coordinate x.
	@param float y: normalized screen coordinate y.
	
	returns the first entity intersecting with the ray casted from the mouse point.
	*/
	Entity* mousePick(float x, float y);

	/*
	@function preRender()
	
	performs all calculations necessary for multi pass shading
	*/
	void preRender();

	/*
	function selectEntities()
	
	selects all entities that are highlighted.
	*/
	void selectEntities();

	/*
	@function clearSelection()
	
	clears all selections from entities.
	*/
	void clearSelection();


	/*
	@function getEntity(int index)
	@params int index: index of the entity in the list.

	returns the entity from __entityList and index 'index'
	*/
	Entity* getEntity(int index);


	/*
	@function getLightEntity(int index)
	@params int index: index of the light entity in the list.
	
	returns the light entity from __lightEntityList and index 'index'
	*/
	LightEntity* getLightEntity(int index);


	/*
	@function getBoundaryEntity(int index)
	@params int index: index of the boundary entity in the list.
	
	returns the boundary entity at index 'index'
	*/
	BoundaryEntity* getBoundaryEntity(int index);


	PointCloudEntity* getPointCloudEntity(int index);

	/*
	@function removeEntity(int index)
	@params: int index: index of the entity in the list.
	
	removes the entity in the scene at index 'index'
	*/
	bool removeEntity(int index);
	bool removeEntity(Entity* e);

	bool removePointCloudEntity(int index);
	bool removePointCloudEntity(PointCloudEntity* pce);

	/*
	@function getEntityIndex(Entity* e)
	@params: Entitiy* e
	
	gets the entity index pointed by 'e'.
	*/
	int getEntityIndex(Entity* e);

	int getPointCloudEntityIndex(PointCloudEntity* pce);
	
	template <typename T>
	Scene* copyScene();

	/*
	@function removeLightEntity(int index)
	@params: int index: index of the entity in the list.

	removes the light entity in the scene at index 'index'
	*/
	bool removeLightEntity(int index);
	bool removeLightEntity(Entity* e);
	int getLightEntityIndex(Entity* e);

	/*getters, returns the number of entities in the scene*/
	int numEntities();
	int numLightEntities();
	int numBoundaryEntities();
	int numPointCloudEntities();

	/*
		renders the scene to an image at 'renderDirectory'
	*/
	void renderOffline(int imageWidth, int imageHeight, std::string renderDirectory);
	void renderOfflineParallel(int imageWidth, int imageHeight, std::string renderDirectory);
	void renderIrradianceCache(float** nnWeights, int* weightSize, float** nnBiases, int* biasSize, int inputSize, int layer1Size, int layer2Size);

	float directRadiance(float* position, float* normal, MeshHierarchyKd* mh);
	void interpolateNormal(Face* face, float* hitPoint, float* normalResult);
	void computeFaceBasis(Face* face, float* normal, float* basisX, float* basisZ);
	std::vector<float> computeVertexRadiance();

	void addMeshHierarchyKdTexture();
	void addPointLightTexture();
	void addLightEntitiesToScene();
	void addFaceLightTextureToScene();
	void partitionBoundaryEntities();
	void computeSubsurfaceScatteringTextures();
	void addShadowMap();
	void addReflectiveShadowMap();
	std::vector<Texture_obj<GLfloat>*> computePhotonMapDiffuseTexture();
	void addEnvironmentMap(std::vector<Texture_obj<GLfloat>*>& environmentMap);
	std::vector<Texture_obj<GLfloat>*> computeRadianceTexture();
	void addIRShadowMap();

	/*computes a list of all vertices / faces in the scene in global coordinates*/
	std::vector<Face*> computeFaceList();
	std::vector<Face*> computeEntityFaceList();
	std::vector<MaterialFace*> computeEntityMaterialFaceList();
	std::vector<Face*> computeLightFaceList();
	std::vector<Vertex*> computeEntityVertexList();
	
	/*debug shader, displays all the photons in the scene*/
	RadianceMapShader* radianceMapShader;

	/*
	@function environmentMapVertexRadiance

	compute the incident radiance at each vertex in the scene, returned as a vector of 'VertexRadiance'
	*/
	std::vector<Texture_obj<GLfloat>*> environmentMapVertexRadiance();
	
	/*
	@function computePhotonMap

	creates a photon map using all entities in the scene, returned as a vector of photons.
	@param direct : 
	*/
	std::vector<PhotonMap*> computePhotonMap(bool direct, int numPhotons);
	
	/*
	@function photonMapVertexRadiance
	@param vector<PhotonMap*> &photonMap: computed photon map
	@param vector<VertecRadiance*> &vertexRadiance : returned per-vertex radiance.
	
	computes the spherical harmonics of the radiance for each vertex in the scene.
	*/
	void photonMapVertexRadiance(std::vector<PhotonMap*> photonMap, std::vector<VertexRadiance*> &vertexRadiance);

	/*
	@function photonMapVertexRadiance
	@param vector<PhotonMap*> &photonMap: computed photon map
	@param vector<float> &vertexRadiance : returned per-vertex radiance.

	computes the diffuse radiance for each vertex in the scene using photon mapping.
	*/
	void photonMapVertexRadiance(std::vector<PhotonMap*> photonMap, std::vector<VertexRadianceDiffuse*> &vertexRadiance);

	/*adds the photon map radiance to each entity in the scene*/
	void addPhotonMap(bool direct);

	/*scene shading type*/
	unsigned int shadingType;

	/*render buffer object*/
	Renderbuffer* renderbuffer;

	/*contains the photons sampled on a face. Upsed in photon mapping*/
	std::vector<HitPointFace*> photonHits;

	/*contains a per vertex diffuse irradiance*/
	std::vector<VertexRadiance*> vertexRadiance;

private:
	void buildBoundaryTreeTex();
	void addBoundingTreeToShaders();
	void randomizeDirection(float* result);
	void randomizeDirection(float* result, float* normal);
	void randomizePoint(float* v1, float* v2, float* v3, float* result);
	void sampleEnvironment(float* position, float* initialDirection, MeshHierarchyKd* mh, float* color);

	/*vector containing all the entites in the scene*/
	std::vector<Entity*> __entityList;

	/*vector containing all the light entites in the scene*/
	std::vector<LightEntity*> __lightEntityList;

	/*vector containing all the boundary entites in the scene*/
	std::vector<BoundaryEntity*> __boundaryEntityList;

	std::vector<PointCloudEntity*> __pointCloudEntityList;

	Camera* __camera;
	BoundingHierarchy* __boundingHierarchy;
	int __numTreeTexBoundaries;
	float* __boundaryTreeTexFloat;
	int __boundaryTreeTexFloatSize;
	int* __boundaryTreeTexInt;
	int __boundaryTreeTexIntSize;

	FilterBlurShader* filterBlurShader;
};

/*
@function importScene(const char* filepath, Camera* cam)

@params filePath export location of the scene
@param cam 
*/
Scene* importScene(const char* filePath, Camera* cam);

#endif