#include "Scene.h"
#include <time.h> 
#include <fstream>
#include "../header/LevelEditorGlobals.h"
#include "../header/window/LevelEditorWindow.h"

#define PHOTON_MAP_DEBUG 1
#define ENVIRONMENT_MAP_DEBUG 0

/*
@function Scene(Camera* camera)
@params: camera: pointer to the scene's camera instance

Scene Constructor.
*/
Scene::Scene(Camera* camera)
{
	__camera = camera;
	shadingType = SHADER_TYPE_RASTERIZE; 
	renderbuffer = new Renderbuffer(getControllerInstance()->screenWidth(), getControllerInstance()->screenHeight(),GL_UNSIGNED_BYTE);

#if POST_PROCESSING
	this->filterBlurShader = new FilterBlurShader(SCREEN_WIDTH,SCREEN_HEIGHT,"../bin/assets/textures/lpf.png");
	this->filterBlurShader->addTexture(renderbuffer->framebuffer->colorTex, "inputTex", -1);
#endif
}


/*
@function ~Scene()

destructor routine.
*/
Scene::~Scene()
{
	while (__entityList.size() > 0)
	{
		delete __entityList[__entityList.size() - 1];
		__entityList.pop_back();
	}

	while (__lightEntityList.size() > 0)
	{
		delete __lightEntityList[__lightEntityList.size() - 1];
		__lightEntityList.pop_back();
	}
	delete renderbuffer;
	delete filterBlurShader;
}


/*
@function updateScene()

updateScene, invokes the update routines for each entity in the scene.
*/
void Scene::updateScene()
{
	for(int i=0; i<__entityList.size(); i++)
	{
		__entityList[i]->update(__camera);
	}

	for (int i = 0; i<__lightEntityList.size(); i++)
	{
		__lightEntityList[i]->update(__camera);
	}
	
}


/*
@function detectCollisions()

detects collisions between entites in the scene.
*/
void Scene::detectCollisions()
{
	return;
	
}


/*
@function addEntity(Entity* e)
@params Entity* e: entity to be added to the scene.

adds entity e to the vector __entityList
*/
void Scene::addEntity(Entity* e)
{
	__entityList.push_back(e);
#ifdef _EDITOR
	((Model_obj*)e)->outlineShader->addMatrices(((Model_obj*)e)->mvm(), this->__camera->projectionMatrix());
#endif
}


/*
@function addLightEntity(LightEntity* e)
@params LightEntity* e: LightEntity to be added to the scene.

adds entity e to the vector __lightEntityList
*/
void Scene::addLightEntity(LightEntity* le)
{
	__lightEntityList.push_back(le);
#ifdef _EDITOR
	le->outlineShader->addMatrices(le->mvm(), this->__camera->projectionMatrix());
#endif
}


/*
@function addBoundaryEntity(BoundaryEntity* e)
@param BoundaryEntity* e: BoundaryEntity to be added to the scene.

adds entity e to the vector __boundaryEntityList
*/
void Scene::addBoundaryEntity(BoundaryEntity* be)
{
	__boundaryEntityList.push_back(be);
#ifdef _EDITOR
	be->outlineShader->addMatrices(be->mvm(), this->__camera->projectionMatrix());
#endif
}


/*
@function preRender()

performs all calculations necessary for multi pass shading
*/
void Scene::preRender()
{
	if (shadingType == SHADER_TYPE_WAVELET_ENVIRONMENT_MAP || 
		shadingType == SHADER_TYPE_SHADOW_MAP ||
		shadingType == SHADER_TYPE_SH_BRDF || 
		shadingType == SHADER_TYPE_SH_BRDF_PHOTON_MAP ||
		shadingType == SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE  ||
		shadingType == SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE
		)
	{
		addShadowMap();
	}

	if (shadingType == SHADER_TYPE_PHOTON_MAP)
	{
		addShadowMap();
	}


	if (shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE || shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
	{
		if (shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
			addPhotonMap(true);
		else if (shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE)
			addPhotonMap(false);
		

		for (int i = 0; i < numEntities(); i++)
		{
			Model_obj* m = (Model_obj*)getEntity(i);
			int transmittance;

			if (shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
				transmittance = 1;
			else if (shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE)
				transmittance = 0;

			m->shader()->addUniform(&transmittance, "transmittance", UNIFORM_INT_1, false);
		}
	}

	if (shadingType == SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE)
	{
		for (int i = 0; i < numEntities(); i++)
		{
			((Model_obj*)getEntity(i))->addFaceListTexture();
		}
	}

}


/*
@function render()

render, invokes the render routines for each entity in the scene.
*/
void Scene::render()
{
#if POST_PROCESSING
	this->renderbuffer->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDrawBuffer(GL_FRONT);
#endif

	const GLenum buffers[] = { GL_BACK, };
	glDrawBuffers(1, buffers);

	for(int i=0; i<__entityList.size(); i++)
	{
		__entityList[i]->render();
	}
#ifdef _EDITOR
	if ( (shadingType == SHADER_TYPE_SH_BRDF_PHOTON_MAP || this->shadingType == SHADER_TYPE_RADIANCE_CACHE_UNIFORM_GRID || this->shadingType == SHADER_TYPE_PHOTON_MAP) && radianceMapShader != NULL)
	{
#if PHOTON_MAP_DEBUG
		radianceMapShader->render();
#endif
	}
#endif

#ifdef _EDITOR
	for (int i = 0; i<__lightEntityList.size(); i++)
	{
		__lightEntityList[i]->render();
	}

	for (int i = 0; i < __boundaryEntityList.size(); i++)
	{
		__boundaryEntityList[i]->render();
	}
#endif


#if POST_PROCESSING
	glFinish();
	renderbuffer->unbindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	filterBlurShader->render();
	glFinish();
#endif
}

int Scene::numEntities()
{
	return __entityList.size();
}

int  Scene::numLightEntities()
{
	return __lightEntityList.size();
}

int Scene::numBoundaryEntities()
{
	return __boundaryEntityList.size();
}

Entity* Scene::getEntity(int index)
{
	if (index >= __entityList.size() || index < 0)
		return NULL;
	return __entityList[index];
}


/*
@function getLightEntity(int index)
@params int index: index of the light entity in the list.

returns the light entity from __lightEntityList and index 'index'
*/
LightEntity* Scene::getLightEntity(int index)
{
	if (index >= __lightEntityList.size() || index < 0)
		return NULL;
	return __lightEntityList[index];
}


/*
@function getBoundaryEntity(int index)
@params int index: index of the boundary entity in the list.

returns the boundary entity at index 'index'
*/
BoundaryEntity* Scene::getBoundaryEntity(int index)
{
	if (index >= __boundaryEntityList.size() || index < 0)
	{
		return NULL;
	}
	return __boundaryEntityList[index];
}


/*
@function mousePick()

@param float x: normalized screen coordinate x.
@param float y: normalized screen coordinate y.

returns the first entity intersecting with the ray casted from the mouse point.
*/
Entity* Scene::mousePick(float x, float y)
{
	Ray ray;
	float posGlobal[] = { 0.0, 0.0, 0.0, 1.0 };
	float dirGlobal[] = { 0.5*x/__camera->aspectRatio(),-y, __camera->nearPlane(),0.0f };
	normalize(dirGlobal);

	float posLocal[4];
	float dirLocal[4];

	RayHitInfo hitInfo;

	for (int i = 0; i < this->__entityList.size(); i++)
	{
		Model_obj* currentEntity = (Model_obj*)__entityList[i];

		currentEntity->mvmInverse()->multVec(posGlobal, posLocal);
		currentEntity->mvmInverse()->multVec(dirGlobal, dirLocal);

		//normalize(dirLocal);
		ray.dir.x = dirLocal[0];
		ray.dir.y = dirLocal[1];
		ray.dir.z = dirLocal[2];

		ray.pos.x = posLocal[0];
		ray.pos.y = posLocal[1];
		ray.pos.z = posLocal[2];

		if (intersect(&ray, currentEntity->bounds, &hitInfo))
		{
			currentEntity->highlighted = true;
		}
		else
		{
			currentEntity->highlighted = false;
		}
	}

	return NULL;
}


/*
function selectEntities()

selects all entities that are highlighted.
*/
void Scene::selectEntities()
{
	clearSelection();
	Model_obj* e;
	for (int i = 0; i < __entityList.size(); i++)
	{
		e = (Model_obj*)__entityList[i];
		if (e->highlighted)
		{
			e->selected = true;
			e->outlineShader->setHighlightedColour(1.0, 0.0, 0.0);
		}
	}
}


/*
@function clearSelection()

clears all selections from entities.
*/
void Scene::clearSelection()
{
	Model_obj* e;
	for (int i = 0; i < __entityList.size(); i++)
	{
		e = (Model_obj*)__entityList[i];
		e->selected = false;
		e->outlineShader->setHighlightedColour(1.0, 1.0, 1.0);
	}
}


/*
@function removeEntity(int index)
@params: int index: index of the entity in the list.

removes the entity in the scene at index 'index'
*/
bool Scene::removeEntity(int index)
{
	if (index < 0 || index >= __entityList.size())
	{
		return false;
	}
	else
	{
		for (int j = index; j < __entityList.size() - 1; j++)
		{
			__entityList[j] = __entityList[j + 1];
		}
		__entityList.pop_back();
	}
	return true;
}


/*
@function getEntity(Entity* e)
@params int index: index of the entity in the list.

returns the entity from __entityList pointed by 'e'
*/
bool Scene::removeEntity(Entity* e)
{
	int i = getEntityIndex(e);
	if( i == -1)
		return false;
	else
	{
		removeEntity(i);
	}
	return true;
}

/*
@function getEntity(Entity* e)
@params int index: index of the entity in the list.

returns the entity from __entityList pointed by 'e'
*/
int Scene::getEntityIndex(Entity* e)
{
	for (int i = 0; i < __entityList.size(); i++)
	{
		if (__entityList[i] == e)
			return i;
	}
	return -1;
}


/*
@function removeLightEntity(int index)
@params: int index: index of the entity in the list.

removes the light entity in the scene at index 'index'
*/
bool Scene::removeLightEntity(int index)
{
	if (index < 0 || index >= __lightEntityList.size())
	{
		return false;
	}
	else
	{
		for (int j = index; j < __lightEntityList.size() - 1; j++)
		{
			__lightEntityList[j] = __lightEntityList[j + 1];
		}
		__lightEntityList.pop_back();
	}
	return true;
}

bool Scene::removeLightEntity(Entity* e)
{
	int i = getLightEntityIndex(e);
	if (i == -1)
		return false;
	else
	{
		removeLightEntity(i);
	}
	return true;
}

/*
@function getLightEntity(Entity* e)
@params int index: index of the light entity in the list.

returns the light entity from __lightEntityList pointed by 'e'
*/
int Scene::getLightEntityIndex(Entity* e)
{
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		if (__lightEntityList[i] == e)
			return i;
	}
	return -1;
}

void Scene::addFaceLightTextureToScene()
{
	if (__lightEntityList.size() <= 0)
		return;
	std::vector<Face*> allFaces = computeLightFaceList();
	int totalLightSize = allFaces.size()*4*4;

	GLfloat* lighTex = new GLfloat[totalLightSize];

	int lIndex = 0;
	for (int i = 0; i < allFaces.size(); i++)
	{

			lighTex[lIndex] = allFaces[i]->vertex[0]->pos[0];
			lighTex[lIndex+1] = allFaces[i]->vertex[0]->pos[1];
			lighTex[lIndex+2] = allFaces[i]->vertex[0]->pos[2];
			lighTex[lIndex + 3] = 8768.0;

			lighTex[lIndex + 4] = allFaces[i]->vertex[1]->pos[0];
			lighTex[lIndex + 5] = allFaces[i]->vertex[1]->pos[1];
			lighTex[lIndex + 6] = allFaces[i]->vertex[1]->pos[2];
			lighTex[lIndex + 7] = 8768.0;

			lighTex[lIndex + 8] = allFaces[i]->vertex[2]->pos[0];
			lighTex[lIndex + 9] = allFaces[i]->vertex[2]->pos[1];
			lighTex[lIndex + 10] = allFaces[i]->vertex[2]->pos[2];
			lighTex[lIndex + 11] = 8768.0;

			lighTex[lIndex + 12] = allFaces[i]->normal[0]->pos[0];
			lighTex[lIndex + 13] = allFaces[i]->normal[0]->pos[1];
			lighTex[lIndex + 14] = allFaces[i]->normal[0]->pos[2];
			lighTex[lIndex + 15] = allFaces[i]->texCoord[0]->pos[0];

			lIndex = lIndex + 16;

	}
	GLint* numFacesPerLight = new GLint[__lightEntityList.size()*4];
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		numFacesPerLight[i*4] = (int)__lightEntityList[i]->faceList().size();
		numFacesPerLight[i * 4 + 1] = 0;
		numFacesPerLight[i * 4 + 2] = 0;
		numFacesPerLight[i * 4 + 3] = 0;
	}
	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)__entityList[i])->shader()->addBufferTexture(lighTex, totalLightSize, sizeof(GLfloat), "faceLightTexture", GL_RGBA32F, -1);
		((Model_obj*)__entityList[i])->shader()->addBufferTexture(numFacesPerLight, __lightEntityList.size() * 4, sizeof(GLint), "numFacesPerLight", GL_RGBA32I, -1);
	}
	allFaces.clear();
	delete lighTex;
	delete numFacesPerLight;
}

void Scene::addLightEntitiesToScene()
{
	if (__lightEntityList.size() <= 0)
		return;
	std::vector<Light*> l;
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		Light* light = new Light();
		LightEntity* le = __lightEntityList[i];

		light->lightPosition[0] = le->x();
		light->lightPosition[1] = le->y();
		light->lightPosition[2] = le->z();
		light->lightPosition[4] = 1;

		light->lightDirection[0] = le->x();
		light->lightDirection[1] = le->y();
		light->lightDirection[2] = le->z();
		light->lightDirection[4] = 1;

		light->intensity[0] = le->intensity;
		light->intensity[1] = le->intensity;
		light->intensity[2] = le->intensity;
		light->intensity[3] = le->intensity;

		l.push_back(light);
	}

	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)getEntity(i))->addLight(l);
		//((Model_obj*)getEntity(i))->addLight(le->globalPosition);
	}

	while(l.size() > 0)
	{
		Light* light = l[l.size() - 1];
		delete light;
		l.pop_back();
	}

}

