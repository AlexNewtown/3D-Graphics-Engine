#pragma once

#include "math\Matrix.h"
#include "data structures\SphericalHarmonics.h"
#include <vector>
#include <memory>

#define SVD_BRDF_ALUMINIUM "../bin/BRDF/aluminumExport.bin"

class SVDBrdf;
class BrdfSampled;

extern std::vector<BrdfSampled*> brdfSampled;

void initBrdfSamples();
void randomizeBrdfDirection(float phiIn, float thetaIn, float &phiOut, float &thetaOut, int brdfIndex, float* sampledColor);

/* 
   outbound vaires with (i), f(i,j)
   inbound varies with (j), f(i,j)
*/

class SVDBrdf
{
	/* 
	   Structure for holding Vectors U and V
	   We multiply the Singular values into U before export.
	*/
public:
	SVDBrdf(const char* filePath);
	~SVDBrdf();
	unsigned int n;
	unsigned int matrixSize;
	float* U;
	float* V;
};

class BrdfSampled
{
public:
	BrdfSampled(char* filePath);
	~BrdfSampled();
	void sample(SphericalCoords* in, SphericalCoords* out, float* color);
	void sample(float phiIn, float thetaIn, float phiOut, float thetaOut, float* color);
	unsigned int n;
	unsigned int matrixSize;
	std::unique_ptr<SquareMatrix> brdf[3];
};
