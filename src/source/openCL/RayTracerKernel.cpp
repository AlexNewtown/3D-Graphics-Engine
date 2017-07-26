#include "../../header/openCL/RayTracerKernel.h"

RayTracerKernel::RayTracerKernel(ClContext* context, std::vector<MaterialFace*> &materialFaces, std::vector<Material*> &materialList, std::vector<LightEntity*> &lights) : ClKernel(RAY_TRACER_KERNEL_SOURCE, context, std::vector<std::string>())
{
	/*
	1. create mesh hierarchy
	2. add mesh hierarchy textures
	3. add materials
	*/

	createKernel("rayCastUpdate");

	createKernel("computeDirectRadiance");
	createKernel("stochasticRayTraceRadiance");

	createKernel("computeDirectRadianceCache");
	createKernel("stochasticRayTraceRadianceCache");

	__position = NULL;
	__direction = NULL;
	__facesFloat = NULL;
	__facesInt = NULL;
	__kdTexFloat = NULL;
	__kdTexInt = NULL;
	__material = NULL;
	__pointLight = NULL;
	__raidance = NULL;

	createBuffers(materialFaces, materialList, lights);
	addRayTracerKernelArgs();
}

RayTracerKernel::~RayTracerKernel()
{
	if(!__position) clReleaseMemObject(__position);
	if (!__direction) clReleaseMemObject(__direction);
	if (!__facesFloat) clReleaseMemObject(__facesFloat);
	if (!__facesInt) clReleaseMemObject(__facesInt);
	if (!__kdTexFloat) clReleaseMemObject(__kdTexFloat);
	if (!__kdTexInt) clReleaseMemObject(__kdTexInt);
	if (!__material) clReleaseMemObject(__material);
	if (!__pointLight) clReleaseMemObject(__pointLight);
	if (!__raidance) clReleaseMemObject(__raidance);
	if (!__faceIndexHit) clReleaseMemObject(__faceIndexHit);
	if (!__sampleWeight) clReleaseMemObject(__sampleWeight);
	if (!__colorTexture) clReleaseMemObject(__colorTexture);
	if (!__bumpMapTexture) clReleaseMemObject(__bumpMapTexture);
}

void RayTracerKernel::enqueueRayTraceKernel(int numSamples)
{
	//enqueueKernel(0, numSamples, 1);
}

void RayTracerKernel::enqueueRayTraceUpdateKernel(int numSamples)
{
	enqueueKernel(0, numSamples, 1);
}

void RayTracerKernel::enqueueDirectRadianceKernel(int numSamples)
{
	enqueueKernel(1, numSamples, 1);
}

void RayTracerKernel::enqueueStochasticRayTraceRadianceKernel(int numSamples)
{
	enqueueKernel(2, numSamples, 1);
}

void RayTracerKernel::enqueueDirectRadianceCacheKernel(int numSamples)
{
	enqueueKernel(3, numSamples, 1);
}

void RayTracerKernel::enqueueStochasticRayTraceRadianceCacheKernel(int numSamples)
{
	enqueueKernel(4, numSamples, 1);
}

