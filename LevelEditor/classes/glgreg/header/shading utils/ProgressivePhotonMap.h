#pragma once
#include "data structures\MeshStructures.h"
#include "data structures\SphericalHarmonics.h"
#include <vector>

#define ALHPA_PROGRESSIVE_PHOTON_MAP 0.75

class HitPointFace : public Face
{
public:
	HitPointFace();
	HitPointFace(Face* f);

	std::vector<PhotonHit*> photonDirection;
	std::vector<Complex**> sphericalHarmonics;

	float basisX[3];
	float basisY[3];
	float basisZ[3];
	float midPoint[3];

	unsigned int photonCount;
	unsigned int lastPhotonCount;
	void removePhotons();

private:
	void init();
};

class VertexRadiance : public Vertex
{
public:
	VertexRadiance();
	VertexRadiance(Vertex* v);
	~VertexRadiance();
	void removePhotons();
	unsigned int lastNumPhotons;
	float radius;
	Vertex normal;
	std::vector<Complex**> shIrradiance;
	std::vector<PhotonHit*> photons;
};

class VertexRadianceDiffuse : public Vertex
{
public:
	VertexRadianceDiffuse();
	VertexRadianceDiffuse(Vertex* v);
	~VertexRadianceDiffuse();

	unsigned int lastNumPhotons;
	float radius;
	float radiance;
	Vertex normal;
};