void Scene::partitionBoundaryEntities()
{
	/*
	need to figure out:
	how the scene will be divided,
	need to add axis aligned boundaries,

	subdivide the scene spatially where the volume of
	each child is as equal as possible.

	Algorithm:
	
	sort the vector based on spatial coordinates x, y or z
	accumulate the total volume of the boundaries.
	divide the vector at the midpoint of the total volume.

	BoundingHierarchy
	{
		Bounds* b;	// total bounding volume of the children
		BoundingHierarchy* children[2];		// bounding children, NULL if leaf
		int sortOn;
	}

	*/

	__boundingHierarchy = new BoundingHierarchy(__boundaryEntityList,0);
	buildBoundaryTreeTex();
	addBoundingTreeToShaders();
}

void Scene::buildBoundaryTreeTex()
{
	std::vector<BoundaryTreeTex*> btt;
	__boundingHierarchy->toBoundaryTreeTex(btt);
	__numTreeTexBoundaries = btt.size();
	
	int bufferSizeInt = sizeof(BoundaryTreeTexInt);
	int bufferSizeFloat = sizeof(BoundaryTreeTexFloat);

	__boundaryTreeTexFloat = (float*)malloc(sizeof(BoundaryTreeTexFloat)*__numTreeTexBoundaries);
	__boundaryTreeTexInt = (int*)malloc(sizeof(BoundaryTreeTexInt)*__numTreeTexBoundaries);

	__boundaryTreeTexIntSize = bufferSizeInt/16;
	__boundaryTreeTexFloatSize = bufferSizeFloat/16;

	for (int i = 0; i < __numTreeTexBoundaries; i++)
	{
		int j = i*__boundaryTreeTexIntSize*4;
		__boundaryTreeTexInt[j] = btt[i]->index;
		__boundaryTreeTexInt[j + 1] = btt[i]->childIndex0;
		__boundaryTreeTexInt[j + 2] = btt[i]->childIndex1;
		__boundaryTreeTexInt[j + 3] = btt[i]->boundaryType;

		__boundaryTreeTexInt[j + 4] = btt[i]->sortOn;


		int k = i*__boundaryTreeTexFloatSize*4;
		__boundaryTreeTexFloat[k] = btt[i]->radius;
		__boundaryTreeTexFloat[k+1] = btt[i]->position[0];
		__boundaryTreeTexFloat[k+2] = btt[i]->position[1];
		__boundaryTreeTexFloat[k+3] = btt[i]->position[2];

		__boundaryTreeTexFloat[k + 4] = btt[i]->boundaryHigh[0];
		__boundaryTreeTexFloat[k+5] = btt[i]->boundaryHigh[1];
		__boundaryTreeTexFloat[k+6] = btt[i]->boundaryHigh[2];
		__boundaryTreeTexFloat[k+7] = btt[i]->splittingPlane;

		__boundaryTreeTexFloat[k + 8] = btt[i]->boundaryLow[0];
		__boundaryTreeTexFloat[k+9] = btt[i]->boundaryLow[1];
		__boundaryTreeTexFloat[k+10] = btt[i]->boundaryLow[2];
		__boundaryTreeTexFloat[k+11] = btt[i]->boundaryLow[3];
	}

}

void Scene::addBoundingTreeToShaders()
{
	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)__entityList[i])->addBoundaryTextures(__boundaryTreeTexFloat, __boundaryTreeTexFloatSize, __boundaryTreeTexInt, __boundaryTreeTexIntSize, __numTreeTexBoundaries);
	}
}

std::vector<MaterialFace*> Scene::computeEntityMaterialFaceList()
{
	std::vector<MaterialFace*> allFaces;
	Model_obj* currentEntity;
	for (int i = 0; i < __entityList.size(); i++)
	{
		currentEntity = (Model_obj*)__entityList[i];
		for (int j = 0; j < currentEntity->faceList().size(); j++)
		{
			Face* currentFace = currentEntity->faceList()[j];

			MaterialFace* f = new MaterialFace();
			GLfloat localPos[4];
			GLfloat result[4];

			for (int k = 0; k < 3; k++)
			{
				localPos[0] = currentFace->vertex[k]->pos[0];
				localPos[1] = currentFace->vertex[k]->pos[1];
				localPos[2] = currentFace->vertex[k]->pos[2];
				localPos[3] = 1.0;
				currentEntity->localMvm()->multVec(localPos, result);
				f->vertex[k] = new Vertex();
				f->vertex[k]->pos[0] = result[0];
				f->vertex[k]->pos[1] = result[1];
				f->vertex[k]->pos[2] = result[2];

				localPos[0] = currentFace->normal[k]->pos[0];
				localPos[1] = currentFace->normal[k]->pos[1];
				localPos[2] = currentFace->normal[k]->pos[2];
				localPos[3] = 0.0;
				currentEntity->nm()->multVec(localPos, result);
				normalize(result);
				f->normal[k] = new Vertex();
				f->normal[k]->pos[0] = result[0];
				f->normal[k]->pos[1] = result[1];
				f->normal[k]->pos[2] = result[2];
			}
			f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0]) / 3.0;
			f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1]) / 3.0;
			f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2]) / 3.0;
			
			//f->diffuseP = currentEntity->diffuseProbability;
			f->diffuseP = pow( (currentEntity->indexOfRefraction - 1.0)/(currentEntity->indexOfRefraction + 1.0), 2.0 );
			//f->specularP = currentEntity->specularProbability;
			f->specularP = 1.0 - f->diffuseP;
			f->indexOfRefraction = currentEntity->indexOfRefraction;
			allFaces.push_back(f);
		}
	}
	return allFaces;
}

std::vector<Vertex*> Scene::computeEntityVertexList()
{
	std::vector<Vertex*> vList;

	for (int i = 0; i < this->__entityList.size(); i++)
	{
		Model_obj* e = (Model_obj*)getEntity(i);
		for (int j = 0; j < e->vertexList().size(); j++)
		{
			Vertex* v = e->vertexList()[j];
			Vertex* vGlobal = new Vertex();
			float globalPos[4];
			float localPos[] = { v->pos[0], v->pos[1], v->pos[2], 1.0 };
			e->localMvm()->multVec(localPos, globalPos);
			vGlobal->pos[0] = globalPos[0];
			vGlobal->pos[1] = globalPos[1];
			vGlobal->pos[2] = globalPos[2];
			vList.push_back(vGlobal);
		}
		
	}
	return vList;
}

std::vector<Face*> Scene::computeEntityFaceList()
{
	std::vector<Face*> allFaces;
	Model_obj* currentEntity;
	for (int i = 0; i < __entityList.size(); i++)
	{
		currentEntity = (Model_obj*)__entityList[i];
		for (int j = 0; j < currentEntity->faceList().size(); j++)
		{
			Face* currentFace = currentEntity->faceList()[j];

			Face* f = new Face();
			GLfloat localPos[4];
			GLfloat result[4];

			for (int k = 0; k < 3; k++)
			{
				localPos[0] = currentFace->vertex[k]->pos[0];
				localPos[1] = currentFace->vertex[k]->pos[1];
				localPos[2] = currentFace->vertex[k]->pos[2];
				localPos[3] = 1.0;
				currentEntity->localMvm()->multVec(localPos, result);
				f->vertex[k] = new Vertex();
				f->vertex[k]->pos[0] = result[0];
				f->vertex[k]->pos[1] = result[1];
				f->vertex[k]->pos[2] = result[2];

				localPos[0] = currentFace->normal[k]->pos[0];
				localPos[1] = currentFace->normal[k]->pos[1];
				localPos[2] = currentFace->normal[k]->pos[2];
				localPos[3] = 0.0;
				currentEntity->localNm()->multVec(localPos, result);
				normalize(result);
				f->normal[k] = new Vertex();
				f->normal[k]->pos[0] = result[0];
				f->normal[k]->pos[1] = result[1];
				f->normal[k]->pos[2] = result[2];

				if (currentFace->texCoord[k] != NULL)
				{
					f->texCoord[k] = new TexCoord();
					f->texCoord[k]->pos[0] = currentFace->texCoord[k]->pos[0];
					f->texCoord[k]->pos[1] = currentFace->texCoord[k]->pos[1];
				}
				f->vertIndex[0] = currentFace->vertIndex[0];
				f->vertIndex[1] = currentFace->vertIndex[1];
				f->vertIndex[2] = currentFace->vertIndex[2];

				f->normalIndex[0] = currentFace->normalIndex[0];
				f->normalIndex[1] = currentFace->normalIndex[1];
				f->normalIndex[2] = currentFace->normalIndex[2];

				f->texCoordIndex[0] = currentFace->texCoordIndex[0];
				f->texCoordIndex[1] = currentFace->texCoordIndex[1];
				f->texCoordIndex[2] = currentFace->texCoordIndex[2];
			}
			f->index = currentFace->index;

			f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0]) / 3.0;
			f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1]) / 3.0;
			f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2]) / 3.0;
			allFaces.push_back(f);
		}
	}
	return allFaces;
}

std::vector<Face*> Scene::computeLightFaceList()
{
	std::vector<Face*> allFaces;
	LightEntity* currentEntity;
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		currentEntity = (LightEntity*)__lightEntityList[i];
		for (int j = 0; j < currentEntity->faceList().size(); j++)
		{
			Face* currentFace = currentEntity->faceList()[j];

			Face* f = new Face();
			GLfloat localPos[4];
			GLfloat result[4];

			for (int k = 0; k < 3; k++)
			{
				localPos[0] = currentFace->vertex[k]->pos[0];
				localPos[1] = currentFace->vertex[k]->pos[1];
				localPos[2] = currentFace->vertex[k]->pos[2];
				localPos[3] = 1.0;
				currentEntity->localMvm()->multVec(localPos, result);
				f->vertex[k] = new Vertex();
				f->vertex[k]->pos[0] = result[0];
				f->vertex[k]->pos[1] = result[1];
				f->vertex[k]->pos[2] = result[2];

				localPos[0] = currentFace->normal[k]->pos[0];
				localPos[1] = currentFace->normal[k]->pos[1];
				localPos[2] = currentFace->normal[k]->pos[2];
				localPos[3] = 0.0;
				currentEntity->localNm()->multVec(localPos, result);
				normalize(result);
				f->normal[k] = new Vertex();
				f->normal[k]->pos[0] = result[0];
				f->normal[k]->pos[1] = result[1];
				f->normal[k]->pos[2] = result[2];

				f->texCoord[k] = new TexCoord();
				f->texCoord[k]->pos[0] = currentEntity->intensity;
			}
			f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0]) / 3.0;
			f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1]) / 3.0;
			f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2]) / 3.0;

			f->materialIndex = LIGHT_MATERIAL_INDEX;

			allFaces.push_back(f);
		}
	}
	return allFaces;
}

std::vector<Face*> Scene::computeFaceList()
{
	std::vector<Face*> allFaces;
	Model_obj* currentEntity;
	for (int i = 0; i < __entityList.size(); i++)
	{
		currentEntity = (Model_obj*)__entityList[i];
		SquareMatrix mNorm(4);
		currentEntity->localMvm()->inverse(&mNorm);
		mNorm.transpose();
		for (int j = 0; j < currentEntity->faceList().size(); j++)
		{
			Face* currentFace = currentEntity->faceList()[j];
			Face* f = new Face();

			GLfloat localPos[4];
			GLfloat result[4];

			for (int k = 0; k < 3; k++)
			{
				localPos[0] = currentFace->vertex[k]->pos[0];
				localPos[1] = currentFace->vertex[k]->pos[1];
				localPos[2] = currentFace->vertex[k]->pos[2];
				localPos[3] = 1.0;
				currentEntity->localMvm()->multVec(localPos, result);
				f->vertex[k] = new Vertex();
				f->vertex[k]->pos[0] = result[0];
				f->vertex[k]->pos[1] = result[1];
				f->vertex[k]->pos[2] = result[2];

				localPos[0] = currentFace->normal[k]->pos[0];
				localPos[1] = currentFace->normal[k]->pos[1];
				localPos[2] = currentFace->normal[k]->pos[2];
				localPos[3] = 0.0;

				mNorm.multVec(localPos, result);
				normalize(result);
				f->normal[k] = new Vertex();
				f->normal[k]->pos[0] = result[0];
				f->normal[k]->pos[1] = result[1];
				f->normal[k]->pos[2] = result[2];

				f->texCoord[0] = currentFace->texCoord[0];
				f->texCoord[1] = currentFace->texCoord[1];

				f->vertIndex[0] = currentFace->vertIndex[0];
				f->vertIndex[1] = currentFace->vertIndex[1];
				f->vertIndex[2] = currentFace->vertIndex[2];

				f->normalIndex[0] = currentFace->normalIndex[0];
				f->normalIndex[1] = currentFace->normalIndex[1];
				f->normalIndex[2] = currentFace->normalIndex[2];

				f->texCoordIndex[0] = currentFace->texCoordIndex[0];
				f->texCoordIndex[1] = currentFace->texCoordIndex[1];
				f->texCoordIndex[2] = currentFace->texCoordIndex[2];
			}
			f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0]) / 3.0;
			f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1]) / 3.0;
			f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2]) / 3.0;
			allFaces.push_back(f);
		}
	}
	Model_obj* currentLightEntity;
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		currentLightEntity = (Model_obj*)__lightEntityList[i];
		SquareMatrix mNorm(4);
		currentLightEntity->localMvm()->inverse(&mNorm);
		mNorm.transpose();
		for (int j = 0; j < currentLightEntity->faceList().size(); j++)
		{
			Face* currentFace = currentLightEntity->faceList()[j];
			Face* f = new Face();

			GLfloat localPos[4];
			GLfloat result[4];

			for (int k = 0; k < 3; k++)
			{
				localPos[0] = currentFace->vertex[k]->pos[0];
				localPos[1] = currentFace->vertex[k]->pos[1];
				localPos[2] = currentFace->vertex[k]->pos[2];
				localPos[3] = 1.0;
				currentLightEntity->localMvm()->multVec(localPos, result);
				f->vertex[k] = new Vertex();
				f->vertex[k]->pos[0] = result[0];
				f->vertex[k]->pos[1] = result[1];
				f->vertex[k]->pos[2] = result[2];

				localPos[0] = currentFace->normal[k]->pos[0];
				localPos[1] = currentFace->normal[k]->pos[1];
				localPos[2] = currentFace->normal[k]->pos[2];
				localPos[3] = 0.0;
				mNorm.multVec(localPos, result);

				normalize(result);
				f->normal[k] = new Vertex();
				f->normal[k]->pos[0] = result[0];
				f->normal[k]->pos[1] = result[1];
				f->normal[k]->pos[2] = result[2];

				f->texCoord[0] = currentFace->texCoord[0];
				f->texCoord[1] = currentFace->texCoord[1];

				f->vertIndex[0] = currentFace->vertIndex[0];
				f->vertIndex[1] = currentFace->vertIndex[1];
				f->vertIndex[2] = currentFace->vertIndex[2];

				f->normalIndex[0] = currentFace->normalIndex[0];
				f->normalIndex[1] = currentFace->normalIndex[1];
				f->normalIndex[2] = currentFace->normalIndex[2];

				f->texCoordIndex[0] = currentFace->texCoordIndex[0];
				f->texCoordIndex[1] = currentFace->texCoordIndex[1];
				f->texCoordIndex[2] = currentFace->texCoordIndex[2];
			}
			f->avg[0] = (f->vertex[0]->pos[0] + f->vertex[1]->pos[0] + f->vertex[2]->pos[0]) / 3.0;
			f->avg[1] = (f->vertex[0]->pos[1] + f->vertex[1]->pos[1] + f->vertex[2]->pos[1]) / 3.0;
			f->avg[2] = (f->vertex[0]->pos[2] + f->vertex[1]->pos[2] + f->vertex[2]->pos[2]) / 3.0;
			f->materialIndex = LIGHT_MATERIAL_INDEX;
			allFaces.push_back(f);
		}

	}
	return allFaces;
}