float* RayTracerKernel::rayTraceImage(int imageWidth, int imageHeight, float aspectRatio, float fov)
{
	__width = imageWidth;
	__height = imageHeight;

	int radianceTexSize = imageWidth * imageHeight * 3;
	float* image = new float[imageWidth*imageHeight * 4];
	float* imageDirectRadiance = new float[imageWidth*imageHeight * 4];

	__radianceBufferSize = radianceTexSize*sizeof(float);
	__positionBufferSize = radianceTexSize*sizeof(float);
	__directionBufferSize = radianceTexSize*sizeof(float);
	__faceIndexBufferSize = imageWidth * imageHeight * sizeof(float);
	__sampleWeightBufferSize = imageWidth*imageHeight*sizeof(float);

	int numThreads = imageWidth * imageHeight;

	int index = 0;
	for (int x = 0; x < imageWidth; x++)
	{
		for (int y = 0; y < imageHeight; y++)
		{
			image[4*index] = 0.0;
			image[4*index + 1] = 0.0;
			image[4*index + 2] = 0.0;
			image[4*index + 3] = 1.0;
			
			imageDirectRadiance[4 * index] = 0.0;
			imageDirectRadiance[4 * index + 1] = 0.0;
			imageDirectRadiance[4 * index + 2] = 0.0;
			imageDirectRadiance[4 * index + 3] = 1.0;
			index++;
		}
	}

	radiance = new float[radianceTexSize];
	__positionArray = new float[radianceTexSize];
	__directionArray = new float[radianceTexSize];
	__faceIndexHitArray = new int[imageWidth * imageHeight];
	__sampleWeightArray = new float[imageWidth*imageHeight];

	cl_int ret;
	__raidance = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, radianceTexSize*sizeof(float), NULL, &ret);
	__position = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, radianceTexSize*sizeof(float), NULL, &ret);
	__direction = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, radianceTexSize*sizeof(float), NULL, &ret);
	__faceIndexHit = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __faceIndexBufferSize, NULL, &ret);
	__sampleWeight = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __sampleWeightBufferSize, NULL, &ret);


	addRayTracerKernelArgs();

	float imageSpacingX = 1.0 / (float)imageWidth;
	float imageSpacingY = 1.0 / (float)imageHeight;
	float pos[3];
	float dir[3];

	index = 0;
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			float imagePlaneX = (2.0*(float)x*imageSpacingX - 1.0) * aspectRatio * fov;
			float imagePlaneY = (1.0 - 2.0*(float)y*imageSpacingY) * fov;

			pos[0] = 0.0;
			pos[1] = 0.0;
			pos[2] = 0.0;

			dir[0] = imagePlaneX;
			dir[1] = imagePlaneY;
			dir[2] = -1;
			normalize(dir);

			__positionArray[3 * index] = pos[0];
			__positionArray[3 * index + 1] = pos[1];
			__positionArray[3 * index + 2] = pos[2];

			__directionArray[3 * index] = dir[0];
			__directionArray[3 * index + 1] = dir[1];
			__directionArray[3 * index + 2] = dir[2];

			index++;
		}
	}

	writeRayBuffers();
	clearSampleWeightBuffer();

	enqueueRayTraceUpdateKernel(numThreads);
	enqueueDirectRadianceKernel(numThreads);
	copyRadianceToImage(imageWidth, imageHeight, imageDirectRadiance);

	
	bool direct = false;

	for (int iter = 0; iter < numRayTracingIters; iter++)
	{
		time_t timer;
		time(&timer);
		int seed = timer % 5000;

		addKernelArg(2, 11, sizeof(cl_int), (void*)&seed);
		//enqueueRayTraceKernel(numThreads);
		enqueueStochasticRayTraceRadianceKernel(numThreads);
		readSampleWeight();
		readRadiance();

		index = 0;
		for (int y = 0; y < imageHeight; y++)
		{
			for (int x = 0; x < imageWidth; x++)
			{
				image[index * 4] += radiance[index * 3];
				image[index * 4 + 1] += radiance[index * 3 + 1];
				image[index * 4 + 2] += radiance[index * 3 + 2];
				index++;
			}
		}
	}

	// normalize
	readSampleWeight();
	index = 0;
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			image[index * 4] = image[index * 4] / (float)__sampleWeightArray[index];
			image[index * 4 + 1] = image[index * 4 + 1] / (float)__sampleWeightArray[index];
			image[index * 4 + 2] = image[index * 4 + 2] / (float)__sampleWeightArray[index];
			

			if (direct)
			{
				image[index * 4] += imageDirectRadiance[index * 4];
				image[index * 4 + 1] += imageDirectRadiance[index * 4 + 1];
				image[index * 4 + 2] += imageDirectRadiance[index * 4 + 2];
			}

			image[index * 4 + 3] = 1.0;
			index++;
		}
	}
	clampRadiance(imageWidth, imageHeight, image);
	
	delete radiance;
	delete __positionArray;
	delete __directionArray;
	delete __faceIndexHitArray;
	delete __sampleWeightArray;

	clReleaseMemObject(__raidance);
	clReleaseMemObject(__position);
	clReleaseMemObject(__direction);
	clReleaseMemObject(__faceIndexHit);
	clReleaseMemObject(__sampleWeight);

	__raidance = NULL;
	__position = NULL;
	__direction = NULL;
	__faceIndexHit = NULL;
	__sampleWeight = NULL;
	return image;
}

