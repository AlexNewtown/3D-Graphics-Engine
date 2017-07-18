#define MIN_FLOAT -8388608
#define MAX_FLOAT 213909504
#define PARALLEL_THRESHOLD 0.00000000001
#define MAX_QUEUE_SIZE 1000
#define MAX_BOUNCES 1
#define THETA_RANGE 2*M_PI
#define PHI_RANGE 0.5*M_PI

struct Face_float
{
	float v1[3];
	float v2[3];
	float v3[3];
	
	float n1[3];
	float n2[3];
	float n3[3];

	float tc1[2];
	float tc2[2];
	float tc3[2];
};

struct Face_int
{
	int index;
	int materialIndex;
	int brdfIndex;
	int colorTexIndex;
	int bumpTexIndex;
};

struct Material
{
	float ka[3];
	float kd[3];
	float ks[3];
	float tf[3];

	float ni;
	float ns;
	float d;
	float sharpness;
};

struct KdTreeTextureFloat
{
	float maxX;
	float maxY;
	float maxZ;
	float maxW;

	float minX;
	float minY;
	float minZ;
	float minW;
};

struct KdTreeTextureInt
{
	int treeIndex;
	int child0;
	int child1;
	int sortOn;
	int numFaces;
	int faceIndex;

	int totalNumFaces;
	int padding2;
};

struct PointLight
{
	float position[3];
	float intensity;
};

struct Radiance
{
	float color[3];
};

struct RayHitInfo
{
	float rayDir[3];
	float rayPos[3];
	
	float minX;
	float minY;
	float minZ;

	float maxX;
	float maxY;
	float maxZ;
	
	float tmin;
	float tmax;
	
	float txmin;
	float txmax;
	float tymin;
	float tymax;
	float tzmin;
	float tzmax;
};

/* FUNCTIONS */
void rayCastBVH(float* position, float* direction, struct RayHitInfo* rhi, int* faceIndex, __global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Face_float* faceFloat, __global struct Face_int* faceInt);
void directRadiance(float* position,  float* surfaceNormal, float* outDirection, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* faceInt, 
	__global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, float* result);
void sampleRadiance(float* origin, float* hitPosition, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* faceInt,  
	__global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, float* outgoingRadiance);


float random(int* s);
void swap(float* a, float* b);
void crossProduct(float* x, float* y, float* result);
float dotProduct(float* x, float* y);
void normalizeVector(float* x);
float vectorLength(float* x);
void computeMinMax(struct RayHitInfo* rhi, float* rayDir, float* rayPos);
void copyHitInfo(struct RayHitInfo* original, struct RayHitInfo* copy);
bool rayBoxIntersect(float* rayDir, float* rayPos, float* boundsMin, float* boundsMax, struct RayHitInfo* hitInfo);
bool rayTriangleIntersect(float* rayDir, float* rayPos, float* v1, float* v2, float* v3, struct RayHitInfo* rhi);
bool pointInBox(float* pos, float* boundsMin, float* boundsMax);
void enqueueBVH(int val, int* queue, int* queueIndex);
int getNextInQueueBVH(int* queue, int* queueIndex);
bool queueEmptyBVH(int queueIndex);
void interpolateNormals(float* hitPos, struct Face_float* face, float* normalResult);
void interpolateNormalsBumpMap(float* hitPos, struct Face_float* faceFloat, struct Face_int* faceInt, __read_only image3d_t bumpTex, float* result);
void interpolateTextureCoords(float* hitPos, struct Face_float* face, float* texResult);
float triangleArea(float* v1, float* v2, float* v3);
void getFaceFloat(__global struct Face_float* facesFloat, int faceIndex, struct Face_float* face);
void getFaceInt(__global struct Face_int* facesInt, int faceIndex, struct Face_int* face);
float randomizeDirection(struct Face_float* faceFloat, float* position, float* normalInterp, float* direction, int randSeed );
void reflection(float* dirIn, float* dirOut, float* normal, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct Material* material, float* result);
void reflectionDiffuse(int faceIndex, __global struct Face_int* facesInt, __global struct Material* material, float* result);

/* KERNELS */

__kernel void rayCastUpdate(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global int* faceIndexHit);

__kernel void computeDirectRadiance(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit,
	__read_only image3d_t colorTex, __read_only image3d_t bumpTex);

__kernel void stochasticRayTraceRadiance(__global float* position, __global float* directionOut, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit, int randSeed,
	__global float* sampleWeight, __read_only image3d_t colorTex, __read_only image3d_t bumpTex
	);

__kernel void computeDirectRadianceCache(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit,
	__read_only image3d_t colorTex, __read_only image3d_t bumpTex);

__kernel void stochasticRayTraceRadianceCache(__global float* position, __global float* directionOut, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit, int randSeed,
	__global float* sampleWeight, __read_only image3d_t colorTex, __read_only image3d_t bumpTex
	);

#define RAND_MAX 2147483647.0

float random(int* s)
{
	/*
	float a = 482710.0;
	float c = 0.0;
	float r = fmod((float)(a*(*s) + c), RAND_MAX);
	*s = (int)r;
	return r / RAND_MAX;
	*/

	float a = (int)s[0]*12.9898 + (int)s[1]*78.233;
	float b = sin(a);
	float fret;
	float r = fract(b*43758.5453,&fret);
	s[0] = (int)(b*43758.5453);
	return r;
}

float randomizeDirection(struct Face_float* faceFloat, float* position, float* normalInterp, float* direction, int randSeed )
{

	float bx[3];
	bx[0] = faceFloat->v2[0] - faceFloat->v1[0];
	bx[1] = faceFloat->v2[1] - faceFloat->v1[1];
	bx[2] = faceFloat->v2[2] - faceFloat->v1[2];
	normalizeVector(bx);

	float bz[3];
	bz[0] = faceFloat->v3[0] - faceFloat->v2[0];
	bz[1] = faceFloat->v3[1] - faceFloat->v2[1];
	bz[2] = faceFloat->v3[2] - faceFloat->v2[2];
	normalizeVector(bz);

	float n[3];
	crossProduct(bx,bz,n);
	normalizeVector(n);

	crossProduct(bx,n,bz);
	normalizeVector(bz);

	int s[] = {randSeed,get_global_id(0)};
	float thetaRand = THETA_RANGE*(random(s));
	
	//float phiRand = PHI_RANGE*random(s);
	float phiRand = asin(random(s));

	float dirX = cos(thetaRand)*sin(phiRand);
	float dirZ = sin(thetaRand)*sin(phiRand);
	float dirY = cos(phiRand);

	direction[0] = bx[0] * dirX + bz[0] * dirZ + n[0] * dirY;
	direction[1] = bx[1] * dirX + bz[1] * dirZ + n[1] * dirY;
	direction[2] = bx[2] * dirX + bz[2] * dirZ + n[2] * dirY;
	normalizeVector(direction);

	float p = M_PI*M_PI;
	p = 1.0 / p;
	return p;
}