void Scene::randomizeDirection(float* result)
{
	do
	{
		result[0] = 2.0*(float)rand() / RAND_MAX - 1.0;
		result[1] = 2.0*(float)rand() / RAND_MAX - 1.0;
		result[2] = 2.0*(float)rand() / RAND_MAX - 1.0;
	} while (length(result) > 1 && ((result[0] + result[1] + result[2]) == 0.0));
	normalize(result);
}
void Scene::randomizeDirection(float* result, float* normal)
{
	do
	{
		result[0] = 2.0*(float)rand() / RAND_MAX - 1;
		result[1] = 2.0*(float)rand() / RAND_MAX - 1;
		result[2] = 2.0*(float)rand() / RAND_MAX - 1;
		
	} while ((result[0] + result[1] + result[2]) == 0.0 && dotProduct(result, normal) < 0.0 && length(result) > 1);
	normalize(result);
}

void Scene::randomizePoint(float* v1, float* v2, float* v3, float* result)
{
	float r[3];
	do
	{
		r[0] = (float)rand() / RAND_MAX;
		r[1] = (float)rand() / RAND_MAX;
		r[2] = (float)rand() / RAND_MAX;
	} while (r[0] + r[1] + r[2] == 0.0);
	float w = r[0] + r[1] + r[2];
	r[0] = r[0] / w;
	r[1] = r[1] / w;
	r[2] = r[2] / w;
	result[0] = r[0] * v1[0] + r[1] * v2[0] + r[2] * v3[0];
	result[1] = r[0] * v1[1] + r[1] * v2[1] + r[2] * v3[1];
	result[2] = r[0] * v1[2] + r[1] * v2[2] + r[2] * v3[2];
}

#define NUM_PHOTONS 500000
#define MAX_PHOTON_BOUNCES 6
#define PHOTON_INTENSITY_SCALE 3*NUM_PHOTONS
/*
@function computePhotonMap

creates a photon map using all entities in the scene, returned as a vector of photons.
*/
std::vector<PhotonMap*> Scene::computePhotonMap(bool direct, int numPhotons)
{
	std::vector<PhotonMap*> pm;
	std::vector<MaterialFace*> allFaces = computeEntityMaterialFaceList();
	std::vector<Face*> allFacesCopy;
	for (int i = 0; i < allFaces.size(); i++)
	{
		allFacesCopy.push_back((Face*)allFaces[i]);
	}
	MeshHierarchyKd* mh = new MeshHierarchyKd(allFacesCopy, allFaces.size());
	srand(time(NULL));
	Ray r;
	Face* lastHitFace = NULL;
	float photonPower = 1.0f;
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		FrustumLight* le = (FrustumLight*)__lightEntityList[i];
		for (int j = 0; j < numPhotons; j++)
		{
			photonPower = 1.0f;
			lastHitFace = NULL;
			r.pos.x = __lightEntityList[i]->x();
			r.pos.y = __lightEntityList[i]->y();
			r.pos.z = __lightEntityList[i]->z();

			float randomDir[3];
			float basisX[3];
			float basisZ[3];
			float basisY[3];
			basisY[0] = le->mvm()->get(0, 2);
			basisY[1] = le->mvm()->get(1, 2);
			basisY[2] = le->mvm()->get(2, 2);
			normalize(basisY);
			getBasis(basisY, basisX, basisZ);
			float planeW = 1.0;
			randomDir[0] = 1.41*((2.0*planeW*(float)rand() / RAND_MAX) - planeW);
			randomDir[1] = 1.41*((2.0*planeW*(float)rand() / RAND_MAX) - planeW);
			randomDir[2] = -1.0;
			normalize(randomDir);

			randomDir[0] = randomDir[0] * (basisX[0] + basisY[0] + basisZ[0]);
			randomDir[1] = randomDir[1] * (basisX[1] + basisY[1] + basisZ[1]);
			randomDir[2] = randomDir[2] * (basisX[2] + basisY[2] + basisZ[2]);

			for (int k = 0; k <= MAX_PHOTON_BOUNCES; k++)
			{

				Face* hitFace = NULL;
				RayHitInfo rh;
				r.dir.x = randomDir[0];
				r.dir.y = randomDir[1];
				r.dir.z = randomDir[2];
				rh.tmin = 0x7FFFFFFF;

				rh.ray.pos.x = r.pos.x;
				rh.ray.pos.y = r.pos.y;
				rh.ray.pos.z = r.pos.z;
				rh.ray.dir.x = r.dir.x;
				rh.ray.dir.y = r.dir.y;
				rh.ray.dir.z = r.dir.z;

				mh->rayTrace(&r, &rh, &hitFace);
				if (hitFace != NULL && hitFace != lastHitFace)
				{
					/*randomize, if hitFace is 
					diffuse: record and randomize
					specular: randomize
					absorption: break.
					*/

					lastHitFace = hitFace;
					float randMaterial = (float)rand() / RAND_MAX;
					if (randMaterial < ((MaterialFace*)hitFace)->diffuseP)
					{
						float n1[] = { (hitFace->normal[0]->pos[0] + hitFace->normal[1]->pos[0] + hitFace->normal[2]->pos[0]) / 3.0,
							(hitFace->normal[0]->pos[1] + hitFace->normal[1]->pos[1] + hitFace->normal[2]->pos[1]) / 3.0,
							(hitFace->normal[0]->pos[2] + hitFace->normal[1]->pos[2] + hitFace->normal[2]->pos[2]) / 3.0 };
						normalize(n1);

						if (k != 0 || direct)	// only record indirect lighting	
						{
							PhotonMap* photonMap = new PhotonMap();
							photonMap->pos[0] = rh.min.x;
							photonMap->pos[1] = rh.min.y;
							photonMap->pos[2] = rh.min.z;
							photonMap->normal[0] = n1[0];
							photonMap->normal[1] = n1[1];
							photonMap->normal[2] = n1[2];
							photonMap->incidentDirection[0] = r.dir.x;
							photonMap->incidentDirection[1] = r.dir.y;
							photonMap->incidentDirection[2] = r.dir.z;
							float negDir[] = {-r.dir.x , -r.dir.y, -r.dir.z };
							photonPower = photonPower*dotProduct(negDir, photonMap->normal);
							photonMap->power = photonPower;
							photonMap->index = pm.size();
							pm.push_back(photonMap);
						}
						photonPower = photonPower*((MaterialFace*)hitFace)->diffuseP;
						r.pos.x = rh.min.x + n1[0] * 0.001;
						r.pos.y = rh.min.y + n1[1] * 0.001;
						r.pos.z = rh.min.z + n1[2] * 0.001;

						randomizeDirection(randomDir, n1);
					}

					else if (randMaterial < ((MaterialFace*)hitFace)->diffuseP + ((MaterialFace*)hitFace)->specularP)
					{
						float n1[] = { (hitFace->normal[0]->pos[0] + hitFace->normal[1]->pos[0] + hitFace->normal[2]->pos[0]) / 3.0,
							(hitFace->normal[0]->pos[1] + hitFace->normal[1]->pos[1] + hitFace->normal[2]->pos[1]) / 3.0,
							(hitFace->normal[0]->pos[2] + hitFace->normal[1]->pos[2] + hitFace->normal[2]->pos[2]) / 3.0 };
						normalize(n1);

						if (k != 0 || direct)
						{
							PhotonMap* photonMap = new PhotonMap();
							photonMap->pos[0] = rh.min.x;
							photonMap->pos[1] = rh.min.y;
							photonMap->pos[2] = rh.min.z;
							photonMap->normal[0] = n1[0];
							photonMap->normal[1] = n1[1];
							photonMap->normal[2] = n1[2];
							photonMap->incidentDirection[0] = r.dir.x;
							photonMap->incidentDirection[1] = r.dir.y;
							photonMap->incidentDirection[2] = r.dir.z;
							float negDir[] = { -r.dir.x , -r.dir.y, -r.dir.z };
							photonPower = photonPower*dotProduct(negDir, photonMap->normal);
							photonMap->power = photonPower;
							photonMap->index = pm.size();
							pm.push_back(photonMap);
						}

						r.pos.x = rh.min.x + n1[0] * 0.001;
						r.pos.y = rh.min.y + n1[1] * 0.001;
						r.pos.z = rh.min.z + n1[2] * 0.001;
						float rOut[3];
						float rIn[] = { -r.dir.x, -r.dir.y, -r.dir.z };
						rOut[0] = 2 * n1[0] * dotProduct(n1, rIn) - rIn[0];
						rOut[1] = 2 * n1[1] * dotProduct(n1, rIn) - rIn[1];
						rOut[2] = 2 * n1[2] * dotProduct(n1, rIn) - rIn[2];
						normalize(rOut);
						r.dir.x = rOut[0];
						r.dir.y = rOut[1];
						r.dir.z = rOut[2];



						photonPower = photonPower*((MaterialFace*)hitFace)->specularP;
					}

					/*ELSE ABSORPTION*/
					/*
					else
					{
						if (k != 0)	// only record indirect lighting	
						{
							PhotonMap* photonMap = new PhotonMap();
							photonMap->pos[0] = rh.min.x;
							photonMap->pos[1] = rh.min.y;
							photonMap->pos[2] = rh.min.z;

							photonMap->incidentDirection[0] = r.dir.x;
							photonMap->incidentDirection[1] = r.dir.y;
							photonMap->incidentDirection[2] = r.dir.z;

							photonMap->power = (float)PHOTON_INTENSITY_SCALE / (float)NUM_PHOTONS;
							photonMap->power = photonMap->power*pow(0.5, k);
							photonMap->index = pm.size();

							pm.push_back(photonMap);
						}
						break;
					}
					*/

				}
				else
				{
					break;
				}
			}
		}
	}
	delete mh;
	allFaces.clear();
	allFacesCopy.clear();
	return pm;
}

#define K_NEAREST_PHOTON_MAPS 300
#define K_MAX_PHOTONS 1000
/*
@function photonMapVertexRadiance
@param vector<PhotonMap*> &photonMap: computed photon map
@param vector<VertecRadiance*> &vertexRadiance : returned per-vertex radiance.

computes the spherical harmonics of the radiance for each vertex in the scene.
*/
void Scene::photonMapVertexRadiance(std::vector<PhotonMap*> photonMap, std::vector<VertexRadiance*> &vertexRadiance)
{
	/*build the matrix of photons*/
	cv::Mat photonMat(photonMap.size(), 3, CV_32F);

	for (int i = 0; i < photonMap.size(); i++)
	{
		photonMat.at<float>(i, 0) = photonMap[i]->pos[0];
		photonMat.at<float>(i, 1) = photonMap[i]->pos[1];
		photonMat.at<float>(i, 2) = photonMap[i]->pos[2];
	}
	cv::flann::Index photonMapTree(photonMat, cv::flann::KDTreeIndexParams(1), cvflann::FLANN_DIST_EUCLIDEAN);

	for (int i = 0; i < vertexRadiance.size(); i++)
	{
		VertexRadiance* vr = vertexRadiance[i];
		cv::Mat ind;
		cv::Mat dist;
		cv::Mat vertMat(1, 3, CV_32F);
		vertMat.at<float>(0, 0) = vr->pos[0];
		vertMat.at<float>(0, 1) = vr->pos[1];
		vertMat.at<float>(0, 2) = vr->pos[2];
		float radius = vr->radius;

		int photonsRetuned = photonMapTree.radiusSearch(vertMat, ind, dist, vr->radius, K_MAX_PHOTONS);
		float norm[3];
		norm[0] = vr->normal.pos[0];
		norm[1] = vr->normal.pos[1];
		norm[2] = vr->normal.pos[2];

		int j = 0;
		float n = (float)vertexRadiance[i]->lastNumPhotons;
		float m = (float)photonsRetuned;
		std::vector<PhotonHit*> photonHit;
		for (j = 0; j < photonsRetuned; j++)
		{
			int photonIndex = ind.at<int>(0, j);
			float tempPos[3];
			PhotonMap* p = photonMap[photonIndex];
			tempPos[0] = p->pos[0];
			tempPos[1] = p->pos[1];
			tempPos[2] = p->pos[2];
			float tempRad = p->power;
			float tempDir[3];
			tempDir[0] = -p->incidentDirection[0];
			tempDir[1] = -p->incidentDirection[1];
			tempDir[2] = -p->incidentDirection[2];
			float visibility = maximum(dotProduct(norm, tempDir), 0.0);
			//if (visibility > 0.0)
			{
				PhotonHit* ph = new PhotonHit();
				SphericalCoords sc;
				toSphericalCoords(norm, tempDir, &sc);
				ph->phi = sc.phi;
				ph->theta = sc.theta;
				float photonPower = p->power;

				photonPower = photonPower*((n + ALHPA_PROGRESSIVE_PHOTON_MAP*m) / (n + m));
				ph->color[0] = photonPower;
				ph->color[1] = photonPower;
				ph->color[2] = photonPower;
				photonHit.push_back(ph);
			}
			
		}
		if ((n + m) != 0.0)
		{
			float radiusNext = radius*sqrt((n + ALHPA_PROGRESSIVE_PHOTON_MAP*m) / (n + m));
			vertexRadiance[i]->radius = radiusNext;
		}
		vertexRadiance[i]->lastNumPhotons = photonsRetuned;
		computeSphericalHarmonics(photonHit, vertexRadiance[i]->shIrradiance);
	}

}