void RayTracerKernel::computeIrradianceCachePoints(std::vector<Face*> &faceList, float maxArea)
{
	bool done = false;
	int startingSearch = 0;
	while (!done)
	{
		for (int i = startingSearch; i < faceList.size(); i++)
		{
			Face* currentFace = faceList[i];
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



				/* COPY THE NORMALS */
				interpolateNormals(currentFace, fsub1->vertex[0]->pos, fsub1->normal[0]->pos);
				interpolateNormals(currentFace, fsub1->vertex[1]->pos, fsub1->normal[1]->pos);
				interpolateNormals(currentFace, fsub1->vertex[2]->pos, fsub1->normal[2]->pos);

				interpolateNormals(currentFace, fsub2->vertex[0]->pos, fsub2->normal[0]->pos);
				interpolateNormals(currentFace, fsub2->vertex[1]->pos, fsub2->normal[1]->pos);
				interpolateNormals(currentFace, fsub2->vertex[2]->pos, fsub2->normal[2]->pos);

				interpolateNormals(currentFace, fsub3->vertex[0]->pos, fsub3->normal[0]->pos);
				interpolateNormals(currentFace, fsub3->vertex[1]->pos, fsub3->normal[1]->pos);
				interpolateNormals(currentFace, fsub3->vertex[2]->pos, fsub3->normal[2]->pos);

				interpolateNormals(currentFace, fsub4->vertex[0]->pos, fsub4->normal[0]->pos);
				interpolateNormals(currentFace, fsub4->vertex[1]->pos, fsub4->normal[1]->pos);
				interpolateNormals(currentFace, fsub4->vertex[2]->pos, fsub4->normal[2]->pos);

				/*
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
				*/



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

				fsub1->index = currentFace->index;
				fsub2->index = currentFace->index;
				fsub3->index = currentFace->index;
				fsub4->index = currentFace->index;

				//removeFace(i);
				
				faceList.erase(faceList.begin() + i);
				
				faceList.push_back(fsub1);
				faceList.push_back(fsub2);
				faceList.push_back(fsub3);
				faceList.push_back(fsub4);

			}

			for (int j = 0; j < faceList.size(); j++)
			{
				if (faceList[i]->area()  > maxArea)
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

void RayTracerKernel::rayTracePoints(std::vector<Vertex*> &samplePoints, std::vector<Vertex*> &outgoingDirection, std::vector<Vertex*> &normalPoints, std::vector<int> &faceHitIndex, std::vector<Vertex*> &lightPosition, float** inputParams, int* inputSize, float** outputParams, int* outputSize, int* numSamples)
{
	int numVertices = samplePoints.size();
	*numSamples = numVertices;

	__radianceBufferSize = numVertices * 3 * sizeof(float);
	__positionBufferSize = numVertices * 3 * sizeof(float);
	__directionBufferSize = numVertices * 3 * sizeof(float);
	__faceIndexBufferSize = numVertices*sizeof(float);
	__sampleWeightBufferSize = numVertices*sizeof(float);

	int numThreads = numVertices;

	*inputSize = 3;
	*outputSize = 3;

	int inputArrayBufferSize = (*inputSize)*numVertices;
	int outputArrayBufferSize = (*outputSize)*numVertices;
	float* inputArray = new float[inputArrayBufferSize];
	float* outputArray = new float[outputArrayBufferSize];

	*inputParams = inputArray;
	*outputParams = outputArray;

	radiance = new float[numVertices * 3];
	__positionArray = new float[numVertices * 3];
	__directionArray = new float[numVertices * 3];
	__faceIndexHitArray = new int[numVertices];
	__sampleWeightArray = new float[numVertices];

	cl_int ret;
	__raidance = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __radianceBufferSize, NULL, &ret);
	__position = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __positionBufferSize, NULL, &ret);
	__direction = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __directionBufferSize, NULL, &ret);
	__faceIndexHit = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __faceIndexBufferSize, NULL, &ret);
	__sampleWeight = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __sampleWeightBufferSize, NULL, &ret);

	addRayTracerKernelArgs();

	int inputIndex = 0;
	int outputIndex = 0;
	for (int i = 0; i < numVertices; i++)
	{
		__positionArray[3 * i] = samplePoints[i]->pos[0];
		__positionArray[3 * i + 1] = samplePoints[i]->pos[1];
		__positionArray[3 * i + 2] = samplePoints[i]->pos[2];

		__directionArray[3 * i] = outgoingDirection[i]->pos[0];
		__directionArray[3 * i + 1] = outgoingDirection[i]->pos[1];
		__directionArray[3 * i + 2] = outgoingDirection[i]->pos[2];

		__faceIndexHitArray[i] = faceHitIndex[i];


		//position
		inputArray[inputIndex] = samplePoints[i]->pos[0];
		inputArray[inputIndex + 1] = samplePoints[i]->pos[1];
		inputArray[inputIndex + 2] = samplePoints[i]->pos[2];

		/*
		//direction
		inputArray[inputIndex + 3] = outgoingDirection[i]->pos[0];
		inputArray[inputIndex + 4] = outgoingDirection[i]->pos[1];
		inputArray[inputIndex + 5] = outgoingDirection[i]->pos[2];

		//normal
		inputArray[inputIndex + 6] = normalPoints[i]->pos[0];
		inputArray[inputIndex + 7] = normalPoints[i]->pos[1];
		inputArray[inputIndex + 8] = normalPoints[i]->pos[2];

		//lightPosition
		inputArray[inputIndex + 9] = lightPosition[i]->pos[0];
		inputArray[inputIndex + 10] = lightPosition[i]->pos[1];
		inputArray[inputIndex + 11] = lightPosition[i]->pos[2];
		*/
		inputIndex = inputIndex + *inputSize;

		outputArray[outputIndex] = 0.0;
		outputArray[outputIndex + 1] = 0.0;
		outputArray[outputIndex + 2] = 0.0;
		outputIndex = outputIndex + 3;
	}

	writeRayBuffers();
	writeFaceIndexBuffer();
	clearSampleWeightBuffer();


	int index = 0;
	for (int iter = 0; iter < numRayTracingIters; iter++)
	{
		time_t timer;
		time(&timer);
		int seed = timer % 5000;

		addKernelArg(4, 11, sizeof(cl_int), (void*)&seed);
		enqueueStochasticRayTraceRadianceCacheKernel(numThreads);
		readRadiance();

		index = 0;
		for (int i = 0; i < numVertices; i++)
		{
			outputArray[3*i] += radiance[3*i];
			outputArray[3*i + 1] += radiance[3*i + 1];
			outputArray[3*i + 2] += radiance[3*i + 2];
		}
	}


	// normalize
	readSampleWeight();
	for (int i = 0; i < numVertices; i++)
	{
		outputArray[3*i] = outputArray[3*i] / (float)__sampleWeightArray[i];
		outputArray[3*i + 1] = outputArray[3*i + 1] / (float)__sampleWeightArray[i];
		outputArray[3*i + 2] = outputArray[3*i + 2] / (float)__sampleWeightArray[i];
	}

	
	delete radiance;
	delete __positionArray;
	delete __directionArray;
	delete __faceIndexHitArray;
	delete __sampleWeightArray;

	clReleaseMemObject(__raidance);
	clReleaseMemObject(__position);
	clReleaseMemObject(__direction);
	clReleaseMemObject(__faceIndexHit);
	clReleaseMemObject(__sampleWeight);

	__raidance = NULL;
	__position = NULL;
	__direction = NULL;
	__faceIndexHit = NULL;
	__sampleWeight = NULL;
}