float torranceSparrowD(float* hVector, float* normal, float m)
{
	float normalHalfwaveDot = dotProduct(hVector,normal);
	float tsdTerm1 = 1.0/(M_PI*m*m*pow(normalHalfwaveDot,4.0));
	float tsdTerm2 = (pow(normalHalfwaveDot,2.0) - 1.0)/(m*m*(pow(normalHalfwaveDot,2.0)));
	return tsdTerm1*exp(tsdTerm2);
}

float fresnelFactor(float* dirIn, float* normal, float indexOfRefraction)
{
	float f_not = pow(fabs((1.0 - indexOfRefraction)/(1.0 + indexOfRefraction)),2.0);
	float base_exponent = 1.0 - dotProduct(dirIn, normal);
	base_exponent = fabs(base_exponent);
	float f = f_not + (1.0 - f_not)*pow(base_exponent,5.0);
	return f;
}

float torranceSparrowG(float* dirIn, float* dirOut, float* normal, float* hVector)
{
	float n_dot_h =  dotProduct(normal,hVector);
	float n_dot_wo = dotProduct(normal, dirOut);
	float wo_dot_h = dotProduct(dirOut,hVector);
	float n_dot_wi = dotProduct(normal, dirIn );

	float min_val = fmin( 1.0, (2.0*n_dot_h*n_dot_wo)/wo_dot_h );
	return fmin(min_val, (2.0*n_dot_h*n_dot_wi)/wo_dot_h );
}


void reflection(float* dirIn, float* dirOut, float* normal, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct Material* material, float* result)
{
	float m = 2.5;
	float indexOfRefraction = 1.44;

	float hVector[3];
	hVector[0] = dirIn[0] + dirOut[0];
	hVector[1] = dirIn[1] + dirOut[1];
	hVector[2] = dirIn[2] + dirOut[2];
	normalizeVector(hVector);

	float kd[3];
	struct Face_int faceI;
	getFaceInt(facesInt, faceIndex, &faceI);
	kd[0] = material[faceI.materialIndex].kd[0];
	kd[1] = material[faceI.materialIndex].kd[1];
	kd[2] = material[faceI.materialIndex].kd[2];

	float ks[3];
	ks[0] = material[faceI.materialIndex].ks[0];
	ks[1] = material[faceI.materialIndex].ks[1];
	ks[2] = material[faceI.materialIndex].ks[2];

	result[0] = kd[0]/M_PI;
	result[1] = kd[1]/M_PI;
	result[2] = kd[2]/M_PI;
	
	float dp = dotProduct(normal,dirIn);
	float tsd = torranceSparrowD(hVector, normal, m);
	float fres = fresnelFactor(dirIn, normal, indexOfRefraction);
	float tsg = torranceSparrowG(dirIn, dirOut, normal, hVector);
	float specularFactor = (1.0/(4.0*M_PI*dp))*tsd*fres*tsg;

	result[0] = result[0] + ks[0]*specularFactor;
	result[1] = result[1] + ks[1]*specularFactor;
	result[2] = result[2] + ks[2]*specularFactor;
}

void reflectionDiffuse(int faceIndex, __global struct Face_int* facesInt, __global struct Material* material, float* result)
{
	float kd[3];
	struct Face_int faceI;
	getFaceInt(facesInt, faceIndex, &faceI);
	kd[0] = material[faceI.materialIndex].kd[0];
	kd[1] = material[faceI.materialIndex].kd[1];
	kd[2] = material[faceI.materialIndex].kd[2];

	result[0] = kd[0]/M_PI;
	result[1] = kd[1]/M_PI;
	result[2] = kd[2]/M_PI;
}

void getFaceInt(__global struct Face_int* facesInt, int faceIndex, struct Face_int* face)
{
	face->index = facesInt[faceIndex].index;
	face->materialIndex = facesInt[faceIndex].materialIndex;
	face->brdfIndex = facesInt[faceIndex].brdfIndex;
	face->colorTexIndex = facesInt[faceIndex].colorTexIndex;
	face->bumpTexIndex = facesInt[faceIndex].bumpTexIndex;
}

void getFaceFloat(__global struct Face_float* facesFloat, int faceIndex, struct Face_float* face)
{
	face->v1[0] = facesFloat[faceIndex].v1[0];
	face->v1[1] = facesFloat[faceIndex].v1[1];
	face->v1[2] = facesFloat[faceIndex].v1[2];

	face->v2[0] = facesFloat[faceIndex].v2[0];
	face->v2[1] = facesFloat[faceIndex].v2[1];
	face->v2[2] = facesFloat[faceIndex].v2[2];

	face->v3[0] = facesFloat[faceIndex].v3[0];
	face->v3[1] = facesFloat[faceIndex].v3[1];
	face->v3[2] = facesFloat[faceIndex].v3[2];


	face->n1[0] = facesFloat[faceIndex].n1[0];
	face->n1[1] = facesFloat[faceIndex].n1[1];
	face->n1[2] = facesFloat[faceIndex].n1[2];

	face->n2[0] = facesFloat[faceIndex].n2[0];
	face->n2[1] = facesFloat[faceIndex].n2[1];
	face->n2[2] = facesFloat[faceIndex].n2[2];

	face->n3[0] = facesFloat[faceIndex].n3[0];
	face->n3[1] = facesFloat[faceIndex].n3[1];
	face->n3[2] = facesFloat[faceIndex].n3[2];


	face->tc1[0] = facesFloat[faceIndex].tc1[0];
	face->tc1[1] = facesFloat[faceIndex].tc1[1];

	face->tc2[0] = facesFloat[faceIndex].tc2[0];
	face->tc2[1] = facesFloat[faceIndex].tc2[1];

	face->tc3[0] = facesFloat[faceIndex].tc3[0];
	face->tc3[1] = facesFloat[faceIndex].tc3[1];
}

