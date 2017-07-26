#include "shading utils\ProgressivePhotonMap.h"

void HitPointFace::init()
{

	photonCount = 0;
	lastPhotonCount = 0;
	
	for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
	{
		Complex** c = new Complex*[3];
		c[0] = new Complex(0.0, 0.0);
		c[1] = new Complex(0.0, 0.0);
		c[2] = new Complex(0.0, 0.0);
		sphericalHarmonics.push_back(c);
	}
	

}

HitPointFace::HitPointFace()
{
	init();
	
}
HitPointFace::HitPointFace(Face* f)
{
	index = f->index;
	for (int i = 0; i < 3; i++)
	{
		vertIndex[i] = f->vertIndex[i];
		vertex[i] = new Vertex();
		vertex[i]->pos[0] = f->vertex[i]->pos[0];
		vertex[i]->pos[1] = f->vertex[i]->pos[1];
		vertex[i]->pos[2] = f->vertex[i]->pos[2];

		normalIndex[i] = normalIndex[i];
		normal[i] = new Vertex();
		normal[i]->pos[0] = f->normal[i]->pos[0];
		normal[i]->pos[1] = f->normal[i]->pos[1];
		normal[i]->pos[2] = f->normal[i]->pos[2];

		texCoordIndex[i] = f->texCoordIndex[i];
		texCoord[i] = new TexCoord();
		if (f->texCoord[i] != NULL)
		{
			texCoord[i]->pos[0] = f->texCoord[i]->pos[0];
			texCoord[i]->pos[1] = f->texCoord[i]->pos[1];
		}
		avg[i] = f->avg[i];
	}
	materialIndex = f->materialIndex;
	brdfIndex = f->brdfIndex;

	init();

	basisY[0] = normal[0]->pos[0];
	basisY[1] = normal[0]->pos[1];
	basisY[2] = normal[0]->pos[2];

	basisX[0] = vertex[1]->pos[0] - vertex[0]->pos[0];
	basisX[1] = vertex[1]->pos[1] - vertex[0]->pos[1];
	basisX[2] = vertex[1]->pos[2] - vertex[0]->pos[2];
	normalize(basisX);

	crossProduct(basisY, basisX, basisZ);
	normalize(basisZ);
}

void HitPointFace::removePhotons()
{

	for (int i = photonDirection.size() - 1; i >= 0; i--)
	{
		PhotonHit* ph = photonDirection[i];
		photonDirection.pop_back();
		delete ph;
	}
	
}

VertexRadiance::VertexRadiance()
{

}
VertexRadiance::VertexRadiance(Vertex* v)
{
	this->pos[0] = v->pos[0];
	this->pos[1] = v->pos[1];
	this->pos[2] = v->pos[2];
	radius = 1.0;
	lastNumPhotons = 0;

	for (int i = 0; i < SPHERICAL_HARMONICS_ACCURACY; i++)
	{
		Complex** c = new Complex*[3];
		c[0] = new Complex(0.0, 0.0);
		c[1] = new Complex(0.0, 0.0);
		c[2] = new Complex(0.0, 0.0);
		shIrradiance.push_back(c);
	}
}
VertexRadiance::~VertexRadiance()
{
	for (int i = 0; i < shIrradiance.size(); i++)
	{
		Complex** c = shIrradiance[shIrradiance.size() - 1];
		for (int j = 0; j < 3; j++)
		{
			delete c[j];
		}
		shIrradiance.pop_back();
	}

	for (int i = 0; i < photons.size(); i++)
	{
		PhotonHit* sc = photons[photons.size() - 1];
		delete sc;
		photons.pop_back();
	}
}

void VertexRadiance::removePhotons()
{
	for (int i = photons.size() - 1; i >= 0; i--)
	{
		PhotonHit* ph = photons[i];
		photons.pop_back();
		delete ph;
	}
}

VertexRadianceDiffuse::VertexRadianceDiffuse()
{
	radius = 1.0;
	radiance = 0.0;
}
VertexRadianceDiffuse::VertexRadianceDiffuse(Vertex* v)
{
	this->pos[0] = v->pos[0];
	this->pos[1] = v->pos[1];
	this->pos[2] = v->pos[2];
	radius = 1.0;
	radiance = 0.0;
}
VertexRadianceDiffuse::~VertexRadianceDiffuse()
{

}