std::vector<IrradianceCache*> RayTracerKernel::rayTracePoints(std::vector<Face*> &faces, float maxFaceArea)
{
	//computeIrradianceCachePoints(faces, maxFaceArea);
	int numFaces = faces.size();
	int numVertices = numFaces * 3;

	__radianceBufferSize = numVertices*3*sizeof(float);
	__positionBufferSize = numVertices*3*sizeof(float);
	__directionBufferSize = numVertices*3*sizeof(float);
	__faceIndexBufferSize = numVertices*sizeof(float);
	__sampleWeightBufferSize = numVertices*sizeof(float);

	int numThreads = numFaces*3;

	std::vector<IrradianceCache*> irradianceCache;

	int index = 0;
	for (int x = 0; x < numVertices; x++)
	{
		IrradianceCache* ir = new IrradianceCache();
		irradianceCache.push_back(ir);
	}

	radiance = new float[numVertices*3];
	__positionArray = new float[__positionBufferSize];
	__directionArray = new float[__directionBufferSize];
	__faceIndexHitArray = new int[numVertices];
	__sampleWeightArray = new float[numVertices];

	cl_int ret;
	__raidance = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __radianceBufferSize, NULL, &ret);
	__position = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __positionBufferSize, NULL, &ret);
	__direction = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __directionBufferSize, NULL, &ret);
	__faceIndexHit = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __faceIndexBufferSize, NULL, &ret);
	__sampleWeight = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, __sampleWeightBufferSize, NULL, &ret);


	addRayTracerKernelArgs();

	float pos[3];
	float dir[3];

	index = 0;
	for (int i = 0; i < numFaces; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			__positionArray[3 * index] = faces[i]->vertex[j]->pos[0];
			__positionArray[3 * index + 1] = faces[i]->vertex[j]->pos[1];
			__positionArray[3 * index + 2] = faces[i]->vertex[j]->pos[2];

			__directionArray[3 * index] = faces[i]->normal[j]->pos[0];
			__directionArray[3 * index + 1] = faces[i]->normal[j]->pos[1];
			__directionArray[3 * index + 2] = faces[i]->normal[j]->pos[2];

			__faceIndexHitArray[index] = faces[i]->index;
			index++;
		}
	}

	writeRayBuffers();
	writeFaceIndexBuffer();
	clearSampleWeightBuffer();



	for (int iter = 0; iter < numRayTracingIters; iter++)
	{
		time_t timer;
		time(&timer);
		int seed = timer % 5000;

		addKernelArg(4, 11, sizeof(cl_int), (void*)&seed);
		enqueueStochasticRayTraceRadianceCacheKernel(numThreads);
		readRadiance();

		index = 0;
		for (int i = 0; i < numVertices; i++)
		{
			irradianceCache[i]->irradiance[0] += radiance[index * 3];
			irradianceCache[i]->irradiance[1] += radiance[index * 3 + 1];
			irradianceCache[i]->irradiance[2] += radiance[index * 3 + 2];
			index++;
		}

	}


	// normalize
	readSampleWeight();
	index = 0;
	for (int i = 0; i < numVertices; i++)
	{
		irradianceCache[i]->irradiance[0] = irradianceCache[i]->irradiance[0] / (float)__sampleWeightArray[index];
		irradianceCache[i]->irradiance[1] = irradianceCache[i]->irradiance[1] / (float)__sampleWeightArray[index];
		irradianceCache[i]->irradiance[2] = irradianceCache[i]->irradiance[2] / (float)__sampleWeightArray[index];
		index++;
	}

	index = 0;

	if (false)
	{
		enqueueDirectRadianceCacheKernel(numThreads);
		readRadiance();
		for (int i = 0; i < numVertices; i++)
		{
			irradianceCache[i]->irradiance[0] += radiance[index * 3];
			irradianceCache[i]->irradiance[1] += radiance[index * 3 + 1];
			irradianceCache[i]->irradiance[2] += radiance[index * 3 + 2];
			index++;
		}
	}

	
	delete radiance;
	delete __positionArray;
	delete __directionArray;
	delete __faceIndexHitArray;
	delete __sampleWeightArray;

	return irradianceCache;
}

void RayTracerKernel::copyRadianceToImage(int imageWidth, int imageHeight, float* image)
{
	int index = 0;
	readRadiance();
	index = 0;
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			image[index * 4] = radiance[index * 3];
			image[index * 4 + 1] = radiance[index * 3 + 1];
			image[index * 4 + 2] = radiance[index * 3 + 2];
			index++;
		}
	}
}

void RayTracerKernel::clampRadiance(int imageWidth, int imageHeight, float* image)
{
	int index = 0;
	index = 0;
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			image[index * 4] = minimum(image[index * 4], 1.0);
			image[index * 4 + 1] = minimum(image[index * 4 + 1], 1.0);
			image[index * 4 + 2] = minimum(image[index * 4 + 2], 1.0);
			index++;
		}
	}
}

void RayTracerKernel::clampRadiance(std::vector<IrradianceCache*> irr)
{
	int index = 0;
	index = 0;
	
	for (int i = 0; i < irr.size(); i++)
	{
		irr[i]->irradiance[0] = minimum(irr[i]->irradiance[0], 1.0);
		irr[i]->irradiance[1] = minimum(irr[i]->irradiance[1], 1.0);
		irr[i]->irradiance[2] = minimum(irr[i]->irradiance[2], 1.0);
	}
}

void RayTracerKernel::averageRadiance(int numRays, float* incidentRadiance)
{
	readRadiance();
	float sum[] = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < numRays; i++)
	{
		sum[0] += radiance[i * 3];
		sum[1] += radiance[i * 3 + 1];
		sum[2] += radiance[i * 3 + 2];
	}
	incidentRadiance[0] = sum[0] / float(numRays);
	incidentRadiance[1] = sum[1] / float(numRays);
	incidentRadiance[2] = sum[2] / float(numRays);
}