float triangleArea(float* v1, float* v2, float* v3)
{
	float A[] = { v1[0] - v2[0], v1[1] - v2[1] , v1[2] - v2[2] };
	float B[] = { v1[0] - v3[0], v1[1] - v3[1] , v1[2] - v3[2] };
	float C[] = { v2[0] - v3[0], v2[1] - v3[1] , v2[2] - v3[2] };
	float a = dotProduct(A, A);
	a = max(a, 0.0);

	float b = dotProduct(B, B);
	b = max(b, 0.0);

	float c = dotProduct(C, C);
	c = max(c, 0.0);

	a = sqrt(a);
	b = sqrt(b);
	c = sqrt(c);

	float s = (a + b + c) / 2.0;

	float ar = s*(s - a)*(s - b)*(s - c);
	ar = max(ar, 0.0);
	ar = sqrt(ar);
	return ar;
}

void interpolateNormalsBumpMap(float* hitPos, struct Face_float* faceFloat, struct Face_int* faceInt, __read_only image3d_t bumpTex, float* result)
{
	/*
	float texCoord[2];
	// get the texture coordinates
	interpolateTextureCoords(hitPos, faceFloat, texCoord);


	const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;
	float delta = 0.01;

	//sample the bump map image at +- deltaX and +- deltaX
	float4 texSampleDXP = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	texSampleDXP = read_imagef(bumpTex, sampler, (float4)(texCoord[0] + delta, 1.0 - texCoord[1], 0.0, 0.0));

	float4 texSampleDXM = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	texSampleDXM = read_imagef(bumpTex, sampler, (float4)(texCoord[0] - delta, 1.0 - texCoord[1], 0.0, 0.0));

	float4 texSampleDYP = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	texSampleDYP = read_imagef(bumpTex, sampler, (float4)(texCoord[0], 1.0 - texCoord[1] - delta, 0.0, 0.0));

	float4 texSampleDYM = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	texSampleDYM = read_imagef(bumpTex, sampler, (float4)(texCoord[0], 1.0 - texCoord[1] + delta, 0.0, 0.0));
	*/

}

void interpolateNormals(float* hitPos, struct Face_float* face, float* normalResult)
{
	float areaV1 = triangleArea(face->v2, face->v3, hitPos);
	float areaV2 = triangleArea(face->v1, face->v3, hitPos);
	float areaV3 = triangleArea(face->v1, face->v2, hitPos);
	float totalArea = areaV1 + areaV2 + areaV3;
	areaV1 = areaV1 / totalArea;
	areaV2 = areaV2 / totalArea;
	areaV3 = areaV3 / totalArea;

	normalResult[0] = areaV1 * face->n1[0] + areaV2 * face->n2[0] + areaV3 * face->n3[0];
	normalResult[1] = areaV1 * face->n1[1] + areaV2 * face->n2[1] + areaV3 * face->n3[1];
	normalResult[2] = areaV1 * face->n1[2] + areaV2 * face->n2[2] + areaV3 * face->n3[2];
	normalizeVector(normalResult);
}

void interpolateTextureCoords(float* hitPos, struct Face_float* face, float* texResult)
{
	float areaV1 = triangleArea(face->v2, face->v3, hitPos);
	float areaV2 = triangleArea(face->v1, face->v3, hitPos);
	float areaV3 = triangleArea(face->v1, face->v2, hitPos);
	float totalArea = areaV1 + areaV2 + areaV3;
	areaV1 = areaV1 / totalArea;
	areaV2 = areaV2 / totalArea;
	areaV3 = areaV3 / totalArea;

	texResult[0] = areaV1 * face->tc1[0] + areaV2 * face->tc2[0] + areaV3 * face->tc3[0];
	texResult[1] = areaV1 * face->tc1[1] + areaV2 * face->tc2[1] + areaV3 * face->tc3[1];
}

void swap(float* a, float* b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}


void crossProduct(float* x, float* y, float* result)
{
	float r1 = x[1] * y[2];
	float r2 = x[2] * y[1];
	result[0] = x[1]*y[2] - x[2]*y[1];//aybz - azby
	result[1] = x[2]*y[0] - x[0]*y[2];//azbx - axbz
	result[2] = x[0]*y[1] - x[1]*y[0];//axby - aybx
}

float dotProduct(float* x, float* y)
{
	float result = x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
	return result;
}

void normalizeVector(float* x)
{
	float l = fabs(x[0]*x[0]) + fabs(x[1]*x[1]) + fabs(x[2]*x[2]);
	if( l == 0.0)
		return;

	l = sqrt(l);
	x[0] = x[0]/l;
	x[1] = x[1]/l;
	x[2] = x[2]/l;
}

float vectorLength(float* x)
{
	float l = fabs(x[0]*x[0]) + fabs(x[1]*x[1]) + fabs(x[2]*x[2]);
	return sqrt(l);
}

void computeMinMax(struct RayHitInfo* rhi, float* rayDir, float* rayPos)
{
	rhi->rayDir[0] = rayDir[0];
	rhi->rayDir[1] = rayDir[1];
	rhi->rayDir[2] = rayDir[2];

	rhi->rayPos[0] = rayPos[0];
	rhi->rayPos[1] = rayPos[1];
	rhi->rayPos[2] = rayPos[2];

	rhi->minX = rayPos[0] + rhi->tmin*rayDir[0];
	rhi->minY = rayPos[1] + rhi->tmin*rayDir[1];
	rhi->minZ = rayPos[2] + rhi->tmin*rayDir[2];

	rhi->maxX = rayPos[0] + rhi->tmax*rayDir[0];
	rhi->maxY = rayPos[1] + rhi->tmax*rayDir[1];
	rhi->maxZ = rayPos[2] + rhi->tmax*rayDir[2];
}

