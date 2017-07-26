#include "shading utils\SVDBrdfLoader.h"
#include <stdio.h>
#include <fstream>
#include <vector>

std::vector<BrdfSampled*> brdfSampled;

void initBrdfSamples()
{
	brdfSampled.push_back(new BrdfSampled(SVD_BRDF_ALUMINIUM));
}

SVDBrdf::SVDBrdf(const char* filePath)
{
	std::fstream file;
	file.open(filePath, std::fstream::in | std::fstream::binary);

	file.read((char*)&n, 4);
	unsigned int totalNumSamples = n * 4 * n*n * 4 * n;
	matrixSize = n * 4 * n;
	U = new float[totalNumSamples];
	V = new float[totalNumSamples];

	unsigned int index = 0;
	float f;
	for (int i = 0; i < matrixSize; i++)
	{

		for (int j = 0; j < matrixSize; j++)
		{
			file.read((char*)&f, 4);
			U[index] = f;
			index++;
		}
	}

	index = 0;
	for (int i = 0; i < matrixSize; i++)
	{

		for (int j = 0; j < matrixSize; j++)
		{
			file.read((char*)&f,4);
			V[index] = f;
			index++;
		}
	}

}

SVDBrdf::~SVDBrdf()
{
	delete U;
	delete V;
}

BrdfSampled::BrdfSampled(char* filePath)
{
	std::fstream file;
	file.open(filePath, std::fstream::in | std::fstream::binary);

	file.read((char*)&n, 4);
	unsigned int totalNumSamples = n * 4 * n*n * 4 * n;
	matrixSize = n * 4 * n;

	brdf[0].reset(new SquareMatrix(matrixSize));
	float f = 0;
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			file.read((char*)&f, 4);
			brdf[0]->set(j, i, f);
		}
	}

	brdf[1].reset(new SquareMatrix(matrixSize));
	for (int i = 0; i < matrixSize/2; i++)
	{
		for (int j = 0; j < matrixSize/2; j++)
		{
			file.read((char*)&f, 4);
			brdf[1]->set(j, i, f);
		}
	}

	brdf[2].reset(new SquareMatrix(matrixSize));
	for (int i = 0; i < matrixSize/2; i++)
	{
		for (int j = 0; j < matrixSize/2; j++)
		{
			file.read((char*)&f, 4);
			brdf[2]->set(j, i, f);
		}
	}
	//brdf[0]->exportMatrix("test.txt");

}

BrdfSampled::~BrdfSampled()
{
	delete brdf;
}

void randomizeBrdfDirection(float phiIn, float thetaIn, float &phiOut, float &thetaOut, int brdfIndex, float* color)
{
	BrdfSampled* brdf = brdfSampled[brdfIndex];
	int phiInIndex = floor(phiIn / (0.5*3.141592654 / ((float)brdf->n)));
	int thetaInIndex = floor(thetaIn/ (2*3.141592654 / (((float)brdf->n)*4) ));
	
	if (phiInIndex < 0)
		phiInIndex = 0;

	if (thetaInIndex < 0)
		thetaInIndex = 0;

	int inIndex = phiInIndex*4*brdf->n + thetaInIndex;

	

	float r = (float)rand() / RAND_MAX;
	int outSample = 0;
	float sum = 0;
	for (outSample = 0; outSample < brdf->matrixSize; outSample++)
	{
		sum = sum + brdf->brdf[0]->get(outSample, inIndex);
		if (sum > r)
			break;
	}
	if (outSample >= brdf->matrixSize)
		outSample = brdf->matrixSize - 1;

	int outPhiIndex = int(floor(outSample / (4 * brdf->n)));
	int outThetaIndex = outSample - outPhiIndex * 4 * brdf->n;

	color[0] = brdf->brdf[0]->get(outSample, inIndex);
	color[1] = brdf->brdf[1]->get((int)floor(outSample / 2), (int)floor(inIndex / 2));
	color[2] = brdf->brdf[2]->get((int)floor(outSample / 2), (int)floor(inIndex / 2));

	phiOut = (0.5*3.141592654 / ((float)brdf->n))*(float)outPhiIndex;
	thetaOut = (2 * 3.141592654 / (((float)brdf->n) * 4))*(float)outThetaIndex;
}

void BrdfSampled::sample(SphericalCoords* in, SphericalCoords* out, float* color)
{
	int phiInIndex = floor(in->phi / (0.5*3.141592654 / ((float)n)));
	int thetaInIndex = floor(in->theta / (2 * 3.141592654 / (((float)n) * 4)));
	
	if (phiInIndex < 0)
		phiInIndex = 0;

	if (thetaInIndex < 0)
		thetaInIndex = 0;
	
	int inIndex = phiInIndex * 4 * n + thetaInIndex;

	int phiOutIndex = floor(out->phi / (0.5*3.141592654 / ((float)n)));
	int thetaOutIndex = floor(out->theta / (2 * 3.141592654 / (((float)n) * 4)));

	if (phiOutIndex < 0)
		phiOutIndex = 0;

	if (thetaOutIndex < 0)
		thetaOutIndex = 0;

	int outIndex = phiOutIndex * 4 * n + thetaOutIndex;

	color[0] = brdf[0]->get(outIndex, inIndex);


	phiInIndex = floor(in->phi / (0.5*3.141592654 / ((float)n/2.0)));
	thetaInIndex = floor(in->theta / (2 * 3.141592654 / (((float)n) * 2.0)));
	inIndex = phiInIndex * 2 * n + thetaInIndex;

	phiOutIndex = floor(out->phi / (0.5*3.141592654 / ((float)n/2.0)));
	thetaOutIndex = floor(out->theta / (2 * 3.141592654 / (((float)n) * 2.0)));
	outIndex = phiOutIndex * 2 * n + thetaOutIndex;

	color[1] = brdf[1]->get(outIndex, inIndex);
	color[2] = brdf[2]->get(outIndex, inIndex);
}

void BrdfSampled::sample(float phiIn, float thetaIn, float phiOut, float thetaOut, float* color)
{
	SphericalCoords in(phiIn,thetaIn);
	SphericalCoords out(phiOut,thetaOut);
	sample(&in, &out, color);
}