/*
@function photonMapVertexRadiance
@param vector<PhotonMap*> &photonMap: computed photon map
@param vector<float> &vertexRadiance : returned per-vertex radiance.

computes the diffuse radiance for each vertex in the scene using photon mapping.
*/
void Scene::photonMapVertexRadiance(std::vector<PhotonMap*> photonMap, std::vector<VertexRadianceDiffuse*> &vertexRadiance)
{
	/*build the matrix of photons*/
	cv::Mat photonMat(photonMap.size(), 3, CV_32F);

	for (int i = 0; i < photonMap.size(); i++)
	{
		photonMat.at<float>(i, 0) = photonMap[i]->pos[0];
		photonMat.at<float>(i, 1) = photonMap[i]->pos[1];
		photonMat.at<float>(i, 2) = photonMap[i]->pos[2];
	}
	cv::flann::Index photonMapTree(photonMat, cv::flann::KDTreeIndexParams(1), cvflann::FLANN_DIST_EUCLIDEAN);

	for (int i = 0; i < vertexRadiance.size(); i++)
	{
		VertexRadianceDiffuse* vr = vertexRadiance[i];
		cv::Mat ind;
		cv::Mat dist;
		cv::Mat vertMat(1, 3, CV_32F);
		vertMat.at<float>(0, 0) = vr->pos[0];
		vertMat.at<float>(0, 1) = vr->pos[1];
		vertMat.at<float>(0, 2) = vr->pos[2];
		float radius = vr->radius;
		float radiance = 0.0;
		int photonsRetuned = photonMapTree.radiusSearch(vertMat, ind, dist, radius, K_MAX_PHOTONS);

		int j = 0;
		float n = (float)vertexRadiance[i]->lastNumPhotons;
		float m = (float)photonsRetuned;
		
		for (j = 0; j < photonsRetuned; j++)
		{
			int photonIndex = ind.at<int>(0, j);
			PhotonMap* p = photonMap[photonIndex];
			float tempRad = p->power;
			radiance = radiance + tempRad;
		}
		radiance = radiance / (M_PI*radius*radius);
		vr->radiance = radiance;
		if ((n + m) != 0.0)
		{
			float radiusNext = radius*sqrt((n + ALHPA_PROGRESSIVE_PHOTON_MAP*m) / (n + m));
			vertexRadiance[i]->radius = radiusNext;
		}
		vertexRadiance[i]->lastNumPhotons = photonsRetuned;
	}
}

#define PHOTON_MAP_CLUSTERS 1

/*adds the photon map radiance to each entity in the scene*/
void Scene::addPhotonMap(bool direct)
{
	std::vector<PhotonMap*> photonMap = computePhotonMap(direct, NUM_PHOTONS);
	
	if (photonMap.size() <= 0)
		return;

	radianceMapShader = new RadianceMapShader(photonMap);
	
#if PHOTON_MAP_CLUSTERS
	void* photonMapClusterFloat;
	void* photonMapClusterInt;
	void* photonMapTexture;
	createPhotonMapClusterTexture(photonMap, photonMapClusterFloat, photonMapClusterInt, photonMapTexture);
#else
	PhotonMapKdTree* pmkdt = new PhotonMapKdTree(photonMap);
	void* ti = NULL;
	void* tf = NULL;
	int tiSize, tfSize;
	createPhotonMapKdTexture(pmkdt, ti, &tiSize, tf, &tfSize);
	void* pmTex = createPhotonMapTexture(photonMap);

	int photonMapKdFloatSize = sizeof(PhotonMapKdTextureFloat) / (4 * 4);
	int photonMapKdIntSize = sizeof(PhotonMapKdTextureInt) / (4 * 4);
#endif

	int photonMapDataSize = sizeof(PhotonMapTexture) / (4 * 4);
	int clusterIntSize = sizeof(PhotonMapClusterInt) / (4 * 4);
	int clusterFloatSize = sizeof(PhotonMapClusterFloat) / (4 * 4);

	int numPhotons = photonMap.size();

	float nearPlane = getControllerInstance()->camera()->nearPlane();
	float farPlane = getControllerInstance()->camera()->farPlane();

	for (int i = 0; i < numEntities(); i++)
	{
		Model_obj* m = (Model_obj*)getEntity(i);
		m->shader()->addUniform(&numPhotons, "numPhotons", UNIFORM_INT_1, false);

#if PHOTON_MAP_CLUSTERS
		int numClusters = K_PHOTON_MAP_CLUSTERS;
		m->shader()->addUniform(&clusterFloatSize, "photonClusterFloatDataSize", UNIFORM_INT_1, false);
		m->shader()->addUniform(&clusterIntSize, "photonClusterIntDataSize", UNIFORM_INT_1, false);
		m->shader()->addBufferTexture(photonMapClusterFloat, K_PHOTON_MAP_CLUSTERS, sizeof(PhotonMapClusterFloat), "photonClusterFloat", GL_RGBA32F, -1);
		m->shader()->addBufferTexture(photonMapClusterInt, K_PHOTON_MAP_CLUSTERS, sizeof(PhotonMapClusterInt), "photonClusterInt", GL_RGBA32I, -1);
		m->shader()->addBufferTexture(photonMapTexture, numPhotons, sizeof(PhotonMapTexture), "photonList", GL_RGBA32F, -1);
		m->shader()->addUniform(&numClusters, "numPhotonClusters", UNIFORM_INT_1, false);
#else
		m->shader()->addUniform(&photonMapKdFloatSize, "treeTexutreFloatDataSize", UNIFORM_INT_1, false);
		m->shader()->addUniform(&photonMapKdIntSize, "treeTexutreIntDataSize", UNIFORM_INT_1, false);

		m->shader()->addBufferTexture(pmTex, numPhotons, sizeof(PhotonMapTexture), "photonList", GL_RGBA32F, -1);
		m->shader()->addBufferTexture(ti, tiSize, sizeof(PhotonMapKdTextureInt), "treeTextureInt", GL_RGBA32I, -1);
		m->shader()->addBufferTexture(tf, tfSize, sizeof(PhotonMapKdTextureFloat), "treeTextureFloat", GL_RGBA32F, -1);
#endif
		m->shader()->addUniform(&photonMapDataSize, "photonDataSize", UNIFORM_INT_1, false);

		//m->shader()->addUniform(&nearPlane, "nearPlane", UNIFORM_FLOAT_1, false);
		//m->shader()->addUniform(&farPlane, "farPlane", UNIFORM_FLOAT_1, false);
	}


	photonMap.clear();

	return;

}

void Scene::addIRShadowMap()
{
	std::vector<PhotonMap*> irMap = computePhotonMap(true, 5);

	/*create the light camera with the matrix*/
	Camera* lightCam = new Camera(90, 1, NEAR_PLANE, FAR_PLANE, new Matrix4x4());

	/*create the temp renderbuffer*/
	Renderbuffer* rb = new Renderbuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);


	/*copy the entites,*/
	std::vector<Model_obj*> entityCopyList;
	int nLight = irMap.size();
	std::vector<Texture_obj<float>*> depthTex;

	Model_obj* entityLookup;
	for (int i = 0; i < __entityList.size(); i++)
	{
		entityLookup = (Model_obj*)__entityList[i];
		/*create a new entity, copy over all the parameters*/
		Model_obj* e = new Model_obj(entityLookup->filePath(), new DepthShader(), false);
		e->moveTo(entityLookup->x(), entityLookup->y(), entityLookup->z());
		e->rotateTo(entityLookup->rotX(), entityLookup->rotY(), entityLookup->rotZ());
		e->scaleTo(entityLookup->scaleX(), entityLookup->scaleY(), entityLookup->scaleZ());
		e->copyParameters(entityLookup);

		e->shader()->addUniform((void*)e->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, false);

		entityCopyList.push_back(e);

		entityLookup->shader()->addUniform((void*)&nLight, "numLightMaps", UNIFORM_INT_1, false);

		int fp = getControllerInstance()->camera()->farPlane();
		int np = getControllerInstance()->camera()->nearPlane();
		entityLookup->shader()->addUniform(&fp, "farPlane", UNIFORM_INT_1, false);
		entityLookup->shader()->addUniform(&np, "nearPlane", UNIFORM_INT_1, false);
	}

	Matrix4x4** lightCamMatrix = new Matrix4x4*[irMap.size()];
	for (int i = 0; i < irMap.size(); i++)
	{
		glDisable(GL_BLEND);
		rb->bindFramebuffer();

		PhotonMap* map = irMap[i];

		float x = map->pos[0];
		float y = map->pos[1];
		float z = map->pos[2];
		float normX = map->normal[0];
		float normY = map->normal[1];
		float normZ = map->normal[2];
		float az = atan2f(normX, -normZ);
		float ay = asin(normY);

		lightCam->mvm()->loadIdentity();
		lightCam->mvm()->rotate(az, 0, 1.0, 0);
		lightCam->mvm()->rotate(ay, 1.0, 0.0, 0);
		lightCam->mvm()->translate(-x, -y, -z);

		lightCamMatrix[i] = new Matrix4x4();
		float lightPos[4];
		lightPos[0] = x;
		lightPos[1] = y;
		lightPos[2] = z;

		lightPos[3] = map->power;

		for (int j = 0; j < __entityList.size(); j++)
		{
			entityLookup = (Model_obj*)__entityList[j];

			std::string matrixUniformString("lightCameraMatrix[");
			matrixUniformString += std::to_string(i);
			matrixUniformString += std::string("]");

			std::string lightPositionString("irLightData[");
			lightPositionString += std::to_string(i);
			lightPositionString += std::string("]");

			Model_obj* e = (Model_obj*)entityCopyList[j];
			e->update(lightCam);
			lightCam->projectionMatrix()->mult(e->mvm(), lightCamMatrix[i]);
			entityLookup->shader()->addUniform((void*)lightCamMatrix[i]->mat, (GLchar*)matrixUniformString.c_str(), UNIFORM_MAT_4, false);
			e->shader()->addUniform(lightCam->projectionMatrix()->mat, "projectionMatrix", UNIFORM_MAT_4, false);
			((Model_obj*)this->getEntity(j))->shader()->addUniform(lightPos, (GLchar*)lightPositionString.c_str(), UNIFORM_FLOAT_4, false);
		}

		glDisable(GL_BLEND);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int j = 0; j < entityCopyList.size(); j++)
		{
			/*render the shadow map*/
			entityCopyList[j]->update(lightCam);
			entityCopyList[j]->render();
		}
		glFinish();
		rb->unbindFramebuffer();

		int tWidth, tHeight;
		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->depthTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);

		float* depthFullRes = new float[tWidth*tHeight];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthFullRes);


		int index = 0;
		cv::Mat depthMat(tWidth, tHeight, CV_32F);
		for (int indexY = 0; indexY < tHeight; indexY++)
		{
			for (int indexX = 0; indexX < tWidth; indexX++)
			{
				depthMat.at<float>(indexX, indexY) = depthFullRes[index];
				index++;
			}
		}
		
		int resizeWidth = tWidth / 4;
		int resizeHeight = tHeight / 4;

		cv::Mat depthMatResize;
		cv::resize(depthMat, depthMatResize, cv::Size(resizeHeight, resizeWidth), 0.0, 0.0, CV_INTER_LINEAR);
		
		index = 0;
		float* depthResize = new float[resizeWidth*resizeHeight];
		for (int indexY = 0; indexY < resizeHeight; indexY++)
		{
			for (int indexX = 0; indexX < resizeWidth; indexX++)
			{
				depthResize[index] = depthMatResize.at<float>(indexX, indexY);
				index++;
			}
		}

		Texture_obj<GLfloat>* tex = new Texture_obj<GLfloat>();
		tex->width = resizeWidth;
		tex->height = resizeHeight;
		tex->channelSize = 1;
		tex->buf = depthResize;

		depthTex.push_back(tex);
	}


	/*ADD TEXTURES*/
	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)__entityList[i])->shader()->addTextureArrayDepth(depthTex, depthTex.size(), "depthMap", -1);
	}


	/*CLEANUP*/
	/*delete the copy entities*/
	for (int i = entityCopyList.size() - 1; i >= 0; i--)
	{
		Model_obj* m = entityCopyList[i];
		delete m;
		entityCopyList.pop_back();
	}

	for (int i = 0; i < nLight; i++)
	{
		delete lightCamMatrix[i];
	}

}