void copyHitInfo(struct RayHitInfo* original, struct RayHitInfo* copy)
{
	copy->rayDir[0] = original->rayDir[0];
	copy->rayDir[1] = original->rayDir[1];
	copy->rayDir[2] = original->rayDir[2];

	copy->rayPos[0] = original->rayPos[0];
	copy->rayPos[1] = original->rayPos[1];
	copy->rayPos[2] = original->rayPos[2];

	copy->minX = original->minX;
	copy->minY = original->minY;
	copy->minZ = original->minZ;

	copy->maxX = original->maxX;
	copy->maxY = original->maxY;
	copy->maxZ = original->maxZ;

	copy->tmin = original->tmin;
	copy->tmax = original->tmax;

	copy->txmin = original->txmin;
	copy->txmax = original->txmax;
	copy->tymin = original->tymin;
	copy->tymax = original->tymax;
	copy->tzmin = original->tzmin;
	copy->tzmax = original->tzmax;
}

bool rayBoxIntersect(float* rayDir, float* rayPos, float* boundsMin, float* boundsMax, struct RayHitInfo* hitInfo)
{
	float tmin = 0.0f;
	float tmax = MAX_FLOAT;
	float txmin = 0, tymin = 0, tzmin = 0;
	float txmax = MAX_FLOAT;
	float tymax = MAX_FLOAT;
	float tzmax = MAX_FLOAT;

	if (fabs(rayDir[0]) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (rayPos[0] < boundsMin[0] || rayPos[0] > boundsMax[0])
			return false;
	}
	if (fabs(rayDir[1]) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (rayPos[1] < boundsMin[1] || rayPos[1] > boundsMax[1])
			return false;
	}
	if (fabs(rayDir[2]) < PARALLEL_THRESHOLD)
	{
		// ray is parallel to slab.
		if (rayPos[2] < boundsMin[2] || rayPos[2] > boundsMax[2])
			return false;
	}

	txmin = (boundsMin[0] - rayPos[0]) / rayDir[0];
	txmax = (boundsMax[0] - rayPos[0]) / rayDir[0];
	if (txmin > txmax)
		swap(&txmin, &txmax);

	tymin = (boundsMin[1] - rayPos[1]) / rayDir[1];
	tymax = (boundsMax[1] - rayPos[1]) / rayDir[1];
	if (tymin > tymax)
		swap(&tymin, &tymax);


	tzmin = (boundsMin[2] - rayPos[2]) / rayDir[2];
	tzmax = (boundsMax[2] - rayPos[2]) / rayDir[2];
	if (tzmin > tzmax)
		swap(&tzmin, &tzmax);

	tmax = min(txmax, tymax);
	tmax = min(tzmax, tmax);
	tmin = max(txmin, tymin);
	tmin = max(tzmin, tmin);

	if (tmin > tmax)
		return false;

	hitInfo->tmax = tmax;
	hitInfo->tmin = tmin;
	hitInfo->txmax = txmax;
	hitInfo->txmin = txmin;
	hitInfo->tymax = tymax;
	hitInfo->tymin = tymin;
	hitInfo->tzmax = tzmax;
	hitInfo->tzmin = tzmin;
	computeMinMax(hitInfo,rayDir, rayPos);
	return true;
}

bool rayTriangleIntersect(float* rayDir, float* rayPos, float* v1, float* v2, float* v3, struct RayHitInfo* rhi)
{
	float E1[] = { v2[0] - v1[0],
					v2[1] - v1[1],
					v2[2] - v1[2] };

	float E2[] = { v3[0] - v1[0],
					v3[1] - v1[1],
					v3[2] - v1[2] };

	float T[] = { rayPos[0] - v1[0],
		rayPos[1] - v1[1],
		rayPos[2] - v1[2] };

	float Q[3];
	crossProduct(T, E1, Q);

	float P[3];
	crossProduct(rayDir, E2,P);

	float det = P[0] * E1[0] + P[1] * E1[1] + P[2] * E1[2];

	float detInv = 1.0f / det;

	float u = (P[0] * T[0] + P[1] * T[1] + P[2] * T[2])*detInv;

	if (u < 0.0f || u > 1.0f)
		return false;

	float v = (Q[0] * rayDir[0] + Q[1] * rayDir[1] + Q[2] * rayDir[2])*detInv;
	
	if (v < 0.0f || u > 1.0f)
		return false;

	if (u + v > 1.0f)
		return false;
	float t = (Q[0] * E2[0] + Q[1] * E2[1] + Q[2] * E2[2])*detInv;

	rhi->rayDir[0] = rayDir[0];
	rhi->rayDir[1] = rayDir[1];
	rhi->rayDir[2] = rayDir[2];

	rhi->rayPos[0] = rayPos[0];
	rhi->rayPos[1] = rayPos[1];
	rhi->rayPos[2] = rayPos[2];

	float tmin = dotProduct(E2, Q)*detInv;
	float hit[] = { tmin*rayDir[0] + rayPos[0], tmin*rayDir[1] + rayPos[1], tmin*rayDir[2] + rayPos[2]};

	float txmin = hit[0] - rayPos[0];
	float tymin = hit[1] - rayPos[1];
	float tzmin = hit[2] - rayPos[2];

	float tVec[] = { txmin,tymin,tzmin };
	rhi->txmin = txmin;
	rhi->tymin = tymin;
	rhi->tzmin = tzmin;

	rhi->minX = hit[0];
	rhi->minY = hit[1];
	rhi->minZ = hit[2];
	rhi->tmin = tmin;

	return true;
}

bool pointInBox(float* pos, float* BoundsMin, float* boundsMax)
{
	if( pos[0] < BoundsMin[0] || pos[0] > boundsMax[0])
		return false;
	if( pos[1] < BoundsMin[1] || pos[1] > boundsMax[1])
		return false;
	if( pos[2] < BoundsMin[2] || pos[2] > boundsMax[2])
		return false;

	return true;
}

void enqueueBVH(int val, int* queue, int* queueIndex)
{
	if(*queueIndex < MAX_QUEUE_SIZE - 1)
	{
		queue[*queueIndex] = val;
		(*queueIndex)++;
	}
}
int getNextInQueueBVH(int* queue, int* queueIndex)
{
	if((*queueIndex) < 0)
		return -1;
	int returnVal = queue[0];
	for(int i=0; i<(*queueIndex)-1; i++)
	{
		queue[i] = queue[i+1];
	}

	(*queueIndex)--;
	return returnVal;
}
bool queueEmptyBVH(int queueIndex)
{
	if(queueIndex < 0)
		return true;
	else
		return false;
}