void RayTracerKernel::writeRayBuffers()
{
	cl_int ret = clEnqueueWriteBuffer(__context->commandQueue, __position, CL_TRUE, 0, __positionBufferSize, __positionArray, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __direction, CL_TRUE, 0, __directionBufferSize, __directionArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::writeFaceIndexBuffer()
{
	cl_int ret = clEnqueueWriteBuffer(__context->commandQueue, __faceIndexHit, CL_TRUE, 0, __faceIndexBufferSize, __faceIndexHitArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::createBuffers(std::vector<MaterialFace*> &materialFaces, std::vector<Material*> &materialList, std::vector<LightEntity*> &lights)
{
	cl_int ret;
	createMaterialBuffers(materialList);
	createFaceBuffers(materialFaces);
	createLightBuffers(lights);
	createKdBuffers(materialFaces);
	createTextureBuffers(materialList);
}

void RayTracerKernel::createMaterialBuffers(std::vector<Material*> &materialList)
{
	int materialSize = 16;	// 16 floats
	int numMaterials = materialList.size();

	float* materialArray = new float[materialSize*numMaterials];


	int index = 0;
	for (int i = 0; i < numMaterials; i++)
	{
		Material* mat = materialList[i];
		materialArray[index] = mat->ka[0];
		materialArray[index + 1] = mat->ka[1];
		materialArray[index + 2] = mat->ka[2];

		materialArray[index + 3] = mat->kd[0];
		materialArray[index + 4] = mat->kd[1];
		materialArray[index + 5] = mat->kd[2];

		materialArray[index + 6] = mat->ks[0];
		materialArray[index + 7] = mat->ks[1];
		materialArray[index + 8] = mat->ks[2];

		materialArray[index + 9] = mat->tf[0];
		materialArray[index + 10] = mat->tf[1];
		materialArray[index + 11] = mat->tf[2];

		materialArray[index + 12] = mat->ni;
		materialArray[index + 13] = mat->ns;
		materialArray[index + 14] = mat->d;
		materialArray[index + 15] = mat->sharpness;

		index = index + materialSize;
	}

	cl_int ret;
	__material = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, materialSize*numMaterials*sizeof(float), NULL, &ret);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __material, CL_TRUE, 0, materialSize*numMaterials*sizeof(float), materialArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
	delete materialArray;
}
void RayTracerKernel::createFaceBuffers(std::vector<MaterialFace*> &materialFaces)
{
	int numFaces = materialFaces.size();

	int facesIntSize = 5;
	int facesFloatSize = 24;

	int* facesIntArray = new int[facesIntSize*numFaces];

	int index = 0;
	for (int i = 0; i < numFaces; i++)
	{
		MaterialFace* mf = materialFaces[i];
		facesIntArray[index] = i;
		facesIntArray[index + 1] = mf->materialIndex;
		facesIntArray[index + 2] = mf->brdfIndex;
		facesIntArray[index + 3] = mf->colorTexIndex;
		facesIntArray[index + 4] = mf->bumpTexIndex;
		index = index + facesIntSize;
	}
	cl_int ret;
	__facesInt = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, numFaces*facesIntSize*sizeof(int), NULL, &ret);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __facesInt, CL_TRUE, 0, numFaces*facesIntSize*sizeof(int), facesIntArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
	delete facesIntArray;
	

	float* facesFloatArray = new float[facesFloatSize*numFaces];

	index = 0;
	for (int i = 0; i < numFaces; i++)
	{
		MaterialFace* mf = materialFaces[i];
		facesFloatArray[index] = mf->vertex[0]->pos[0];
		facesFloatArray[index + 1] = mf->vertex[0]->pos[1];
		facesFloatArray[index + 2] = mf->vertex[0]->pos[2];

		facesFloatArray[index + 3] = mf->vertex[1]->pos[0];
		facesFloatArray[index + 4] = mf->vertex[1]->pos[1];
		facesFloatArray[index + 5] = mf->vertex[1]->pos[2];

		facesFloatArray[index + 6] = mf->vertex[2]->pos[0];
		facesFloatArray[index + 7] = mf->vertex[2]->pos[1];
		facesFloatArray[index + 8] = mf->vertex[2]->pos[2];



		facesFloatArray[index + 9] = mf->normal[0]->pos[0];
		facesFloatArray[index + 10] = mf->normal[0]->pos[1];
		facesFloatArray[index + 11] = mf->normal[0]->pos[2];

		facesFloatArray[index + 12] = mf->normal[1]->pos[0];
		facesFloatArray[index + 13] = mf->normal[1]->pos[1];
		facesFloatArray[index + 14] = mf->normal[1]->pos[2];

		facesFloatArray[index + 15] = mf->normal[2]->pos[0];
		facesFloatArray[index + 16] = mf->normal[2]->pos[1];
		facesFloatArray[index + 17] = mf->normal[2]->pos[2];


		if (mf->texCoord[0] != NULL)
		{
			facesFloatArray[index + 18] = mf->texCoord[0]->pos[0];
			facesFloatArray[index + 19] = mf->texCoord[0]->pos[1];
		}
		else
		{
			facesFloatArray[index + 18] = 0.0;
			facesFloatArray[index + 19] = 0.0;
		}

		if (mf->texCoord[1] != NULL)
		{
			facesFloatArray[index + 20] = mf->texCoord[1]->pos[0];
			facesFloatArray[index + 21] = mf->texCoord[1]->pos[1];
		}
		else
		{
			facesFloatArray[index + 20] = 0.0;
			facesFloatArray[index + 21] = 0.0;
		}

		if (mf->texCoord[2] != NULL)
		{
			facesFloatArray[index + 22] = mf->texCoord[2]->pos[0];
			facesFloatArray[index + 23] = mf->texCoord[2]->pos[1];
		}
		else
		{
			facesFloatArray[index + 22] = 0.0;
			facesFloatArray[index + 23] = 0.0;
		}

		index = index + facesFloatSize;
	}

	__facesFloat = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, numFaces*facesFloatSize*sizeof(float), NULL, &ret);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __facesFloat, CL_TRUE, 0, numFaces*facesFloatSize*sizeof(float), facesFloatArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
	delete facesFloatArray;
}
void RayTracerKernel::createLightBuffers(std::vector<LightEntity*> &lights)
{
	int numLights = lights.size();
	__numLights = numLights;
	int lightSize = 4;

	float* lightArray = new float[numLights*lightSize];
	int index = 0;
	for (int i = 0; i < numLights; i++)
	{
		LightEntity* le = lights[i];
		lightArray[index] = le->x();
		lightArray[index + 1] = le->y();
		lightArray[index + 2] = le->z();
		lightArray[index + 3] = le->intensity;
		index = index + lightSize;
	}
	cl_int ret;
	__pointLight = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, numLights*lightSize*sizeof(float), NULL, &ret);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __pointLight, CL_TRUE, 0, numLights*lightSize*sizeof(float), lightArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
	delete lightArray;
}
void RayTracerKernel::createKdBuffers(std::vector<MaterialFace*> &materialFaces)
{
	int numFaces = materialFaces.size();
	std::vector<Face*> facesCopy;
	for (int i = 0; i < numFaces; i++)
	{
		facesCopy.push_back((Face*)materialFaces[i]);
	}

	MeshHierarchyKd* mh = new MeshHierarchyKd(facesCopy, facesCopy.size());
	KdTreeTexture* kdtex = createKdTreeTextureBuffers(mh);
	kdtex->texInt[0].totalNumFaces = materialFaces.size();
	int* kdTreeIntArray = (int*)kdtex->texInt;
	float* kdTreeFloatArray = (float*)kdtex->texFloat;

	cl_int ret;
	__kdTexFloat = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, kdtex->treeSize*sizeof(KdTreeTextureFloat), NULL, &ret);
	__kdTexInt = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, kdtex->treeSize*sizeof(KdTreeTextureInt), NULL, &ret);

	ret = clEnqueueWriteBuffer(__context->commandQueue, __kdTexFloat, CL_TRUE, 0, kdtex->treeSize*sizeof(KdTreeTextureFloat), kdTreeFloatArray, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __kdTexInt, CL_TRUE, 0, kdtex->treeSize*sizeof(KdTreeTextureInt), kdTreeIntArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
	for (int i = 0; i < numFaces; i++)
	{
		facesCopy.pop_back();
	}
	delete mh;
	delete kdtex;
}

