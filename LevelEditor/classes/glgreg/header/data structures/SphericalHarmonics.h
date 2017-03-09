#pragma once
#include "data structures/Complex.h"
#include "math/Matrix.h"
#include <math.h>
#include <vector>
#define SPHERICAL_HARMONICS_ACCURACY 1
#define M_PI 3.1415926538979

struct SphericalCoords
{
	SphericalCoords(float phi, float theta);
	SphericalCoords();
	float theta;
	float phi;
};

struct PhotonHit : public SphericalCoords
{
	PhotonHit() : SphericalCoords()
	{

	}
	PhotonHit(PhotonHit &photonHit);
	float color[3];
};

void toSphericalCoords(float* basisX, float* basisY, float* basisZ, float* dir, SphericalCoords* sc);
void toSphericalCoords(float* basisY, float* dir, SphericalCoords* sc);
void toCartesianCoords(float* basisX, float* basisY, float* basisZ, float* dir, SphericalCoords* sc);

void computeSphericalHarmonics(std::vector<PhotonHit*> &sc, std::vector<Complex**> &sh);
void computeSphericalHarmonics(std::vector<SphericalCoords*> sc, std::vector<float> f, std::vector<Complex*> &sh);
void sphericalHarmonicCoefficient(float phi, float theta, int index, Complex &c);