void sampleRadiance(float* origin, float* hitPosition, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* faceInt, 
	__global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, float* outgoingRadiance)
{
	struct Face_float faceF;
	float normal[3];
	getFaceFloat(facesFloat, faceIndex, &faceF);
	interpolateNormals(hitPosition, &faceF, normal);
	float dir[3];
	dir[0] = -hitPosition[0] + origin[0];
	dir[1] = -hitPosition[1] + origin[1];
	dir[2] = -hitPosition[2] + origin[2];
	float dist = vectorLength(dir);
	normalizeVector(dir);

	float hp[3];
	hp[0] = hitPosition[0] + normal[0]*0.01;
	hp[1] = hitPosition[1] + normal[1]*0.01;
	hp[2] = hitPosition[2] + normal[2]*0.01;
	float dRad[3];
	directRadiance(hp, normal, dir, faceIndex, facesFloat, faceInt, kdTextureFloat, kdTextureInt, material, pointLights, numLights, dRad);

	float transportFactor = 1.0 / pow(dist/100.0, 2.0);
	transportFactor = min(transportFactor, 1.0);

	outgoingRadiance[0] = transportFactor * dRad[0];
	outgoingRadiance[1] = transportFactor * dRad[1];
	outgoingRadiance[2] = transportFactor * dRad[2];
}

void directRadiance(float* position,  float* surfaceNormal, float* outDirection, int faceIndex, __global struct Face_float* facesFloat, __global struct Face_int* faceInt, 
	__global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, float* result)
{
	if(numLights <= 0)
	{
		result[0] = 0.0;
		result[1] = 0.0;
		result[2] = 0.0;
		return;
	}

	float radianceSum = 0.0;
	struct RayHitInfo rhi;
	int hitFaceIndex = -1;
	result[0] = 0.0;
	result[1] = 0.0;
	result[2] = 0.0;

	for(int i=0; i < numLights; i++)
	{
		float lightPos[3];
		lightPos[0] = pointLights[i].position[0];
		lightPos[1] = pointLights[i].position[1];
		lightPos[2] = pointLights[i].position[2];

		float lightDir[3];
		lightDir[0] = lightPos[0] - position[0];
		lightDir[1] = lightPos[1] - position[1];
		lightDir[2] = lightPos[2] - position[2];
		float lightDistance = vectorLength(lightDir);
		normalizeVector(lightDir);
		float lightIntensity = pointLights[i].intensity;
		rhi.tmin = 9999999999.9;

		rayCastBVH(position, lightDir, &rhi, &hitFaceIndex, kdTextureFloat, kdTextureInt, facesFloat, faceInt);
		float faceHitVec[] = {rhi.minX - position[0], rhi.minY - position[1], rhi.minZ - position[2]};
		float hitLength = vectorLength(faceHitVec);

		if(hitFaceIndex == -1 || ( (hitFaceIndex != -1 ) && (lightDistance < hitLength) ) )
		{
			float dp = dotProduct(lightDir,surfaceNormal);
			dp = max(dp,0.0);

			float dpOut = dotProduct(outDirection,surfaceNormal);
			dpOut = max(dpOut,0.0);

			float transportFactor = 1.0 / pow(lightDistance/100.0, 2.0);
			transportFactor = min(transportFactor, 1.0);
			float refl[] = {1.0, 1.0, 1.0};
			reflection(lightDir, outDirection, surfaceNormal, faceIndex, facesFloat, faceInt, material, refl);

			result[0] += transportFactor*lightIntensity*dp*dpOut*refl[0];
			result[1] += transportFactor*lightIntensity*dp*dpOut*refl[1];
			result[2] += transportFactor*lightIntensity*dp*dpOut*refl[2];
		}
	}
}