void RayTracerKernel::createTextureBuffers(std::vector<Material*> &materialList)
{
	int numColorTextures = 0;
	int numBumpTextures = 0;
	int colorTextureSize = 0;
	int bumpTextureSize = 0;
	for (int i = 0; i < materialList.size(); i++)
	{
		Material* mat = materialList[i];
		if (mat->texture != NULL)
		{
			numColorTextures++;
			colorTextureSize += mat->texture->width*mat->texture->height*mat->texture->channelSize;
		}

		if (mat->bumpMapTexture != NULL)
		{
			numBumpTextures++;
			bumpTextureSize += mat->bumpMapTexture->width*mat->bumpMapTexture->height*mat->bumpMapTexture->channelSize;
		}
	}

	cl_int ret;
	if (numColorTextures > 0)
	{
		float* colorTex = new float[colorTextureSize];
		int colorTexIndex = 0;
		int maxColorWidth = 0;
		int maxColorHeight = 0;
		for (int i = 0; i < materialList.size(); i++)
		{
			Material* mat = materialList[i];
			if (mat->texture != NULL)
			{
				int bufferSize = mat->texture->width*mat->texture->height*mat->texture->channelSize;
				maxColorWidth = maximum(mat->texture->width, maxColorWidth);
				maxColorHeight = maximum(mat->texture->height, maxColorHeight);
				for (int j = 0; j < bufferSize; j++)
				{
					colorTex[colorTexIndex] = mat->texture->buf[j];
					colorTexIndex++;
				}
			}
		}

		cl_image_format imageFormat;
		imageFormat.image_channel_order = CL_RGBA;
		imageFormat.image_channel_data_type = CL_FLOAT;

		cl_image_desc imageDesc;
		imageDesc.image_type = CL_MEM_OBJECT_IMAGE3D;
		imageDesc.image_width = maxColorWidth;
		imageDesc.image_height = maxColorHeight;
		imageDesc.image_depth = numColorTextures;
		imageDesc.image_array_size = 0;
		imageDesc.image_row_pitch = 0;
		imageDesc.image_slice_pitch = 0;
		imageDesc.num_mip_levels = 0;
		imageDesc.num_samples = 0;
		imageDesc.buffer = NULL;

		__colorTexture = clCreateImage(__context->context, CL_MEM_READ_ONLY, &imageFormat, &imageDesc, colorTex, &ret);

		size_t rowPitchSize;
		clGetImageInfo(__colorTexture, CL_IMAGE_ROW_PITCH, sizeof(size_t), &rowPitchSize, NULL);

		size_t sliceSize;
		clGetImageInfo(__colorTexture, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &sliceSize, NULL);

		size_t origin[] = { 0.0,0.0,0.0 };
		size_t region[] = { maxColorWidth, maxColorHeight, numColorTextures };
		ret = clEnqueueWriteImage(__context->commandQueue, __colorTexture, CL_TRUE, origin, region, rowPitchSize, sliceSize, colorTex, 0, NULL, NULL);
		clFinish(__context->commandQueue);

		delete colorTex;
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_order = CL_RGBA;
		imageFormat.image_channel_data_type = CL_FLOAT;

		cl_image_desc imageDesc;
		imageDesc.image_type = CL_MEM_OBJECT_IMAGE3D;
		imageDesc.image_width = 1;
		imageDesc.image_height = 1;
		imageDesc.image_depth = 1;
		imageDesc.image_array_size = 0;
		imageDesc.image_row_pitch = 0;
		imageDesc.image_slice_pitch = 0;
		imageDesc.num_mip_levels = 0;
		imageDesc.num_samples = 0;
		imageDesc.buffer = NULL;

		__colorTexture = clCreateImage(__context->context, CL_MEM_READ_ONLY, &imageFormat, &imageDesc, NULL, &ret);
	}
	
	if (numBumpTextures > 0)
	{
		float* bumpTex = new float[bumpTextureSize];
		int bumpTexIndex = 0;

		int maxBumpWidth = 0;
		int maxBumpHeight = 0;
		for (int i = 0; i < materialList.size(); i++)
		{
			Material* mat = materialList[i];
			if (mat->bumpMapTexture != NULL)
			{
				int bufferSize = mat->bumpMapTexture->width*mat->bumpMapTexture->height*mat->bumpMapTexture->channelSize;
				maxBumpWidth = maximum(mat->texture->width, maxBumpWidth);
				maxBumpHeight = maximum(mat->texture->height, maxBumpHeight);
				for (int j = 0; j < bufferSize; j++)
				{
					bumpTex[bumpTexIndex] = mat->bumpMapTexture->buf[j];
					bumpTexIndex++;
				}
			}
		}

		cl_image_format imageFormat;
		imageFormat.image_channel_order = CL_INTENSITY;
		imageFormat.image_channel_data_type = CL_FLOAT;

		cl_image_desc imageDesc;
		imageDesc.image_type = CL_MEM_OBJECT_IMAGE3D;
		imageDesc.image_width = maxBumpWidth;
		imageDesc.image_height = maxBumpHeight;
		imageDesc.image_depth = numColorTextures;
		imageDesc.image_array_size = 0;
		imageDesc.image_row_pitch = 0;
		imageDesc.image_slice_pitch = 0;
		imageDesc.num_mip_levels = 0;
		imageDesc.num_samples = 0;
		imageDesc.buffer = __bumpMapTexture;

		__bumpMapTexture = clCreateImage(__context->context, CL_MEM_READ_ONLY, &imageFormat, &imageDesc, bumpTex, &ret);

		size_t rowPitchSize;
		clGetImageInfo(__bumpMapTexture, CL_IMAGE_ROW_PITCH, sizeof(size_t), &rowPitchSize, NULL);

		size_t sliceSize;
		clGetImageInfo(__bumpMapTexture, CL_IMAGE_SLICE_PITCH, sizeof(size_t), &sliceSize, NULL);

		size_t origin[] = { 0.0,0.0,0.0 };
		size_t region[] = { maxBumpWidth, maxBumpHeight, numBumpTextures };
		ret = clEnqueueWriteImage(__context->commandQueue, __bumpMapTexture, CL_TRUE, origin, region, rowPitchSize, sliceSize, bumpTex, 0, NULL, NULL);
		clFinish(__context->commandQueue);

		delete bumpTex;
	}
	else
	{
		cl_image_format imageFormat;
		imageFormat.image_channel_order = CL_INTENSITY;
		imageFormat.image_channel_data_type = CL_FLOAT;

		cl_image_desc imageDesc;
		imageDesc.image_type = CL_MEM_OBJECT_IMAGE3D;
		imageDesc.image_width = 1;
		imageDesc.image_height = 1;
		imageDesc.image_depth = 1;
		imageDesc.image_array_size = 0;
		imageDesc.image_row_pitch = 0;
		imageDesc.image_slice_pitch = 0;
		imageDesc.num_mip_levels = 0;
		imageDesc.num_samples = 0;
		imageDesc.buffer = __bumpMapTexture;
		__bumpMapTexture = clCreateImage(__context->context, CL_MEM_READ_ONLY, &imageFormat, &imageDesc, NULL, &ret);
	}

}