/*
@function addShadowMapw

adds a shadow map texture and an array of shadow map matricies to each model in the scene
*/
void Scene::addShadowMap()
{
	/*create the temp renderbuffer*/
	Renderbuffer* rb = new Renderbuffer(SCREEN_WIDTH,SCREEN_HEIGHT,GL_FLOAT);

	/*create the light camera with the matrix*/
	Camera* lightCam = new Camera(90, 1, NEAR_PLANE, FAR_PLANE, new Matrix4x4());
	lightCam->projectionMatrix()->set(0, 0, 0.707);
	lightCam->projectionMatrix()->set(1, 1, 0.707);

	/*copy the entites,*/
	std::vector<Model_obj*> entityCopyList;
	int nLight = this->numLightEntities();
	std::vector<Texture_obj<float>*> depthTex;

	Model_obj* entityLookup;
	for (int i = 0; i < __entityList.size(); i++)
	{
		entityLookup = (Model_obj*)__entityList[i];
		/*create a new entity, copy over all the parameters*/
		Model_obj* e = new Model_obj(entityLookup->filePath(), new DepthShader(), false);
		e->moveTo(entityLookup->x(), entityLookup->y(), entityLookup->z());
		e->rotateTo(entityLookup->rotX(), entityLookup->rotY(), entityLookup->rotZ());
		e->scaleTo(entityLookup->scaleX(), entityLookup->scaleY(), entityLookup->scaleZ());
		e->copyParameters(entityLookup);

		e->shader()->addUniform((void*)e->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, false);

		entityCopyList.push_back(e);
		
		entityLookup->shader()->addUniform((void*)&nLight, "numLightMaps", UNIFORM_INT_1, false);

		int fp = getControllerInstance()->camera()->farPlane();
		int np = getControllerInstance()->camera()->nearPlane();
		entityLookup->shader()->addUniform(&fp, "farPlane", UNIFORM_INT_1, false);
		entityLookup->shader()->addUniform(&np, "nearPlane", UNIFORM_INT_1, false);
	}

	Matrix4x4** lightCamMatrix = new Matrix4x4*[nLight];
	for (int lightIndex = 0; lightIndex < nLight; lightIndex++)
	{
		glDisable(GL_BLEND);
		rb->bindFramebuffer();
		FrustumLight* fl = (FrustumLight*)getLightEntity(lightIndex);

		lightCam->mvm()->loadIdentity();
		
		lightCam->mvm()->rotate(-fl->rotX(), 1.0, 0.0, 0.0);
		lightCam->mvm()->rotate(-fl->rotY(), 0.0, 1.0, 0.0);
		lightCam->mvm()->rotate(-fl->rotZ(), 0.0, 0.0, 1.0);
		lightCam->mvm()->translate(-fl->position()[0], -fl->position()[1], -fl->position()[2]);

		lightCamMatrix[lightIndex] = new Matrix4x4();
		float lightPos[4];
		lightPos[0] = fl->position()[0];
		lightPos[1] = fl->position()[1];
		lightPos[2] = fl->position()[2];
		//lightCam->projectionMatrix()->set(0, 0, 1.0/fl->projectionWidth);
		//lightCam->projectionMatrix()->set(1, 1, 1.0/fl->projectionHeight);

		for (int i = 0; i < __entityList.size(); i++)
		{
			entityLookup = (Model_obj*)__entityList[i];

			std::string matrixUniformString("lightCameraMatrix[");
			matrixUniformString += std::to_string(lightIndex);
			matrixUniformString += std::string("]");

			std::string lightPositionString("lightPosition[");
			lightPositionString += std::to_string(lightIndex);
			lightPositionString += std::string("]");
			
			Model_obj* e = (Model_obj*)entityCopyList[i];
			e->update(lightCam);
			lightCam->projectionMatrix()->mult(e->mvm(), lightCamMatrix[lightIndex]);
			entityLookup->shader()->addUniform((void*)lightCamMatrix[lightIndex]->mat, (GLchar*)matrixUniformString.c_str(), UNIFORM_MAT_4, false);
			entityLookup->shader()->addUniform(lightPos, (GLchar*)lightPositionString.c_str(), UNIFORM_FLOAT_4, false);
			e->shader()->addUniform(lightCam->projectionMatrix()->mat, "projectionMatrix", UNIFORM_MAT_4, false);
			
		}

		glDisable(GL_BLEND);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < entityCopyList.size(); i++)
		{
			/*render the shadow map*/
			entityCopyList[i]->update(lightCam);
			entityCopyList[i]->render();
		}
		glFinish();
		rb->unbindFramebuffer();

		Texture_obj<GLfloat>* tex = new Texture_obj<GLfloat>();
		int tWidth, tHeight;
		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->depthTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);

		tex->width = tWidth;
		tex->height = tHeight;
		tex->channelSize = 1;
		tex->buf = new float[tWidth*tHeight];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, tex->buf);

		cv::Mat mm(tWidth, tHeight, CV_32F);
		int tIndex = 0;
		for (int j = 0; j < tHeight; j++)
		{
			for (int i = 0; i < tWidth; i++)
			{
				mm.at<float>(i, j) = tex->buf[tIndex];
				float temp = tex->buf[tIndex];
				tIndex++;
			}
		}

		cv::Mat out(tHeight, tWidth, CV_32F);
		cv::GaussianBlur(mm, out, cv::Size(21, 21), 2.0, 2.0, cv::BORDER_REPLICATE);
		
		depthTex.push_back(tex);

		tIndex = 0;
		for (int j = 0; j < tHeight; j++)
		{
			for (int i = 0; i < tWidth; i++)
			{
				tex->buf[tIndex] = (out.at<float>(i, j));
				tIndex++;
			}
		}

		glEnable(GL_BLEND);
		rb->unbindFramebuffer();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	delete lightCam;

	/*delete the copy entities*/
	for (int i = entityCopyList.size() - 1; i >= 0; i--)
	{
		Model_obj* m = entityCopyList[i];
		delete m;
		entityCopyList.pop_back();
	}

	for (int i = 0; i < nLight; i++)
	{
		delete lightCamMatrix[i];
	}


	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)__entityList[i])->shader()->addTextureArrayDepth(depthTex, depthTex.size(), "depthMap", -1);
	}

	delete rb;
}


void Scene::addReflectiveShadowMap()
{
	addShadowMap();

	//Renderbuffer *rb = new Renderbuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);

	Camera* lightCam = new Camera(90, 1, NEAR_PLANE, FAR_PLANE, new Matrix4x4());
	lightCam->projectionMatrix()->set(0, 0, 0.707);
	lightCam->projectionMatrix()->set(1, 1, 0.707);

	int tWidth = SCREEN_WIDTH;
	int tHeight = SCREEN_HEIGHT;
	GLfloat* returnTex;

	std::vector<Texture_obj<GLfloat>*> imVecGlobalPosition;
	std::vector<Texture_obj<GLfloat>*> imVecNormal;
	std::vector<Texture_obj<GLfloat>*> imVecReflectedRadiance;
	std::vector<Texture_obj<GLfloat>*> imVecNormalDepth;

	Texture_obj<GLfloat>* texObj;

	std::vector<unsigned char> bufout;
	unsigned char* ucText = new unsigned char[SCREEN_WIDTH*SCREEN_HEIGHT * 4];
	
	//Scene* sceneCopy = this->copyScene<RadianceShadowMapShader>();
	Scene* sc = this->copyScene<RadianceShadowMapShader>();
	int nLight = this->numLightEntities();
	Model_obj* entityLookup;
	
	for (int i = 0; i < __entityList.size(); i++)
	{
		entityLookup = (Model_obj*)sc->getEntity(i);
		/*create a new entity, copy over all the parameters*/

		entityLookup->moveTo(entityLookup->x(), entityLookup->y(), entityLookup->z());
		entityLookup->rotateTo(entityLookup->rotX(), entityLookup->rotY(), entityLookup->rotZ());
		entityLookup->scaleTo(entityLookup->scaleX(), entityLookup->scaleY(), entityLookup->scaleZ());
		entityLookup->copyParameters((Model_obj*)(this->getEntity(i)));
		entityLookup->shader()->addUniform((void*)entityLookup->mvm()->data(), "modelViewMatrix", UNIFORM_MAT_4, false);
		entityLookup->shader()->addUniform((void*)&nLight, "numLightMaps", UNIFORM_INT_1, false);

		int fp = getControllerInstance()->camera()->farPlane();
		int np = getControllerInstance()->camera()->nearPlane();
		entityLookup->shader()->addUniform(&fp, "farPlane", UNIFORM_INT_1, false);
		entityLookup->shader()->addUniform(&np, "nearPlane", UNIFORM_INT_1, false);
	}

	glDisable(GL_BLEND);
	//const GLenum buffers[] = { GL_NONE, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	//glDrawBuffers(1, buffers);

	float lightPos[4];
	Matrix4x4** lightCamMatrix = new Matrix4x4*[nLight];
	for (int lightIndex = 0; lightIndex < nLight; lightIndex++)
	{
		sc->renderbuffer->bindFramebuffer();
		FrustumLight* fl = (FrustumLight*)getLightEntity(lightIndex);

		lightCam->mvm()->loadIdentity();
		lightCam->mvm()->translate(-fl->position()[0], -fl->position()[1], -fl->position()[2]);
		lightCam->mvm()->rotate(-fl->rotX(), 1.0, 0.0, 0.0);
		lightCam->mvm()->rotate(-fl->rotY(), 0.0, 1.0, 0.0);
		lightCam->mvm()->rotate(-fl->rotZ(), 0.0, 0.0, 1.0);

		lightCamMatrix[lightIndex] = new Matrix4x4();
		lightPos[0] = fl->position()[0];
		lightPos[1] = fl->position()[1];
		lightPos[2] = fl->position()[2];
		//lightCam->projectionMatrix()->set(0, 0, 1.0/fl->projectionWidth);
		//lightCam->projectionMatrix()->set(1, 1, 1.0/fl->projectionHeight);

		for (int i = 0; i < __entityList.size(); i++)
		{
			entityLookup = (Model_obj*)__entityList[i];

			Model_obj* e = (Model_obj*)sc->getEntity(i);
			e->update(lightCam);
			lightCam->projectionMatrix()->mult(e->mvm(), lightCamMatrix[lightIndex]);
			e->shader()->addUniform(lightCam->projectionMatrix()->mat, "projectionMatrix", UNIFORM_MAT_4, false);
			e->shader()->addUniform(lightPos, "lightPosition[0]", UNIFORM_FLOAT_4, false);
		}

		glDisable(GL_BLEND);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, buffers);
		
		for (int i = 0; i < sc->numEntities(); i++)
		{
			/*render the shadow map*/
			sc->getEntity(i)->update(lightCam);
			sc->getEntity(i)->render();
		}
		glFinish();
		sc->renderbuffer->unbindFramebuffer();

		glBindTexture(GL_TEXTURE_2D, sc->renderbuffer->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);

		texObj = new Texture_obj<GLfloat>();
		texObj->width = tWidth;
		texObj->height = tHeight;
		texObj->buf = returnTex;
		texObj->channelSize = 4;
		imVecGlobalPosition.push_back(texObj);
		

		glBindTexture(GL_TEXTURE_2D, sc->renderbuffer->framebuffer->colorTex1);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);

		texObj = new Texture_obj<GLfloat>();
		texObj->width = tWidth;
		texObj->height = tHeight;
		texObj->buf = returnTex;
		texObj->channelSize = 4;
		imVecNormal.push_back(texObj);
		
		
		glBindTexture(GL_TEXTURE_2D, sc->renderbuffer->framebuffer->colorTex2);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);

		texObj = new Texture_obj<GLfloat>();
		texObj->width = tWidth;
		texObj->height = tHeight;
		texObj->buf = returnTex;
		texObj->channelSize = 4;
		imVecReflectedRadiance.push_back(texObj);
		
	}



	for (int i = 0; i < numEntities(); i++)
	{
		((Model_obj*)this->getEntity(i))->shader()->addTextureArray(imVecGlobalPosition, imVecGlobalPosition.size(), "globalPositionTexture", -1);
		((Model_obj*)this->getEntity(i))->shader()->addTextureArray(imVecNormal, imVecNormal.size(), "normalTexture", -1);
		((Model_obj*)this->getEntity(i))->shader()->addTextureArray(imVecReflectedRadiance, imVecReflectedRadiance.size(), "reflectedRadianceTexture", -1);
	}

	delete sc;
	
}


template <typename T>
Scene* Scene::copyScene()
{
	Camera* sceneCam = new Camera(90, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	Scene* sceneCopy = new Scene(sceneCam);
	float aspectRatio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	int sType;
	for (int i = 0; i < __entityList.size(); i++)
	{
		Model_obj* m = new Model_obj(((Model_obj*)__entityList[i])->filePath(), new T(), false);
		sType = m->shaderType();
		m->copyParameters(((Model_obj*)__entityList[i]));
		m->moveTo(__entityList[i]->x(), __entityList[i]->y(), __entityList[i]->z());
		m->rotateTo(__entityList[i]->rotX(), __entityList[i]->rotY(), __entityList[i]->rotZ());
		m->scaleTo(__entityList[i]->scaleX(), __entityList[i]->scaleY(), __entityList[i]->scaleZ());
		m->shader()->addUniform((void*)&aspectRatio, "windowAspectRatio", UNIFORM_FLOAT_1, false);
		sceneCopy->addEntity(m);
	}
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		LightEntity* l = new LightEntity(__lightEntityList[i]->lightType);
		LightEntity* lightEntity = (LightEntity*)__lightEntityList[i];

		l->moveTo(lightEntity->x(), lightEntity->y(), lightEntity->z());
		l->rotateTo(lightEntity->rotX(), lightEntity->rotY(), lightEntity->rotZ());
		l->scaleTo(lightEntity->scaleX(), lightEntity->scaleY(), lightEntity->scaleZ());

		sceneCopy->addLightEntity(l);
	}
	sceneCopy->shadingType = sType;
	sceneCopy->updateScene();
	sceneCopy->preRender();
	return sceneCopy;
}