void rayCastBVH(float* position, float* direction, struct RayHitInfo* rhi, int* faceIndex, __global struct KdTreeTextureFloat* kdTextureFloat, __global struct KdTreeTextureInt* kdTextureInt, __global struct Face_float* faceFloat, __global struct Face_int* faceInt)
{
	struct RayHitInfo hitTemp;
	float minDistance = 999999.0;
	bool faceHit = false;
	*faceIndex = -1;
	
	/*
	int totalNumFaces = kdTextureInt[0].totalNumFaces;
	totalNumFaces = min(totalNumFaces,1000);

	for(int i=0; i < totalNumFaces; i++)
	{
		float v1[3];
		float v2[3];
		float v3[3];

		v1[0] = faceFloat[i].v1[0];
		v1[1] = faceFloat[i].v1[1];
		v1[2] = faceFloat[i].v1[2];

		v2[0] = faceFloat[i].v2[0];
		v2[1] = faceFloat[i].v2[1];
		v2[2] = faceFloat[i].v2[2];

		v3[0] = faceFloat[i].v3[0];
		v3[1] = faceFloat[i].v3[1];
		v3[2] = faceFloat[i].v3[2];

		if(rayTriangleIntersect(direction, position, v1, v2, v3, &hitTemp))
		{
			if(hitTemp.tmin > 0.0 && hitTemp.tmin < minDistance)
			{
				copyHitInfo(&hitTemp, rhi);
				faceHit = true;
				*faceIndex = i;
				minDistance = hitTemp.tmin;
			}
		}
	}
	return;
	*/


	int rayQueue[MAX_QUEUE_SIZE];
	int rayQueueIndex = 0;
	enqueueBVH(0 ,rayQueue, &rayQueueIndex);
	int maxRecursion = 0;

	while(!queueEmptyBVH(rayQueueIndex))
	{
		int q = getNextInQueueBVH(rayQueue, &rayQueueIndex);
		float boundsMin[3];
		boundsMin[0] = kdTextureFloat[q].minX;
		boundsMin[1] = kdTextureFloat[q].minY;
		boundsMin[2] = kdTextureFloat[q].minZ;

		float boundsMax[3];
		boundsMax[0] = kdTextureFloat[q].maxX;
		boundsMax[1] = kdTextureFloat[q].maxY;
		boundsMax[2] = kdTextureFloat[q].maxZ;

		if(rayBoxIntersect(direction, position, boundsMin, boundsMax, &hitTemp) || pointInBox(position, boundsMin, boundsMax))
		{
			int bvhChild0 = kdTextureInt[q].child0;
			int bvhChild1 = kdTextureInt[q].child1;
			if(bvhChild0 == -1 && bvhChild1 == -1)
			{
				int fi = kdTextureInt[q].faceIndex;

				float v1[3];
				float v2[3];
				float v3[3];

				v1[0] = faceFloat[fi].v1[0];
				v1[1] = faceFloat[fi].v1[1];
				v1[2] = faceFloat[fi].v1[2];

				v2[0] = faceFloat[fi].v2[0];
				v2[1] = faceFloat[fi].v2[1];
				v2[2] = faceFloat[fi].v2[2];

				v3[0] = faceFloat[fi].v3[0];
				v3[1] = faceFloat[fi].v3[1];
				v3[2] = faceFloat[fi].v3[2];

				if(rayTriangleIntersect(direction, position, v1, v2, v3, &hitTemp))
				{
					if(hitTemp.tmin > 0.0 && hitTemp.tmin < minDistance)
					{
						copyHitInfo(&hitTemp, rhi);
						faceHit = true;
						*faceIndex = fi;
						minDistance = hitTemp.tmin;
					}
				}
			}
			else
			{

				boundsMin[0] = kdTextureFloat[bvhChild0].minX;
				boundsMin[1] = kdTextureFloat[bvhChild0].minY;
				boundsMin[2] = kdTextureFloat[bvhChild0].minZ;

				boundsMax[0] = kdTextureFloat[bvhChild0].maxX;
				boundsMax[1] = kdTextureFloat[bvhChild0].maxY;
				boundsMax[2] = kdTextureFloat[bvhChild0].maxZ;
				if(rayBoxIntersect(direction, position, boundsMin, boundsMax, &hitTemp) || pointInBox(position, boundsMin, boundsMax))
				{
					enqueueBVH(bvhChild0,rayQueue, &rayQueueIndex);
				}


				boundsMin[0] = kdTextureFloat[bvhChild1].minX;
				boundsMin[1] = kdTextureFloat[bvhChild1].minY;
				boundsMin[2] = kdTextureFloat[bvhChild1].minZ;

				boundsMax[0] = kdTextureFloat[bvhChild1].maxX;
				boundsMax[1] = kdTextureFloat[bvhChild1].maxY;
				boundsMax[2] = kdTextureFloat[bvhChild1].maxZ;
				if(rayBoxIntersect(direction, position, boundsMin, boundsMax, &hitTemp) || pointInBox(position, boundsMin, boundsMax))
				{
					enqueueBVH(bvhChild1,rayQueue, &rayQueueIndex);
				}
			}

		}
		maxRecursion++;

		if(maxRecursion > MAX_QUEUE_SIZE)
			break;
	}
}

__kernel void rayCastUpdate(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global int* faceIndexHit)
{
	int index = get_global_id(0);
	float pos[3];
	float dir[3];
	
	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];

	dir[0] = direction[3*index];
	dir[1] = direction[3*index + 1];
	dir[2] = direction[3*index + 2];

	int faceIndex = -1;
	struct RayHitInfo rhi;
	rayCastBVH(pos, dir, &rhi, &faceIndex, kdTextureFloat, kdTextureInt, facesFloat, facesInt);
	faceIndexHit[index] = faceIndex;
	if(faceIndex != -1)
	{
		position[3*index] = rhi.minX;
		position[3*index + 1] = rhi.minY;
		position[3*index + 2] = rhi.minZ;
	}
}

__kernel void computeDirectRadiance(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit,
	__read_only image3d_t colorTex, __read_only image3d_t bumpTex)
{
	int index = get_global_id(0);

	if(faceIndexHit[index] == -1 )
	{
		outputRadiance[index].color[0] = 0.0;
		outputRadiance[index].color[1] = 0.0;
		outputRadiance[index].color[2] = 0.0;
		return;
	}

	float pos[3];
	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];

	float dir[3];
	dir[0] = -direction[3*index];
	dir[1] = -direction[3*index + 1];
	dir[2] = -direction[3*index + 2];

	struct Face_float f;
	int faceIndex = faceIndexHit[index];
	getFaceFloat(facesFloat, faceIndex, &f);
	float normal[3];
	float texCoord[2];
	interpolateNormals(pos, &f, normal);
	interpolateTextureCoords(pos, &f, texCoord);

	pos[0] = pos[0] + normal[0]*0.01;
	pos[1] = pos[1] + normal[1]*0.01;
	pos[2] = pos[2] + normal[2]*0.01;

	float rad[3];
	float origin[] = {0.0,0.0,0.0};
	directRadiance(pos, normal, dir, faceIndex, facesFloat, facesInt,  kdTextureFloat, kdTextureInt, material, pointLights, numLights, rad);

	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];
	float dist = vectorLength(pos);
	float transportFactor = 1.0 / pow(dist/100.0, 2.0);
	transportFactor = min(transportFactor, 1.0);

	int cTexIndex = facesInt[faceIndex].colorTexIndex;
	const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;
	
	float4 texSample = (float4)(1.0f, 1.0f, 1.0f, 1.0f);
	if(cTexIndex != -1)
	{
		texSample = read_imagef(colorTex, sampler, (float4)(texCoord[0], 1.0 - texCoord[1], (float)cTexIndex, 0.0));
	}
	outputRadiance[index].color[0] = rad[0] * transportFactor * texSample.x;
	outputRadiance[index].color[1] = rad[1] * transportFactor * texSample.y;
	outputRadiance[index].color[2] = rad[2] * transportFactor * texSample.z;
}