void RayTracerKernel::addRayTracerKernelArgs()
{
	/*ray cast update*/
	addKernelArg(0, 0, sizeof(cl_mem), (void*)&__position);
	addKernelArg(0, 1, sizeof(cl_mem), (void*)&__direction);
	addKernelArg(0, 2, sizeof(cl_mem), (void*)&__facesFloat);
	addKernelArg(0, 3, sizeof(cl_mem), (void*)&__facesInt);
	addKernelArg(0, 4, sizeof(cl_mem), (void*)&__kdTexFloat);
	addKernelArg(0, 5, sizeof(cl_mem), (void*)&__kdTexInt);
	addKernelArg(0, 6, sizeof(cl_mem), (void*)&__faceIndexHit);


	/*compute direct radiance*/
	addKernelArg(1, 0, sizeof(cl_mem), (void*)&__position);
	addKernelArg(1, 1, sizeof(cl_mem), (void*)&__direction);
	addKernelArg(1, 2, sizeof(cl_mem), (void*)&__facesFloat);
	addKernelArg(1, 3, sizeof(cl_mem), (void*)&__facesInt);
	addKernelArg(1, 4, sizeof(cl_mem), (void*)&__kdTexFloat);
	addKernelArg(1, 5, sizeof(cl_mem), (void*)&__kdTexInt);
	addKernelArg(1, 6, sizeof(cl_mem), (void*)&__material);
	addKernelArg(1, 7, sizeof(cl_mem), (void*)&__pointLight);
	addKernelArg(1, 8, sizeof(cl_int), (void*)&__numLights);
	addKernelArg(1, 9, sizeof(cl_mem), (void*)&__raidance);
	addKernelArg(1, 10, sizeof(cl_mem), (void*)&__faceIndexHit);
	addKernelArg(1, 11, sizeof(cl_mem), (void*)&__colorTexture);
	addKernelArg(1, 12, sizeof(cl_mem), (void*)&__bumpMapTexture);


	/*stochastic ray trace radiance*/
	addKernelArg(2, 0, sizeof(cl_mem), (void*)&__position);
	addKernelArg(2, 1, sizeof(cl_mem), (void*)&__direction);
	addKernelArg(2, 2, sizeof(cl_mem), (void*)&__facesFloat);
	addKernelArg(2, 3, sizeof(cl_mem), (void*)&__facesInt);
	addKernelArg(2, 4, sizeof(cl_mem), (void*)&__kdTexFloat);
	addKernelArg(2, 5, sizeof(cl_mem), (void*)&__kdTexInt);
	addKernelArg(2, 6, sizeof(cl_mem), (void*)&__material);
	addKernelArg(2, 7, sizeof(cl_mem), (void*)&__pointLight);
	addKernelArg(2, 8, sizeof(cl_int), (void*)&__numLights);
	addKernelArg(2, 9, sizeof(cl_mem), (void*)&__raidance);
	addKernelArg(2, 10, sizeof(cl_mem), (void*)&__faceIndexHit);
	addKernelArg(2, 12, sizeof(cl_mem), (void*)&__sampleWeight);
	addKernelArg(2, 13, sizeof(cl_mem), (void*)&__colorTexture);
	addKernelArg(2, 14, sizeof(cl_mem), (void*)&__bumpMapTexture);




	/*compute direct radiance for caches*/
	addKernelArg(3, 0, sizeof(cl_mem), (void*)&__position);
	addKernelArg(3, 1, sizeof(cl_mem), (void*)&__direction);
	addKernelArg(3, 2, sizeof(cl_mem), (void*)&__facesFloat);
	addKernelArg(3, 3, sizeof(cl_mem), (void*)&__facesInt);
	addKernelArg(3, 4, sizeof(cl_mem), (void*)&__kdTexFloat);
	addKernelArg(3, 5, sizeof(cl_mem), (void*)&__kdTexInt);
	addKernelArg(3, 6, sizeof(cl_mem), (void*)&__material);
	addKernelArg(3, 7, sizeof(cl_mem), (void*)&__pointLight);
	addKernelArg(3, 8, sizeof(cl_int), (void*)&__numLights);
	addKernelArg(3, 9, sizeof(cl_mem), (void*)&__raidance);
	addKernelArg(3, 10, sizeof(cl_mem), (void*)&__faceIndexHit);
	addKernelArg(3, 11, sizeof(cl_mem), (void*)&__colorTexture);
	addKernelArg(3, 12, sizeof(cl_mem), (void*)&__bumpMapTexture);


	/*compute stochastic ray trace radiance for caches*/
	addKernelArg(4, 0, sizeof(cl_mem), (void*)&__position);
	addKernelArg(4, 1, sizeof(cl_mem), (void*)&__direction);
	addKernelArg(4, 2, sizeof(cl_mem), (void*)&__facesFloat);
	addKernelArg(4, 3, sizeof(cl_mem), (void*)&__facesInt);
	addKernelArg(4, 4, sizeof(cl_mem), (void*)&__kdTexFloat);
	addKernelArg(4, 5, sizeof(cl_mem), (void*)&__kdTexInt);
	addKernelArg(4, 6, sizeof(cl_mem), (void*)&__material);
	addKernelArg(4, 7, sizeof(cl_mem), (void*)&__pointLight);
	addKernelArg(4, 8, sizeof(cl_int), (void*)&__numLights);
	addKernelArg(4, 9, sizeof(cl_mem), (void*)&__raidance);
	addKernelArg(4, 10, sizeof(cl_mem), (void*)&__faceIndexHit);
	//randseed
	addKernelArg(4, 12, sizeof(cl_mem), (void*)&__sampleWeight);
	addKernelArg(4, 13, sizeof(cl_mem), (void*)&__colorTexture);
	addKernelArg(4, 14, sizeof(cl_mem), (void*)&__bumpMapTexture);
}