std::vector<Texture_obj<GLfloat>*> Scene::computeRadianceTexture()
{	
	Scene* sceneCopySSSD = this->copyScene<SSSDiffuseProjectionTextureShader>();

	Renderbuffer *rb = new Renderbuffer(SCREEN_WIDTH, SCREEN_HEIGHT, GL_FLOAT);
	
	int tWidth = SCREEN_WIDTH;
	int tHeight = SCREEN_HEIGHT;

	std::vector<Texture_obj<GLfloat>*> imVec;
	Texture_obj<GLfloat>* texObj = new Texture_obj<GLfloat>();
	imVec.push_back(texObj);

	std::vector<Texture_obj<GLfloat>*> texRadianceVec;
	Model_obj* e;
	GLfloat* returnTex;

	std::vector<uchar> bufout;

	for (int i = 0; i < __entityList.size(); i++)
	{

		Scene* sceneCopyRadianceTransmitted = this->copyScene<PhotonMapTransmittanceProjectionTextureShader>();
		
		//render the transmitted radiance of the model to the texture
		rb->bindFramebuffer();
		e = (Model_obj*)sceneCopyRadianceTransmitted->getEntity(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);
		e->render();
		glFinish();
		rb->unbindFramebuffer();
		glDrawBuffer(GL_BACK);

		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_TRUE);

		imVec[0]->buf = returnTex;
		imVec[0]->width = tWidth;
		imVec[0]->height = tHeight;
		imVec[0]->channelSize = 4;
		((Model_obj*)sceneCopySSSD->getEntity(i))->shader()->addTextureArray(imVec, 1, "transmittanceRadiance", -1);
		
		int tIndex = 0;
		unsigned char* returnTex8U = new unsigned char[tWidth*tHeight * 4];
		for (int y = 0; y < tHeight; y++)
		{
			for (int x = 0; x < tWidth; x++)
			{
				returnTex8U[tIndex] = round(255.0*abs(returnTex[tIndex]));
				returnTex8U[tIndex+1] = round(255.0*abs(returnTex[tIndex+1]));
				returnTex8U[tIndex+2] = round(255.0*abs(returnTex[tIndex+2]));
				returnTex8U[tIndex+3] = round(255.0*abs(returnTex[tIndex+3]));
				tIndex += 4;
			}
		}

		std::string fpTransmittedRadiance("../bin/exports/transmittedRadiance.png");
		lodepng::encode(bufout, returnTex8U, tWidth, tHeight);
		lodepng::save_file(bufout, fpTransmittedRadiance);
		bufout.clear();
		delete returnTex8U;

		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex1);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_TRUE);

		imVec[0]->buf = returnTex;
		imVec[0]->width = tWidth;
		imVec[0]->height = tHeight;
		imVec[0]->channelSize = 4;
		((Model_obj*)sceneCopySSSD->getEntity(i))->shader()->addTextureArray(imVec, 1, "reflectedRadiance", -1);
		delete sceneCopyRadianceTransmitted;


		Scene* sceneCopyNormalDepthShader = this->copyScene<NormalDepthProjectionTextureShader>();
		//render the NORMAL DEPTH texture
		rb->bindFramebuffer();
		e = (Model_obj*)sceneCopyNormalDepthShader->getEntity(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		e->render();
		glFinish();
		rb->unbindFramebuffer();

		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_TRUE);

		imVec[0]->buf = returnTex;
		imVec[0]->width = tWidth;
		imVec[0]->height = tHeight;
		imVec[0]->channelSize = 4;
		((Model_obj*)sceneCopySSSD->getEntity(i))->shader()->addTextureArray(imVec, 1, "normalDepthTex", -1);

		tIndex = 0;
		returnTex8U = new unsigned char[tWidth*tHeight * 4];
		for (int y = 0; y < tHeight; y++)
		{
			for (int x = 0; x < tWidth; x++)
			{
				returnTex8U[tIndex] = round(255.0*abs(returnTex[tIndex]));
				returnTex8U[tIndex + 1] = round(255.0*abs(returnTex[tIndex + 1]));
				returnTex8U[tIndex + 2] = round(255.0*abs(returnTex[tIndex + 2]));
				returnTex8U[tIndex + 3] = round(255.0*abs(returnTex[tIndex + 3]));
				tIndex += 4;
			}
		}

		std::string fpNormalDepth("../bin/exports/normalDepth.png");
		lodepng::encode(bufout, returnTex8U, tWidth, tHeight);
		lodepng::save_file(bufout, fpNormalDepth);
		bufout.clear();
		delete sceneCopyNormalDepthShader;
		delete returnTex8U;

		Scene* sceneCopyPositionShader = this->copyScene<PositionProjectionTextureShader>();
		//render the NORMAL DEPTH texture
		rb->bindFramebuffer();
		e = (Model_obj*)sceneCopyPositionShader->getEntity(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		e->render();
		glFinish();
		rb->unbindFramebuffer();

		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_TRUE);

		imVec[0]->buf = returnTex;
		imVec[0]->width = tWidth;
		imVec[0]->height = tHeight;
		imVec[0]->channelSize = 4;
		((Model_obj*)sceneCopySSSD->getEntity(i))->shader()->addTextureArray(imVec, 1, "globalPositionTex", -1);
		
		tIndex = 0;
		returnTex8U = new unsigned char[tWidth*tHeight * 4];
		for (int y = 0; y < tHeight; y++)
		{
			for (int x = 0; x < tWidth; x++)
			{
				returnTex8U[tIndex] = round(255.0*abs(returnTex[tIndex]));
				returnTex8U[tIndex + 1] = round(255.0*abs(returnTex[tIndex + 1]));
				returnTex8U[tIndex + 2] = round(255.0*abs(returnTex[tIndex + 2]));
				returnTex8U[tIndex + 3] = round(255.0*abs(returnTex[tIndex + 3]));
				tIndex += 4;
			}
		}

		std::string fpGlobalPosition("../bin/exports/globalPosition.png");
		lodepng::encode(bufout, returnTex8U, tWidth, tHeight);
		lodepng::save_file(bufout, fpGlobalPosition);
		bufout.clear();
		delete sceneCopyPositionShader;
		delete returnTex8U;

		rb->bindFramebuffer();
		Model_obj* e = (Model_obj*)sceneCopySSSD->getEntity(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		e->render();
		glFinish();
		rb->unbindFramebuffer();

		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		returnTex = new GLfloat[tWidth*tHeight * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, returnTex);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_TRUE);


		tIndex = 0;
		returnTex8U = new unsigned char[tWidth*tHeight * 4];
		for (int y = 0; y < tHeight; y++)
		{
			for (int x = 0; x < tWidth; x++)
			{
				returnTex8U[tIndex] = round(255.0*abs(returnTex[tIndex]));
				returnTex8U[tIndex + 1] = round(255.0*abs(returnTex[tIndex + 1]));
				returnTex8U[tIndex + 2] = round(255.0*abs(returnTex[tIndex + 2]));
				returnTex8U[tIndex + 3] = round(255.0*abs(returnTex[tIndex + 3]));
				tIndex += 4;
			}
		}

		std::string fpSubsurfaceScatteringDiffuse("../bin/exports/subsurfaceScatteringDiffuse.png");
		lodepng::encode(bufout, returnTex8U, tWidth, tHeight);
		lodepng::save_file(bufout, fpSubsurfaceScatteringDiffuse);
		bufout.clear();
		delete returnTex8U;

		Texture_obj<GLfloat>* texObject = new Texture_obj<GLfloat>();
		texObject->buf = returnTex;
		texObject->width = tWidth;
		texObject->height = tHeight;
		texObject->channelSize = 4;
		texRadianceVec.push_back(texObject);
	}

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	delete sceneCopySSSD;
	/*
	delete sceneCopyRadianceReflected;
	delete sceneCopyPositionShader;
	delete sceneCopyNormalDepthShader;
	delete sceneCopySSSD;
	*/

	//kernel.clear();
	//delete kernelTex;
	//delete sceneCopy;
	//delete rbTexFilter;
	delete rb;
	//delete filter;
	//delete sceneCam;
	/*
	glBindTexture(GL_TEXTURE_2D, rbTexFilter->framebuffer->colorTex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
	GLubyte* returnTex = new GLubyte[tWidth*tWidth*4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, returnTex);

	Texture_obj<GLubyte>* texObject = new Texture_obj<GLubyte>();
	texObject->buf = returnTex;
	texObject->width = tWidth;
	texObject->height = tHeight;
	texObject->channelSize = 4;
	*/
	return texRadianceVec;
}

void Scene::computeSubsurfaceScatteringTextures()
{
	addShadowMap();
	computeRadianceTexture();	
}

std::vector<Texture_obj<GLubyte>*> Scene::computePhotonMapDiffuseTexture()
{
	Camera* sceneCam = new Camera(90, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	Scene* sceneCopy = new Scene(sceneCam);
	float aspectRatio = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);
	for (int i = 0; i < __entityList.size(); i++)
	{
		Model_obj* m = new Model_obj(((Model_obj*)__entityList[i])->filePath(), new PhotonMapProjectionTextureShader(), false);
		m->copyParameters(((Model_obj*)__entityList[i]));
		m->moveTo(__entityList[i]->x(), __entityList[i]->y(), __entityList[i]->z());
		m->rotateTo(__entityList[i]->rotX(), __entityList[i]->rotY(), __entityList[i]->rotZ());
		m->scaleTo(__entityList[i]->scaleX(), __entityList[i]->scaleY(), __entityList[i]->scaleZ());
		m->shader()->addUniform((void*)&aspectRatio, "windowAspectRatio", UNIFORM_FLOAT_1, false);
		sceneCopy->addEntity(m);
	}
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		LightEntity* l = new LightEntity(__lightEntityList[i]->lightType);
		LightEntity* lightEntity = (LightEntity*)__lightEntityList[i];

		l->moveTo(lightEntity->x(), lightEntity->y(), lightEntity->z());
		l->rotateTo(lightEntity->rotX(), lightEntity->rotY(), lightEntity->rotZ());
		l->scaleTo(lightEntity->scaleX(), lightEntity->scaleY(), lightEntity->scaleZ());

		sceneCopy->addLightEntity(l);
	}
	sceneCopy->shadingType = SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE;
	sceneCopy->updateScene();
	sceneCopy->addPhotonMap(true);

	Renderbuffer *rb = new Renderbuffer(SCREEN_WIDTH, SCREEN_HEIGHT,GL_UNSIGNED_BYTE);
	int tWidth, tHeight;
	std::vector<Texture_obj<GLubyte>*> textures;

	for (int i = 0; i < __entityList.size(); i++)
	{
		rb->bindFramebuffer();
		Model_obj* e = (Model_obj*)sceneCopy->getEntity(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		e->render();
		glFinish();
		rb->unbindFramebuffer();


		glBindTexture(GL_TEXTURE_2D, rb->framebuffer->colorTex);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
		GLubyte* returnTex = new GLubyte[tWidth*tWidth * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, returnTex);

		Texture_obj<GLubyte>* texObject = new Texture_obj<GLubyte>();
		texObject->buf = returnTex;
		texObject->width = tWidth;
		texObject->height = tHeight;
		texObject->channelSize = 4;
		textures.push_back(texObject);
	}

	for (int i = 0; i < __entityList.size(); i++)
	{
		((Model_obj*)__entityList[i])->shader()->addTextureArray(textures, textures.size(), "photonMapDiffuseTexture", -1);
	}

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	delete sceneCopy;
	delete rb;
	delete sceneCam;

	return textures;
}

/*
@function environmentMapVertexRadiance

compute the incident radiance at each vertex in the scene, returned as a vector of 'VertexRadiance'
*/
std::vector<Texture_obj<GLfloat>*> Scene::environmentMapVertexRadiance()
{
	/*
		for each vertex,
			compute camera matrix and projection matrix for the matrix.
			render the scene using the shadow map shader
			pull the rendered texture
			compute the SH at the vertex
	*/
	Camera* sceneCam = new Camera(90, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	Scene* sceneCopy = new Scene(sceneCam);

	float projectionWidth = 30;
	float projectionHeight = 30;
	Camera* cam = new Camera(90, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	cam->projectionMatrix()->set(0, 0, cam->projectionMatrix()->get(0, 0) / projectionWidth);
	cam->projectionMatrix()->set(1, 1, cam->projectionMatrix()->get(1, 1) / projectionHeight);

	for (int i = 0; i < __entityList.size(); i++)
	{
		Model_obj* m = new Model_obj(((Model_obj*)__entityList[i])->filePath(), new ShadowMapShader(), false);
		//Model_obj* m = new Model_obj(((Model_obj*)__entityList[i])->filePath(), new RastShader(), false);
		m->copyParameters((Model_obj*)__entityList[i]);
		m->shader()->addUniform((void*)cam->projectionMatrix()->data(), "projectionMatrix", UNIFORM_MAT_4, true);
		m->update(cam);
		sceneCopy->addEntity(m);
	}
	for (int i = 0; i < __lightEntityList.size(); i++)
	{
		LightEntity* l = new LightEntity(__lightEntityList[i]->lightType);
		LightEntity* lightEntity = (LightEntity*)__lightEntityList[i];

		l->moveTo(lightEntity->x(), lightEntity->y(), lightEntity->z());
		l->rotateTo(lightEntity->rotX(), lightEntity->rotY(), lightEntity->rotZ());
		l->scaleTo(lightEntity->scaleX(), lightEntity->scaleY(), lightEntity->scaleZ());

		sceneCopy->addLightEntity(l);
	}
	sceneCopy->shadingType = SHADER_TYPE_SHADOW_MAP;

	std::vector<Texture_obj<GLfloat>*> emTextures;
	std::vector<Face*> faceList = this->computeEntityFaceList();
	
	float x, y, z;
	float normX, normY, normZ;
	float basisX[3];
	float basisZ[3];
	GLubyte* tempByte = new GLubyte[SCREEN_WIDTH*SCREEN_HEIGHT*4];

	Model_obj* entityLookup;
	std::vector<Model_obj*> entityCopy;

	sceneCopy->preRender();

	Matrix4x4 matBasis;
	matBasis.loadIdentity();

	int currentEMindex = 0;
	int totalEnvironmentMaps = faceList.size() * 3;
	for (int i = 0; i < faceList.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			x = faceList[i]->vertex[j]->pos[0];
			y = faceList[i]->vertex[j]->pos[1];
			z = faceList[i]->vertex[j]->pos[2];

			normX = faceList[i]->normal[j]->pos[0];
			normY = faceList[i]->normal[j]->pos[1];
			normZ = faceList[i]->normal[j]->pos[2];

			matBasis.loadIdentity();
			matBasis.rotate(-atan2f(normX, normY), 0.0, 0.0, 1.0);
			matBasis.rotate(asin(normZ), 1.0,0.0,0.0);
			basisX[0] = matBasis.get(0, 0);
			basisX[1] = matBasis.get(1, 0);
			basisX[2] = matBasis.get(2, 0);
			basisZ[0] = matBasis.get(0, 2);
			basisZ[1] = matBasis.get(1, 2);
			basisZ[2] = matBasis.get(2, 2);


			x = x + normX*0.01;
			y = y + normY*0.01;
			z = z + normZ*0.01;
			float az = atan2f(normX, -normZ);
			float ay = asin(normY);

			cam->mvm()->loadIdentity();
			cam->mvm()->rotate(az, 0, 1.0, 0);
			cam->mvm()->rotate(ay, 1.0, 0.0, 0);
			cam->mvm()->translate(-x, -y, -z);

			renderbuffer->bindFramebuffer();
			//glDrawBuffer(GL_FRONT);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Model_obj* entityLookup;
			for (int i = 0; i < __entityList.size(); i++)
			{
				entityLookup = (Model_obj*)__entityList[i];
				Model_obj* e = (Model_obj*)sceneCopy->getEntity(i);

				e->moveTo(entityLookup->x(), entityLookup->y(), entityLookup->z());
				e->rotateTo(entityLookup->rotX(), entityLookup->rotY(), entityLookup->rotZ());
				e->scaleTo(entityLookup->scaleX(), entityLookup->scaleY(), entityLookup->scaleZ());
				e->update(cam);
			}
			
			sceneCopy->render();
			glFinish();

			int tWidth, tHeight;
			glBindTexture(GL_TEXTURE_2D, renderbuffer->framebuffer->colorTex);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tWidth);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tHeight);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempByte);

			glEnable(GL_BLEND);
			renderbuffer->unbindFramebuffer();
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glDrawBuffer(GL_BACK);
			glReadBuffer(GL_BACK);

#if ENVIRONMENT_MAP_DEBUG
			std::vector<uchar> bufout;
			std::string fp("../bin/exports/environmentMapExports/exportEM_");
			fp.append(std::to_string(i));
			fp.append("-");
			fp.append(std::to_string(j));
			fp.append(".png");

			lodepng::encode(bufout, tempByte, tWidth, tHeight);
			lodepng::save_file(bufout, fp);
#endif
			Texture_obj<GLfloat>* t = new Texture_obj<GLfloat>();
			int index = 0;
			float irradiance[] = { 0.0,0.0,0.0 };
			float rin[3];
			//cv::Mat emResize(tWidth, tHeight, CV_32FC3);
			for (int yInd = 0; yInd < tHeight; yInd++)
			{
				for (int xInd = 0; xInd < tWidth; xInd++)
				{
					cv::Vec3f vec;
					vec[0] = ((float)tempByte[index]) / 255.0;
					vec[1] = ((float)tempByte[index + 1]) / 255.0;
					vec[2] = ((float)tempByte[index + 2]) / 255.0;
					//emResize.at<cv::Vec3f>(xInd, yInd) = vec;

					float xPlane = -projectionWidth + 2.0*projectionWidth*((float)xInd/(float)tWidth);
					float yPlane = -projectionHeight + 2.0*projectionHeight*((float)yInd / (float)tHeight);
					rin[0] = xPlane*basisX[0] + yPlane*basisZ[0] + 1.0*normX;
					rin[1] = xPlane*basisX[1] + yPlane*basisZ[1] + 1.0*normY;
					rin[2] = xPlane*basisX[2] + yPlane*basisZ[2] + 1.0*normZ;
					normalize(rin);
					float n[] = { normX,normY,normZ };
					float dp = dotProduct(rin, n);
					dp = maximum(dp, 0.0);
					irradiance[0] += vec[0]*dp;
					irradiance[1] += vec[1]*dp;
					irradiance[2] += vec[2]*dp;

					index = index + 4;
				}
			}
			float numPoints = (float)tWidth*(float)tHeight;
			irradiance[0] = (28.8739)*irradiance[0]/ (numPoints);
			irradiance[1] = (28.8739)*irradiance[1]/ (numPoints);
			irradiance[2] = (28.8739)*irradiance[2]/ (numPoints);

			int resizeX = 1;
			int resizeY = 1;
			t->width = resizeX;
			t->height = resizeY;
			t->channelSize = 3;
			t->buf = new GLfloat[t->width*t->height*t->channelSize];
			t->buf[0] = irradiance[0];
			t->buf[1] = irradiance[1];
			t->buf[2] = irradiance[2];

			/*
			cv::resize(emResize, emResize, cv::Size(resizeX, resizeY), 0.0, 0.0, CV_INTER_CUBIC);
			index = 0;
			float sum[] = { 0.0,0.0,0.0 };
			for (int xInd = 0; xInd < resizeX; xInd++)
			{
				for (int yInd = 0; yInd < resizeY; yInd++)
				{
					cv::Vec3f sample = emResize.at<cv::Vec3f>(xInd, yInd);

					t->buf[index] = sample[0];
					t->buf[index + 1] = sample[1];
					t->buf[index + 2] = sample[2];
					index = index + 3;

					sum[0] = sum[0] + sample[0];
					sum[1] = sum[1] + sample[1];
					sum[2] = sum[2] + sample[2];

				}
			}
			*/
			currentEMindex++;
			std::string completionText("environment map completion: ");
			completionText.append(std::to_string(currentEMindex));
			completionText.append("/");
			completionText.append(std::to_string(totalEnvironmentMaps));
			completionText.append("\n");
			OutputDebugStringA(completionText.c_str());
			emTextures.push_back(t);
		}

	}

	delete sceneCam;
	delete sceneCopy;
	faceList.clear();
	delete cam;
	delete tempByte;
	entityCopy.clear();

	return emTextures;
}