__kernel void stochasticRayTraceRadiance(__global float* position, __global float* directionOut, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit, int randSeed,
	__global float* sampleWeight, __read_only image3d_t colorTex, __read_only image3d_t bumpTex
	)
{
	int index = get_global_id(0);
	int faceIndex = faceIndexHit[index];

	// if no shading is required, return.
	if(faceIndex == -1 )
	{
		sampleWeight[index] = sampleWeight[index] + 1.0;
		outputRadiance[index].color[0] = 0.0;
		outputRadiance[index].color[1] = 0.0;
		outputRadiance[index].color[2] = 0.0;
		return;
	}

	//init the starting position, and outgoing direction
	float pos[3];
	float dir[3];
	float dirOut[3];
	dirOut[0] = -directionOut[3*index];
	dirOut[1] = -directionOut[3*index + 1];
	dirOut[2] = -directionOut[3*index + 2];
	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];

	// compute the surface normal
	struct Face_float faceF;
	float normal[3];
	getFaceFloat(facesFloat, faceIndex, &faceF);
	interpolateNormals(pos, &faceF, normal);

	// randomize the first direction
	float p = randomizeDirection(&faceF, pos, normal, dir, randSeed);

	float incidentRadiance;
	float transport[3];
	int hitFaceIndex = -1;
	struct RayHitInfo rhi;
	float transportFactors[] = {1.0, 1.0, 1.0};
	
	//compute the light transport from the surface point to the eye.
	float transportToEye;
	float eyeDist = vectorLength(pos);
	transportToEye = 1.0 / pow(eyeDist/100.0, 2.0);
	transportToEye = min(transportToEye, 1.0);

	transportFactors[0] = transportFactors[0]*transportToEye;
	transportFactors[1] = transportFactors[1]*transportToEye;
	transportFactors[2] = transportFactors[2]*transportToEye;

	pos[0] = pos[0] + normal[0]*0.01;
	pos[1] = pos[1] + normal[1]*0.01;
	pos[2] = pos[2] + normal[2]*0.01;

	float colorSum[] = {0.0, 0.0, 0.0};
	float s[] = {randSeed, get_global_id(0) + (int)fabs(position[0]) + (int)fabs(position[2])};


	for(int bounce = 0; bounce < MAX_BOUNCES; bounce++)
	{
		hitFaceIndex = -1;
		rhi.tmin = 99999999999.9;
		
		// find the nearest face and point from the current position 'pos' in the random direction 'dir'
		rayCastBVH(pos, dir, &rhi, &hitFaceIndex, kdTextureFloat, kdTextureInt, facesFloat, facesInt);
		if(hitFaceIndex != -1)
		{			
			float hitPos[] = {rhi.minX, rhi.minY, rhi.minZ};

			float inboundRadiance[3];
			sampleRadiance(pos, hitPos,hitFaceIndex, facesFloat, facesInt,  kdTextureFloat, kdTextureInt, material, pointLights, numLights, inboundRadiance);

			float hitDist = rhi.tmin;
			float hitTransport = 1.0 / pow(hitDist/100.0, 2.0);
			hitTransport = min(hitTransport, 1.0);

			float refl[3];
			reflection(dir, dirOut, normal, faceIndex, facesFloat, facesInt, material, refl);
			float cosWeight = dotProduct(dir,normal)*dotProduct(dirOut,normal);

			float color[3];
			color[0] = transportFactors[0] * refl[0] * inboundRadiance[0] * cosWeight / p;
			color[1] = transportFactors[1] * refl[1] * inboundRadiance[1] * cosWeight / p;
			color[2] = transportFactors[2] * refl[2] * inboundRadiance[2] * cosWeight / p;

			colorSum[0] += color[0];
			colorSum[1] += color[1];
			colorSum[2] += color[2];

			transportFactors[0] = transportFactors[0] * refl[0] * hitTransport * cosWeight;
			transportFactors[1] = transportFactors[1] * refl[1] * hitTransport * cosWeight;
			transportFactors[2] = transportFactors[2] * refl[2] * hitTransport * cosWeight;

			struct Face_float faceFHit;
			getFaceFloat(facesFloat, hitFaceIndex, &faceFHit);
			interpolateNormals(hitPos, &faceFHit, normal);
			
			float newDir[3];
			dirOut[0] = -dir[0];
			dirOut[1] = -dir[1];
			dirOut[2] = -dir[2];

			p = randomizeDirection(&faceFHit, hitPos, normal, dir, randSeed);

			pos[0] = hitPos[0] + normal[0]*0.01;
			pos[1] = hitPos[1] + normal[1]*0.01;
			pos[2] = hitPos[2] + normal[2]*0.01;
		}
		else
		{
			break;
		}
	}
	colorSum[0] = max(colorSum[0],0.0);
	colorSum[1] = max(colorSum[1],0.0);
	colorSum[2] = max(colorSum[2],0.0);

	sampleWeight[index] = sampleWeight[index] + 1.0;
	outputRadiance[index].color[0] = colorSum[0];
	outputRadiance[index].color[1] = colorSum[1];
	outputRadiance[index].color[2] = colorSum[2];
	
}

__kernel void computeDirectRadianceCache(__global float* position, __global float* direction, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit,
	__read_only image3d_t colorTex, __read_only image3d_t bumpTex)
{
	int index = get_global_id(0);

	if(faceIndexHit[index] == -1 )
	{
		outputRadiance[index].color[0] = 0.0;
		outputRadiance[index].color[1] = 0.0;
		outputRadiance[index].color[2] = 0.0;
		return;
	}

	float pos[3];
	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];

	float dir[3];
	dir[0] = -direction[3*index];
	dir[1] = -direction[3*index + 1];
	dir[2] = -direction[3*index + 2];

	struct Face_float f;
	int faceIndex = faceIndexHit[index];
	getFaceFloat(facesFloat, faceIndex, &f);
	float normal[3];
	float texCoord[2];
	interpolateNormals(pos, &f, normal);
	interpolateTextureCoords(pos, &f, texCoord);

	pos[0] = pos[0] + normal[0]*0.01;
	pos[1] = pos[1] + normal[1]*0.01;
	pos[2] = pos[2] + normal[2]*0.01;

	float rad[3];
	float origin[] = {0.0,0.0,0.0};
	directRadiance(pos, normal, dir, faceIndex, facesFloat, facesInt,  kdTextureFloat, kdTextureInt, material, pointLights, numLights, rad);

	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];
	float dist = vectorLength(pos);
	float transportFactor = 1.0 / pow(dist/100.0, 2.0);
	transportFactor = min(transportFactor, 1.0);

	int cTexIndex = facesInt[faceIndex].colorTexIndex;
	const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;
	
	float4 texSample = (float4)(1.0f, 1.0f, 1.0f, 1.0f);
	if(cTexIndex != -1)
	{
		texSample = read_imagef(colorTex, sampler, (float4)(texCoord[0], 1.0 - texCoord[1], (float)cTexIndex, 0.0));
	}
	outputRadiance[index].color[0] = rad[0] * transportFactor * texSample.x;
	outputRadiance[index].color[1] = rad[1] * transportFactor * texSample.y;
	outputRadiance[index].color[2] = rad[2] * transportFactor * texSample.z;
}