void RayTracerKernel::readRadiance()
{
	cl_int ret;
	ret = clEnqueueReadBuffer(__context->commandQueue, __raidance, CL_TRUE, 0, __radianceBufferSize, (void*)radiance, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::readFaceHitIndex()
{
	cl_int ret;
	ret = clEnqueueReadBuffer(__context->commandQueue, __faceIndexHit, CL_TRUE, 0, __faceIndexBufferSize, (void*)__faceIndexHitArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::readPosition()
{
	cl_int ret;
	ret = clEnqueueReadBuffer(__context->commandQueue, __position, CL_TRUE, 0, __positionBufferSize, (void*)__positionArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::readDirection()
{
	cl_int ret;
	ret = clEnqueueReadBuffer(__context->commandQueue, __direction, CL_TRUE, 0, __positionBufferSize, (void*)__directionArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::readSampleWeight()
{
	cl_int ret;
	ret = clEnqueueReadBuffer(__context->commandQueue, __sampleWeight, CL_TRUE, 0, __sampleWeightBufferSize, (void*)__sampleWeightArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void RayTracerKernel::clearSampleWeightBuffer()
{
	for (int i = 0; i < __sampleWeightBufferSize / sizeof(float); i++)
	{
		__sampleWeightArray[i] = 0.0;
	}
	cl_int ret;
	ret = clEnqueueWriteBuffer(__context->commandQueue, __sampleWeight, CL_TRUE, 0, __sampleWeightBufferSize, __sampleWeightArray, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}


IrradianceCache::IrradianceCache()
{
	this->irradiance[0] = 0.0;
	this->irradiance[1] = 0.0;
	this->irradiance[2] = 0.0;

	this->rotationGrad[0] = 0.0;
	this->rotationGrad[1] = 0.0;
	this->rotationGrad[2] = 0.0;

	this->translationGrad[0] = 0.0;
	this->translationGrad[1] = 0.0;
	this->translationGrad[2] = 0.0;
}