void Scene::addEnvironmentMap(std::vector<Texture_obj<GLfloat>*>& environmentMap)
{
	int numEnvMaps = environmentMap.size();
	int w = environmentMap[0]->width;
	int h = environmentMap[0]->height;
	int channelSize = environmentMap[0]->channelSize;
	int texSize = numEnvMaps*w*h*channelSize;
	GLfloat* emData = new GLfloat[texSize];

	int index = 0;
	int tIndex = 0;
	for (int i = 0; i < numEnvMaps; i++)
	{
		tIndex = 0;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				for (int c = 0; c < channelSize; c++)
				{
					emData[index] = environmentMap[i]->buf[tIndex];
					index++;
					tIndex++;
				}
			}
		}
	}


	int vIndex = 0;
	for (int i = 0; i < numEntities(); i++)
	{
		Model_obj* m = (Model_obj*)getEntity(i);
		m->shader()->addBufferTexture((void*)emData, texSize, sizeof(GLfloat), "environmentMap", GL_RGB32F, -1);
		m->shader()->addUniform(&w, "environmentMapN", UNIFORM_INT_1, false);
		m->shader()->addUniform(&h, "environmentMapM", UNIFORM_INT_1, false);

		GLfloat* vIndexArray = new GLfloat[m->faceList().size() * 3];
		for (int j = 0; j < m->faceList().size() * 3; j++)
		{
			vIndexArray[j] = (float)vIndex;
			vIndex++;
		}
		m->shader()->addAttributeBuffer((void*)vIndexArray, m->faceList().size() * 3, sizeof(float), GL_FLOAT, "vertIndex", 7, 1);
		delete vIndexArray;
	}
	delete emData;
}

/*
@function exportScene()
@param const char* filePath

exports the current scene to a scene file 'filePath'
*/
bool Scene::exportScene(const char* filePath)
{
	std::fstream file;
	file.open(filePath, std::fstream::out | std::fstream::binary);
	if (!file)
		return false;

	file.write((char*)&shadingType, 4);

	int numE = this->numEntities();

	file.write((char*)&numE, 4);

	Model_obj* currentEntity;
	for (int i = 0; i < numE; i++)
	{
		currentEntity = (Model_obj*)__entityList[i];
		int j = 0;
		while (currentEntity->filePath()[j] != '\0')
			j++;
		j++;

		file.write((char*)&j, 4);
		file.write((char*)currentEntity->filePath(), j);

		GLfloat x = currentEntity->x();
		GLfloat y = currentEntity->y();
		GLfloat z = currentEntity->z();

		GLfloat rotX = currentEntity->rotX();
		GLfloat rotY = currentEntity->rotY();
		GLfloat rotZ = currentEntity->rotZ();

		GLfloat scaleX = currentEntity->scaleX();
		GLfloat scaleY = currentEntity->scaleY();
		GLfloat scaleZ = currentEntity->scaleZ();

		file.write((char*)&x, 4);
		file.write((char*)&y, 4);
		file.write((char*)&z, 4);
		file.write((char*)&rotX, 4);
		file.write((char*)&rotY, 4);
		file.write((char*)&rotZ, 4);
		file.write((char*)&scaleX, 4);
		file.write((char*)&scaleY, 4);
		file.write((char*)&scaleZ, 4);

		unsigned int shaderType = currentEntity->shaderType();
		file.write((char*)&shaderType, 4);


		/*BRDF variables*/
		file.write((char*)&currentEntity->brdfType, 4);
		file.write((char*)&currentEntity->indexOfRefraction, 4);
		file.write((char*)&currentEntity->phongN, 4);
		file.write((char*)&currentEntity->microfacetM, 4);

		file.write((char*)&(currentEntity->absorption[0]), 4);
		file.write((char*)&(currentEntity->absorption[1]), 4);
		file.write((char*)&(currentEntity->absorption[2]), 4);
		file.write((char*)&(currentEntity->reducedScattering[0]), 4);
		file.write((char*)&(currentEntity->reducedScattering[1]), 4);
		file.write((char*)&(currentEntity->reducedScattering[2]), 4);
	}

	int numlightE = __lightEntityList.size();

	file.write((char*)&numlightE, 4);
	LightEntity* currentLightEntity;
	for (int i = 0; i < numlightE; i++)
	{
		currentLightEntity = (LightEntity*)__lightEntityList[i];
		int j = 0;
		while (currentLightEntity->filePath()[j] != '\0')
			j++;
		j++;


		file.write((char*)&j, 4);
		file.write(currentLightEntity->filePath(), j);

		GLfloat x = currentLightEntity->x();
		GLfloat y = currentLightEntity->y();
		GLfloat z = currentLightEntity->z();

		GLfloat rotX = currentLightEntity->rotX();
		GLfloat rotY = currentLightEntity->rotY();
		GLfloat rotZ = currentLightEntity->rotZ();

		GLfloat scaleX = currentLightEntity->scaleX();
		GLfloat scaleY = currentLightEntity->scaleY();
		GLfloat scaleZ = currentLightEntity->scaleZ();

		GLfloat lightIntensity = currentLightEntity->intensity;
		GLfloat lightType = currentLightEntity->lightType;

		file.write((char*)&x, 4);
		file.write((char*)&y, 4);
		file.write((char*)&z, 4);
		file.write((char*)&rotX, 4);
		file.write((char*)&rotY, 4);
		file.write((char*)&rotZ, 4);
		file.write((char*)&scaleX, 4);
		file.write((char*)&scaleY, 4);
		file.write((char*)&scaleZ, 4);
		file.write((char*)&lightIntensity, 4);
		file.write((char*)&lightType, 4);

		if (lightType == FRUSTUM_LIGHT)
		{
			GLfloat fov = ((FrustumLight*)currentLightEntity)->fov;
			GLfloat aspectRatio = ((FrustumLight*)currentLightEntity)->aspectRatio;
			GLfloat nearPlane = ((FrustumLight*)currentLightEntity)->nearPlane;
			GLfloat farPlane = ((FrustumLight*)currentLightEntity)->farPlane;
			file.write((char*)&fov, 4);
			file.write((char*)&aspectRatio, 4);
			file.write((char*)&nearPlane, 4);
			file.write((char*)&farPlane, 4);
		}

		unsigned int shaderType = currentLightEntity->shaderType();
		file.write((char*)&shaderType, 4);
	}

	int numBoundE = __boundaryEntityList.size();
	file.write((char*)&numBoundE, 4);

	for (int i = 0; i < numBoundE; i++)
	{
		currentEntity = (Model_obj*)__boundaryEntityList[i];
		int j = 0;
		while (currentEntity->filePath()[j] != '\0')
			j++;
		j++;

		file.write((char*)&j, 4);
		file.write(currentEntity->filePath(), j);

		GLfloat x = currentEntity->x();
		GLfloat y = currentEntity->y();
		GLfloat z = currentEntity->z();

		GLfloat rotX = currentEntity->rotX();
		GLfloat rotY = currentEntity->rotY();
		GLfloat rotZ = currentEntity->rotZ();

		GLfloat scaleX = currentEntity->scaleX();
		GLfloat scaleY = currentEntity->scaleY();
		GLfloat scaleZ = currentEntity->scaleZ();


		file.write((char*)&x, 4);
		file.write((char*)&y, 4);
		file.write((char*)&z, 4);
		file.write((char*)&rotX, 4);
		file.write((char*)&rotY, 4);
		file.write((char*)&rotZ, 4);
		file.write((char*)&scaleX, 4);
		file.write((char*)&scaleY, 4);
		file.write((char*)&scaleZ, 4);

		unsigned int shaderType = currentEntity->shaderType();
		file.write((char*)&shaderType, 4);
	}

	if (shadingType == SHADER_TYPE_WAVELET_ENVIRONMENT_MAP)
	{
		
		std::vector<Texture_obj<GLfloat>*> em = environmentMapVertexRadiance();
		int numEnvMaps = em.size();
		int w = em[0]->width;
		int h = em[0]->height;
		int cSize = em[0]->channelSize;
		int index = 0;
		int tIndex = 0;


		file.write((char*)&numEnvMaps, 4);
		file.write((char*)&w, 4);
		file.write((char*)&h, 4);
		file.write((char*)&cSize, 4);

		for (int i = 0; i < em.size(); i++)
		{
			tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < cSize; c++)
					{
						float d = em[i]->buf[tIndex];
						file.write((char*)&d, 4);
						index++;
						tIndex++;
					}
				}
			}
		}
		

		/*
		int numEnvMaps, w, h, channelSize;
		file.read((char*)&numEnvMaps, 4);
		file.read((char*)&w, 4);
		file.read((char*)&h, 4);
		file.read((char*)&channelSize, 4);


		int texSize = numEnvMaps*w*h*channelSize;
		GLfloat* emData = new GLfloat[texSize];

		int index = 0;
		int tIndex = 0;
		for (int i = 0; i < numEnvMaps; i++)
		{
			tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < channelSize; c++)
					{
						float d;
						file.read((char*)&d, 4);
						emData[index] = d;
						index++;
						tIndex++;
					}
				}
			}
		}
		*/
	}
	else if (shadingType == SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE)
	{
		addFaceLightTextureToScene();
		addShadowMap();
		std::vector<Texture_obj<GLfloat>*> radTex = computeRadianceTexture();

		int numTextures = radTex.size();
		file.write((char*)&numTextures, 4);

		for (int i = 0; i < radTex.size(); i++)
		{
			int h, w, cSize;
			w = radTex[i]->width;
			h = radTex[i]->height;
			cSize = radTex[i]->channelSize;

			file.write((char*)&w, 4);
			file.write((char*)&h, 4);
			file.write((char*)&cSize, 4);

			int tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < cSize; c++)
					{
						GLfloat d = radTex[i]->buf[tIndex];
						file.write((char*)&d, 4);
						tIndex++;
					}
				}
			}
		}
	}
	else if (shadingType == SHADER_TYPE_PHOTON_MAP )
	{
		std::vector<Texture_obj<GLubyte>*> tex = computePhotonMapDiffuseTexture();
		
		int numTextures = tex.size();
		file.write((char*)&numTextures, 4);

		for (int i = 0; i < tex.size(); i++)
		{
			int h, w, cSize;
			w = tex[i]->width;
			h = tex[i]->height;
			cSize = tex[i]->channelSize;

			file.write((char*)&w, 4);
			file.write((char*)&h, 4);
			file.write((char*)&cSize, 4);

			int tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < cSize; c++)
					{
						GLubyte d = tex[i]->buf[tIndex];
						file.write((char*)&d, 1);
						tIndex++;
					}
				}
			}
		}
	}

	file.close();
	return true;
}