__kernel void stochasticRayTraceRadianceCache(__global float* position, __global float* directionOut, __global struct Face_float* facesFloat, __global struct Face_int* facesInt, __global struct KdTreeTextureFloat* kdTextureFloat, 
	__global struct KdTreeTextureInt* kdTextureInt, __global struct Material* material, __global struct PointLight* pointLights, int numLights, __global struct Radiance* outputRadiance, __global int* faceIndexHit, int randSeed,
	__global float* sampleWeight, __read_only image3d_t colorTex, __read_only image3d_t bumpTex
	)
{
	int index = get_global_id(0);
	int faceIndex = faceIndexHit[index];

	// if no shading is required, return.
	if(faceIndex == -1 )
	{
		sampleWeight[index] = sampleWeight[index] + 1.0;
		outputRadiance[index].color[0] = 0.0;
		outputRadiance[index].color[1] = 0.0;
		outputRadiance[index].color[2] = 0.0;
		return;
	}

	//init the starting position, and outgoing direction
	float pos[3];
	float dir[3];
	float dirOut[3];
	dirOut[0] = -directionOut[3*index];
	dirOut[1] = -directionOut[3*index + 1];
	dirOut[2] = -directionOut[3*index + 2];
	pos[0] = position[3*index];
	pos[1] = position[3*index + 1];
	pos[2] = position[3*index + 2];

	// compute the surface normal
	struct Face_float faceF;
	int mIndex = facesInt[faceIndex].materialIndex;
	
	float normal[3];
	getFaceFloat(facesFloat, faceIndex, &faceF);
	interpolateNormals(pos, &faceF, normal);

	// randomize the first direction
	float p = randomizeDirection(&faceF, pos, normal, dir, randSeed);

	float incidentRadiance;
	float transport[3];
	int hitFaceIndex = -1;
	struct RayHitInfo rhi;
	float transportFactors[] = {1.0, 1.0, 1.0};

	pos[0] = pos[0] + normal[0]*0.01;
	pos[1] = pos[1] + normal[1]*0.01;
	pos[2] = pos[2] + normal[2]*0.01;

	float colorSum[] = {0.0, 0.0, 0.0};
	float s[] = {randSeed, get_global_id(0) + (int)fabs(position[0]) + (int)fabs(position[2])};

	for(int bounce = 0; bounce < MAX_BOUNCES; bounce++)
	{
		hitFaceIndex = -1;
		rhi.tmin = 99999999999.9;
		
		// find the nearest face and point from the current position 'pos' in the random direction 'dir'
		rayCastBVH(pos, dir, &rhi, &hitFaceIndex, kdTextureFloat, kdTextureInt, facesFloat, facesInt);
		if(hitFaceIndex != -1)
		{	

			float hitPos[] = {rhi.minX, rhi.minY, rhi.minZ};

			float inboundRadiance[3];
			sampleRadiance(pos, hitPos, hitFaceIndex, facesFloat, facesInt, kdTextureFloat, kdTextureInt, material, pointLights, numLights, inboundRadiance);

			float hitDist = rhi.tmin;
			float hitTransport = 1.0 / pow(hitDist/100.0, 2.0);
			hitTransport = min(hitTransport, 1.0);

			float refl[] = {1.0,1.0,1.0};

			if(bounce > 0)
				reflection(dir, dirOut, normal, faceIndex, facesFloat, facesInt, material, refl);
			else
				reflectionDiffuse(faceIndex, facesInt, material, refl);

			float cosWeight = dotProduct(dir,normal);//*dotProduct(dirOut,normal);

			float color[3];
			color[0] = transportFactors[0] * refl[0] * inboundRadiance[0] * cosWeight / p;
			color[1] = transportFactors[1] * refl[1] * inboundRadiance[1] * cosWeight / p;
			color[2] = transportFactors[2] * refl[2] * inboundRadiance[2] * cosWeight / p;

			colorSum[0] += color[0];
			colorSum[1] += color[1];
			colorSum[2] += color[2];

			transportFactors[0] = transportFactors[0] * refl[0] * hitTransport * cosWeight;
			transportFactors[1] = transportFactors[1] * refl[1] * hitTransport * cosWeight;
			transportFactors[2] = transportFactors[2] * refl[2] * hitTransport * cosWeight;

			struct Face_float faceFHit;
			getFaceFloat(facesFloat, hitFaceIndex, &faceFHit);
			interpolateNormals(hitPos, &faceFHit, normal);
			
			float newDir[3];
			dirOut[0] = -dir[0];
			dirOut[1] = -dir[1];
			dirOut[2] = -dir[2];

			p = randomizeDirection(&faceFHit, hitPos, normal, dir, randSeed);

			pos[0] = hitPos[0] + normal[0]*0.01;
			pos[1] = hitPos[1] + normal[1]*0.01;
			pos[2] = hitPos[2] + normal[2]*0.01;
		}
		else
		{
			break;
		}
	}

	colorSum[0] = min(colorSum[0],1.0);
	colorSum[1] = min(colorSum[1],1.0);
	colorSum[2] = min(colorSum[2],1.0);

	colorSum[0] = max(colorSum[0],0.0);
	colorSum[1] = max(colorSum[1],0.0);
	colorSum[2] = max(colorSum[2],0.0);

	sampleWeight[index] = sampleWeight[index] + 1.0;
	outputRadiance[index].color[0] = min(colorSum[0],1.0);
	outputRadiance[index].color[1] = min(colorSum[1],1.0);
	outputRadiance[index].color[2] = min(colorSum[2],1.0);
}