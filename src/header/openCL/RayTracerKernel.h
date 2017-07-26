#pragma once

#include "../../header/openCL/ClKernel.h"
#include "../../header/openCL/ClContext.h"
#include "../../header/data structures/MeshHierarchyKd.h"
#include "../../header/shading utils/PhotonMap.h"
#include "../../header/entities/LightEntity.h"

#define RAY_TRACER_KERNEL_SOURCE "../bin/kernels/rayTracer.cl"

struct IrradianceCache
{
	IrradianceCache();
	float irradiance[3];
	float rotationGrad[3];
	float translationGrad[3];
};

class RayTracerKernel : public ClKernel
{
public:
	RayTracerKernel(ClContext* context, std::vector<MaterialFace*> &materialFaces, std::vector<Material*> &materialList, std::vector<LightEntity*> &lights);
	virtual ~RayTracerKernel();

	void createBuffers(std::vector<MaterialFace*> &materialFaces, std::vector<Material*> &materialList, std::vector<LightEntity*> &lights);

	void createMaterialBuffers(std::vector<Material*> &materialList);
	void createFaceBuffers(std::vector<MaterialFace*> &materialFaces);
	void createLightBuffers(std::vector<LightEntity*> &lights);
	void createKdBuffers(std::vector<MaterialFace*> &materialFaces);
	void createTextureBuffers(std::vector<Material*> &materialList);

	void writeRayBuffers();
	void writeFaceIndexBuffer();
	void clearSampleWeightBuffer();

	void addRayTracerKernelArgs();

	void readRadiance();
	void readFaceHitIndex();
	void readPosition();
	void readDirection();
	void readSampleWeight();
	void averageRadiance(int numRays, float* radianceResult);
	float* radiance;

	void enqueueRayTraceKernel(int numSamples);
	void enqueueRayTraceUpdateKernel(int numSamples);
	void enqueueDirectRadianceKernel(int numSamples);
	void enqueueStochasticRayTraceRadianceKernel(int numSamples);
	void enqueueDirectRadianceCacheKernel(int numSamples);
	void enqueueStochasticRayTraceRadianceCacheKernel(int numSamples);

	float* rayTraceImage(int imageWidth, int imageHeight, float aspectRatio, float fov);
	std::vector<IrradianceCache*> RayTracerKernel::rayTracePoints(std::vector<Face*> &faces, float maxFaceArea);
	void rayTracePoints(std::vector<Vertex*> &samplePoints, std::vector<Vertex*> &outgoingDirection, std::vector<Vertex*> &normalPoints, std::vector<int> &faceHitIndex, std::vector<Vertex*> &lightPosition, float** inputParams, int* inputSize, float** outputParams, int* outputSize, int* numSamples);
	
	void computeIrradianceCachePoints(std::vector<Face*> &faceList, float maxArea);
	void copyRadianceToImage(int imageWidth, int imageHeight, float* image);
	void clampRadiance(int imageWidth, int imageHeight, float* image);
	void clampRadiance(std::vector<IrradianceCache*> irr);

	const int numRayTracingIters = 100;

private:
	int __numLights;
	cl_mem __position;
	cl_mem __direction;
	cl_mem __facesFloat;
	cl_mem __facesInt;
	cl_mem __kdTexFloat;
	cl_mem __kdTexInt;
	cl_mem __material;
	cl_mem __pointLight;
	cl_mem __raidance;
	cl_mem __faceIndexHit;
	cl_mem __sampleWeight;
	cl_mem __colorTexture;
	cl_mem __bumpMapTexture;

	int __width;
	int __height;

	int __radianceBufferSize;
	int __positionBufferSize;
	int __directionBufferSize;
	int __faceIndexBufferSize;
	int __sampleWeightBufferSize;
	float* __positionArray;
	float* __directionArray;
	int* __faceIndexHitArray;
	float* __sampleWeightArray;
};