Scene* importScene(const char* filePath, Camera* cam)
{	

	std::fstream file;
	file.open(filePath, std::fstream::in | std::fstream::binary);

	if (!file)
		return NULL;

	Scene* scene = new Scene(cam);
	file.read((char*)&scene->shadingType, 4);
	int numEntities;
	file.read((char*)&numEntities, 4);
	
	for (int i = 0; i < numEntities; i++)
	{
		int filePathSize;
		file.read((char*)&filePathSize, 4);
		char* fp = new char[filePathSize];
		file.read(fp, filePathSize);

		GLfloat x;
		GLfloat y;
		GLfloat z;

		GLfloat rotX;
		GLfloat rotY;
		GLfloat rotZ;

		GLfloat scaleX;
		GLfloat scaleY;
		GLfloat scaleZ;

		file.read((char*)&x, 4);
		file.read((char*)&y, 4);
		file.read((char*)&z, 4);
		file.read((char*)&rotX, 4);
		file.read((char*)&rotY, 4);
		file.read((char*)&rotZ, 4);
		file.read((char*)&scaleX, 4);
		file.read((char*)&scaleY, 4);
		file.read((char*)&scaleZ, 4);

		unsigned int shaderType;
		file.read((char*)&shaderType, 4);

		int brdfType;
		file.read((char*)&brdfType, 4);

		/*BRDF variables*/
		float indexOfRefraction, phongN, microfacetM;
		file.read((char*)&indexOfRefraction, 4);
		file.read((char*)&phongN, 4);
		file.read((char*)&microfacetM, 4);

		float absorption[3];
		float reducedScattering[3];
		file.read((char*)&(absorption[0]), 4);
		file.read((char*)&(absorption[1]), 4);
		file.read((char*)&(absorption[2]), 4);
		file.read((char*)&(reducedScattering[0]), 4);
		file.read((char*)&(reducedScattering[1]), 4);
		file.read((char*)&(reducedScattering[2]), 4);

		Model_obj* e;

		if (scene->shadingType == SHADER_TYPE_SHADOW_MAP)
		{
			ShadowMapShader* shaderR = new ShadowMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_PHOTON_MAP)
		{
			PhotonMapShader* shaderR = new PhotonMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_WAVELET_ENVIRONMENT_MAP)
		{
			WaveletEnvironmentMapShader* shaderR = new WaveletEnvironmentMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_SH_BRDF_PHOTON_MAP)
		{
			SHBRDFPhotonMapShader* shaderR = new SHBRDFPhotonMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE)
		{
			ShadowMapProjectionTextureShader* shaderT = new ShadowMapProjectionTextureShader();
			e = new Model_obj(fp, shaderT, false);
		}
		else if (scene->shadingType == SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE)
		{
			SSSDiffuseShader* shaderR = new SSSDiffuseShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE || scene->shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
		{
			PhotonMapProjectionTextureShader* shaderR = new PhotonMapProjectionTextureShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE)
		{
			NormalDepthProjectionTextureShader* shaderR = new NormalDepthProjectionTextureShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_POSITION_PROJECTION_TEXTURE)
		{
			PositionProjectionTextureShader* shaderR = new PositionProjectionTextureShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_NORMAL_PROJECTION_TEXTURE)
		{
			NormalProjectionTextureShader* shaderR = new NormalProjectionTextureShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_REFLECTIVE_SHADOW_MAP)
		{
			ReflectiveShadowMapShader* shaderR = new ReflectiveShadowMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_RADIANCE_SHADOW_MAP)
		{
			RadianceShadowMapShader* shaderR = new RadianceShadowMapShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else if (scene->shadingType == SHADER_TYPE_INSTANT_RADIOSITY)
		{
			InstantRadiosityShader* shaderR = new InstantRadiosityShader();
			e = new Model_obj(fp, shaderR, false);
		}
		else 
		{
			RastShader* shaderR = new RastShader();
			e = new Model_obj(fp, shaderR, false);
		}
		e->moveTo(x, y, z);
		e->scaleTo(scaleX, scaleY, scaleZ);
		e->rotateTo(rotX, rotY, rotZ);
		e->indexOfRefraction = indexOfRefraction;
		e->brdfType = brdfType;
		e->phongN = phongN;
		e->microfacetM = microfacetM;
		e->absorption[0] = absorption[0];
		e->absorption[1] = absorption[1];
		e->absorption[2] = absorption[2];
		e->reducedScattering[0] = reducedScattering[0];
		e->reducedScattering[1] = reducedScattering[1];
		e->reducedScattering[2] = reducedScattering[2];
		scene->addEntity(e);
	}

	int numLightEntities;
	file.read((char*)&numLightEntities, 4);
	for (int i = 0; i < numLightEntities; i++)
	{
		int filePathSize;
		file.read((char*)&filePathSize, 4);
		char* fpl = new char[filePathSize];
		file.read(fpl, filePathSize);

		GLfloat x;
		GLfloat y;
		GLfloat z;

		GLfloat rotX;
		GLfloat rotY;
		GLfloat rotZ;

		GLfloat scaleX;
		GLfloat scaleY;
		GLfloat scaleZ;

		GLfloat lightIntensity;
		GLfloat lightType;

		file.read((char*)&x, 4);
		file.read((char*)&y, 4);
		file.read((char*)&z, 4);
		file.read((char*)&rotX, 4);
		file.read((char*)&rotY, 4);
		file.read((char*)&rotZ, 4);
		file.read((char*)&scaleX, 4);
		file.read((char*)&scaleY, 4);
		file.read((char*)&scaleZ, 4);
		file.read((char*)&lightIntensity, 4);
		file.read((char*)&lightType, 4);

		LightEntity* le;

		if (lightType == FRUSTUM_LIGHT)
		{
			GLfloat pov, ar, np, fp;
			file.read((char*)&pov, 4);
			file.read((char*)&ar, 4);
			file.read((char*)&np, 4);
			file.read((char*)&fp, 4);
			FrustumLight* fl = new FrustumLight(pov, ar, np, fp);
			le = (LightEntity*)fl;
		}
		else
			le = new LightEntity(lightType);

		unsigned int shaderType;
		file.read((char*)&shaderType, 4);

		le->moveTo(x, y, z);
		le->scaleTo(scaleX, scaleY, scaleZ);
		le->rotateTo(rotX, rotY, rotZ);
		le->intensity = lightIntensity;
		scene->addLightEntity(le);
	}


	int numBoundaryEntities;
	file.read((char*)&numBoundaryEntities, 4);
	for (int i = 0; i < numBoundaryEntities; i++)
	{
		int filePathSize;
		file.read((char*)&filePathSize, 4);
		char* fpl = new char[filePathSize];
		file.read(fpl, filePathSize);

		GLfloat x;
		GLfloat y;
		GLfloat z;

		GLfloat rotX;
		GLfloat rotY;
		GLfloat rotZ;

		GLfloat scaleX;
		GLfloat scaleY;
		GLfloat scaleZ;

		file.read((char*)&x, 4);
		file.read((char*)&y, 4);
		file.read((char*)&z, 4);
		file.read((char*)&rotX, 4);
		file.read((char*)&rotY, 4);
		file.read((char*)&rotZ, 4);
		file.read((char*)&scaleX, 4);
		file.read((char*)&scaleY, 4);
		file.read((char*)&scaleZ, 4);

		unsigned int shaderType;
		file.read((char*)&shaderType, 4);
		SphereBoundaryEntity* be;

		be = new SphereBoundaryEntity();

		be->moveTo(x, y, z);
		be->scaleTo(scaleX, scaleY, scaleZ);
		be->rotateTo(rotX, rotY, rotZ);
		scene->addBoundaryEntity(be);
	}
	scene->updateScene();

	if (scene->shadingType == SHADER_TYPE_SHADOW_MAP || scene->shadingType == SHADER_TYPE_SHADOW_MAP_PROJECTION_TEXTURE)
	{
		scene->addFaceLightTextureToScene();
	}
	else if (scene->shadingType == SHADER_TYPE_PHOTON_MAP )
	{
		//scene->addPhotonMap();

		std::vector<Texture_obj<GLubyte>*> tex;

		int numTextures;
		file.read((char*)&numTextures, 4);

		for (int i = 0; i < numTextures; i++)
		{
			int h, w, cSize;
			//w = tex[i]->width;
			//h = tex[i]->height;
			//cSize = tex[i]->channelSize;

			file.read((char*)&w, 4);
			file.read((char*)&h, 4);
			file.read((char*)&cSize, 4);

			Texture_obj<GLubyte>* t = new Texture_obj<GLubyte>();
			t->width = w;
			t->height = h;
			t->channelSize = cSize;
			t->buf = new GLubyte[w*h*cSize];

			int tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < cSize; c++)
					{
						GLubyte d;
						file.read((char*)&d, 1);
						t->buf[tIndex] = d;
						tIndex++;
					}
				}
			}
			tex.push_back(t);
		}

		for (int i = 0; i < scene->numEntities(); i++)
		{
			((Model_obj*)scene->getEntity(i))->shader()->addTextureArray(tex, tex.size(), "photonMapDiffuseTexture", -1);
		}

	}
	else if (scene->shadingType == SHADER_TYPE_SH_BRDF_PHOTON_MAP)
	{
		scene->addPhotonMap(true);
		initBrdfSamples();
		for (int i = 0; i < scene->numEntities(); i++)
		{
			((Model_obj*)scene->getEntity(i))->addSampledBrdfSH(0);
			int brdfSize = brdfSampled[0]->n;
			((Model_obj*)scene->getEntity(i))->shader()->addUniform((void*)&brdfSize, "brdfN", UNIFORM_INT_1, false);
		}
	}
	else if (scene->shadingType == SHADER_TYPE_WAVELET_ENVIRONMENT_MAP)
	{
		int numEnvMaps,w,h,channelSize;
		file.read((char*)&numEnvMaps, 4);
		file.read((char*)&w, 4);
		file.read((char*)&h, 4);
		file.read((char*)&channelSize, 4);

		
		int texSize = numEnvMaps*w*h*channelSize;
		GLfloat* emData = new GLfloat[texSize];

		int index = 0;
		int tIndex = 0;
		for (int i = 0; i < numEnvMaps; i++)
		{
			tIndex = 0;
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					for (int c = 0; c < channelSize; c++)
					{
						float d;
						file.read((char*)&d, 4);
						emData[index] = d;
						index++;
						tIndex++;
					}
				}
			}
		}


		int vIndex = 0;
		for (int i = 0; i < scene->numEntities(); i++)
		{
			Model_obj* m = (Model_obj*)scene->getEntity(i);
			m->shader()->addBufferTexture((void*)emData, texSize, sizeof(GLfloat), "environmentMap", GL_RGB32F, -1);
			m->shader()->addUniform(&w, "environmentMapN", UNIFORM_INT_1, false);
			m->shader()->addUniform(&h, "environmentMapM", UNIFORM_INT_1, false);

			GLfloat* vIndexArray = new GLfloat[m->faceList().size() * 3];
			for (int j = 0; j < m->faceList().size() * 3; j++)
			{
				vIndexArray[j] = (float)vIndex;
				vIndex++;
			}
			m->shader()->addAttributeBuffer((void*)vIndexArray, m->faceList().size() * 3, sizeof(float), GL_FLOAT, "vertIndex", 7, 1);
			delete vIndexArray;
		}
		delete emData;
	}
	else if (scene->shadingType == SHADER_TYPE_SUBSURFACE_SCATTERING_DIFFUSE)
	{
		std::vector<Texture_obj<GLfloat>*> radTexVec;

		int numTex;
		file.read((char*)&numTex, 4);
		for (int i = 0; i < numTex; i++)
		{
			Texture_obj<GLfloat>* radTex = new Texture_obj<GLfloat>();

			file.read((char*)&radTex->width, 4);
			file.read((char*)&radTex->height, 4);
			file.read((char*)&radTex->channelSize, 4);
			radTex->buf = new GLfloat[radTex->width*radTex->height*radTex->channelSize];

			int tIndex = 0;
			for (int y = 0; y < radTex->height; y++)
			{
				for (int x = 0; x < radTex->width; x++)
				{
					for (int c = 0; c < radTex->channelSize; c++)
					{
						GLfloat d;
						file.read((char*)&d, 4);
						radTex->buf[tIndex] = d;
						tIndex++;
					}
				}
			}
			radTexVec.push_back(radTex);
		}

		for (int i = 0; i < numTex; i++)
		{
			//((Model_obj*)(scene->getEntity(i)))->shader()->addTexture((void*)radTexVec[i]->buf, radTexVec[i]->width, radTexVec[i]->height, "subsurfaceScatteringRadiance", GL_FLOAT, GL_RGBA, -1);
			((Model_obj*)(scene->getEntity(i)))->shader()->addTextureArray(radTexVec, 1, "subsurfaceScatteringRadiance", -1);
		}
	}
	else if (scene->shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE || scene->shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
	{
		scene->addPhotonMap(true);

		for (int i = 0; i < scene->numEntities(); i++)
		{
			Model_obj* m = (Model_obj*)scene->getEntity(i);
			int transmittance;
			if (scene->shadingType == SHADER_TYPE_PHOTON_MAP_PROJECTION_TEXTURE)
			{
				transmittance = 0;
				m->shader()->addUniform(&transmittance, "transmittance", UNIFORM_INT_1, false);
			}
			else if(scene->shadingType == SHADER_TYPE_PHOTON_MAP_TRANSMITTANCE_PROJECTION_TEXTURE)
			{
				transmittance = 1;
				m->shader()->addUniform(&transmittance, "transmittance", UNIFORM_INT_1, false);
			}
		}
	}
	else if (scene->shadingType == SHADER_TYPE_NORMAL_DEPTH_PROJECTION_TEXTURE)
	{
		for (int i = 0; i < scene->numEntities(); i++)
		{
			((Model_obj*)scene->getEntity(i))->addFaceListTexture();
		}
	}
	else if (scene->shadingType == SHADER_TYPE_REFLECTIVE_SHADOW_MAP)
	{
		scene->addReflectiveShadowMap();
	}
	else if (scene->shadingType == SHADER_TYPE_INSTANT_RADIOSITY)
	{
		scene->addIRShadowMap();
	}

	file.close();
	scene->updateScene();
	scene->preRender();
	return scene;
}