#include "data structures\SphericalHarmonics.h"
#include "data structures\ray.h"
#include <math.h>

PhotonHit::PhotonHit(PhotonHit &photonHit) : SphericalCoords()
{
	this->color[0] = photonHit.color[0];
	this->color[1] = photonHit.color[1];
	this->color[2] = photonHit.color[2];

	this->phi = photonHit.phi;
	this->theta = photonHit.theta;
}

SphericalCoords::SphericalCoords(float phi, float theta)
{
	this->phi = phi;
	this->theta = theta;
}

SphericalCoords::SphericalCoords()
{

}

void toSphericalCoords(float* basisX, float* basisY, float* basisZ, float* dir, SphericalCoords* sc)
{
	float x = dotProduct(basisX, dir);
	float y = dotProduct(basisY, dir);
	float z = dotProduct(basisZ, dir);

	sc->phi = 3.141592654*0.5 - acos(y);
	sc->theta = atan2(z, x);
	sc->theta = sc->theta + 3.141592654;
}

void toSphericalCoords(float* basisY, float* dir, SphericalCoords* sc)
{
	Matrix4x4 mat;
	mat.rotate(asin(basisY[0]), 0.0, 1.0, 0.0);
	mat.rotate(asin(basisY[1]), 1.0, 0.0, 0.0);

	float basisX[3];
	float basisZ[3];
	basisX[0] = mat.get(0, 0);
	basisX[1] = mat.get(1, 0);
	basisX[2] = mat.get(2, 0);

	basisZ[0] = mat.get(0, 1);
	basisZ[1] = mat.get(1, 1);
	basisZ[2] = mat.get(2, 1);

	float x = dotProduct(basisX, dir);
	float y = dotProduct(basisY, dir);
	float z = dotProduct(basisZ, dir);

	sc->phi = 3.141592654*0.5 - acos(y);
	sc->theta = atan2(z, x);
	sc->theta = sc->theta + 3.141592654;
}

void toCartesianCoords(float* basisX, float* basisY, float* basisZ, float* dir, SphericalCoords* sc)
{
	float wx = cos(sc->theta)*cos(sc->phi);
	float wy = sin(sc->phi);
	float wz = sin(sc->theta)*cos(sc->phi);

	dir[0] = wx*basisX[0] + wy*basisY[0] + wz*basisZ[0];
	dir[1] = wx*basisX[1] + wy*basisY[1] + wz*basisZ[1];
	dir[2] = wx*basisX[2] + wy*basisY[2] + wz*basisZ[2];
}

void computeSphericalHarmonics(std::vector<PhotonHit*> &sc, std::vector<Complex**> &sh)
{
	Complex c(0.0, 0.0);
	Complex sum;
	for (int shIndex = 0; shIndex < SPHERICAL_HARMONICS_ACCURACY; shIndex++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (sc.size() != 0)
			{
				sum.imag = 0.0;
				sum.real = 0.0;
				for (int i = 0; i < sc.size(); i++)
				{
					sphericalHarmonicCoefficient(sc[i]->theta, sc[i]->phi, shIndex, c);
					sum.real = sum.real + c.real*sin(sc[i]->phi)*sc[i]->color[j];
					sum.imag = sum.imag + c.imag*sin(sc[i]->phi)*sc[i]->color[j];
				}

				sh[shIndex][j]->real = sh[shIndex][j]->real + sum.real;
				sh[shIndex][j]->imag = sh[shIndex][j]->imag + sum.imag;
			}
		}
	}
	
}

void computeSphericalHarmonics(std::vector<SphericalCoords*> sc, std::vector<float> f, std::vector<Complex*> &sh)
{
	Complex c;
	Complex sum;
	for (int shIndex = 0; shIndex < SPHERICAL_HARMONICS_ACCURACY; shIndex++)
	{
		if (sc.size() != 0)
		{
			sum.imag = 0.0;
			sum.real = 0.0;
			for (int i = 0; i < sc.size(); i++)
			{
				sphericalHarmonicCoefficient(sc[i]->theta, sc[i]->phi, shIndex, c);
				sum.real = sum.real + c.real*f[i];
				sum.imag = sum.imag + c.imag*f[i];
			}
			sh[shIndex]->real = sh[shIndex]->real + sum.real/sc.size();
			sh[shIndex]->imag = sh[shIndex]->imag + sum.imag/sc.size();
		}
		
	}
}

void sphericalHarmonicCoefficient(float phi, float theta, int index, Complex &c)
{
	c.real = 0.0;
	c.imag = 0.0;

	if (index == 0)
	{
		c.real = 0.5*sqrt(1.0 / M_PI);
		c.imag = 0.0;
	}
	else if (index == 1)
	{
		c.real = 0.5*sqrt(3.0 / (2 * M_PI))*sin(theta)*cos(-phi);
		c.imag = 0.5*sqrt(3.0 / (2 * M_PI))*sin(theta)*sin(-phi);
	}
	else if (index == 2)
	{
		c.real = 0.5*sqrt(3.0 / M_PI)*cos(theta);
		c.imag = 0.0;
	}
	else if (index == 3)
	{
		c.real = -0.5*sqrt(3.0 / (2 * M_PI))*sin(theta)*cos(phi);
		c.imag = -0.5*sqrt(3.0 / (2 * M_PI))*sin(theta)*sin(phi);
	}
	else if (index == 4)
	{
		c.real = 0.25*sqrt(15.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(-2 * phi);
		c.imag = 0.25*sqrt(15.0 / (2 * M_PI))*pow(sin(theta), 2.0)*sin(-2 * phi);
	}
	else if (index == 5)
	{
		c.real = 0.5*sqrt(15.0 / (2 * M_PI))*sin(theta)*cos(theta)*cos(-phi);
		c.imag = 0.5*sqrt(15.0 / (2 * M_PI))*sin(theta)*cos(theta)*sin(-phi);
		
	}
	else if (index == 6)
	{
		c.real = 0.25*sqrt(5.0 / M_PI)*(3 * pow(cos(theta), 2.0) - 1.0);
		c.imag = 0.0;
		
	}
	else if (index == 7)
	{
		c.real = -0.5*sqrt(15.0 / (2 * M_PI))*sin(theta)*cos(theta)*cos(phi);
		c.imag = -0.5*sqrt(15.0 / (2 * M_PI))*sin(theta)*cos(theta)*sin(phi);
		
	}
	else if (index == 8)
	{
		c.real = 0.25*sqrt(15.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(2 * phi);
		c.imag = 0.25*sqrt(15.0 / (2 * M_PI))*pow(sin(theta), 2.0)*sin(2 * phi);
		
	}
	else if (index == 9)
	{
		c.real = (1.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(-3 * phi);
		c.imag = (1.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*sin(-3 * phi);
		
	}
	else if (index == 10)
	{
		c.real = 0.25*sqrt(105.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(theta)*cos(-2 * phi);
		c.imag = 0.25*sqrt(105.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(theta)*sin(-2 * phi);
		
	}
	else if (index == 11)
	{
		c.real = (1.0 / 8)*sqrt(21.0 / M_PI)*sin(theta)*(5.0*pow(cos(theta), 2.0) - 1.0)*cos(-phi);
		c.imag = (1.0 / 8)*sqrt(21.0 / M_PI)*sin(theta)*(5.0*pow(cos(theta), 2.0) - 1.0)*sin(-phi);
		
	}
	else if (index == 12)
	{
		c.real = 0.25*sqrt(7.0 / M_PI)*(5 * pow(cos(theta), 3.0) - 3.0*cos(theta));
		c.imag = 0.0;
		
	}
	else if (index == 13)
	{
		c.real = (-1.0 / 8)*sqrt(21.0 / M_PI)*sin(theta)*(5 * pow(cos(theta), 2.0) - 1.0)*cos(phi);
		c.imag = (-1.0 / 8)*sqrt(21.0 / M_PI)*sin(theta)*(5 * pow(cos(theta), 2.0) - 1.0)*sin(phi);
		
	}
	else if (index == 14)
	{
		c.real = 0.25*sqrt(105.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(theta)*cos(2 * phi);
		c.imag = 0.25*sqrt(105.0 / (2 * M_PI))*pow(sin(theta), 2.0)*cos(theta)*sin(2 * phi);
		
	}
	else if (index == 15)
	{
		c.real = (-1.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(3 * phi);
		c.imag = (-1.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*sin(3 * phi);
		
	}
	else if (index == 16)
	{
		c.real = (3.0 / 16)*sqrt(35.0 / (2 * M_PI))*pow(sin(theta), 4.0)*cos(-4.0*phi);
		c.imag = (3.0 / 16)*sqrt(35.0 / (2 * M_PI))*pow(sin(theta), 4.0)*sin(-4.0*phi);
		
	}
	else if (index == 17)
	{
		c.real = (3.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(theta)*cos(-3.0*phi);
		c.imag = (3.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(theta)*sin(-3.0*phi);
		
	}
	else if (index == 18)
	{
		c.real = (3.0 / 8)*sqrt(5.0 / (2 * M_PI))*pow(sin(theta), 2.0)*(7 * pow(cos(theta), 2.0) - 1.0)*cos(-2.0*phi);
		c.imag = (3.0 / 8)*sqrt(5.0 / (2 * M_PI))*pow(sin(theta), 2.0)*(7 * pow(cos(theta), 2.0) - 1.0)*sin(-2.0*phi);
		
	}

	else if (index == 19)
	{
		c.real = (3.0 / 8)*sqrt(5.0 / (M_PI))*sin(theta)*(7 * pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(-phi);
		c.imag = (3.0 / 8)*sqrt(5.0 / (M_PI))*sin(theta)*(7 * pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(-phi);
		
	}

	else if (index == 20)
	{
		c.real = (3.0 / 16)*sqrt(1 / M_PI)*(35 * pow(cos(theta), 4.0) - 30.0*pow(cos(theta), 2.0) + 3.0);
		c.imag = 0.0;
		
	}

	else if (index == 21)
	{
		c.real = (-3.0 / 8)*sqrt(5.0 / M_PI)*sin(theta)*(7 * pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(phi);
		c.imag = (-3.0 / 8)*sqrt(5.0 / M_PI)*sin(theta)*(7 * pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(phi);
		
	}

	else if (index == 22)
	{
		c.real = (3.0 / 8)*sqrt(5.0 / (2 * M_PI))*pow(sin(theta), 2.0)*(7 * pow(cos(theta), 2.0) - 1.0)*cos(2.0*phi);
		c.imag = (3.0 / 8)*sqrt(5.0 / (2 * M_PI))*pow(sin(theta), 2.0)*(7 * pow(cos(theta), 2.0) - 1.0)*sin(2.0*phi);
		
	}

	else if (index == 23)
	{
		c.real = (-3.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(theta)*cos(3.0*phi);
		c.imag = (-3.0 / 8)*sqrt(35.0 / M_PI)*pow(sin(theta), 3.0)*cos(theta)*sin(3.0*phi);
		
	}

	else if (index == 24)
	{
		c.real = (3.0 / 16)*sqrt(35.0 / (2 * M_PI))*pow(sin(theta), 4.0)*cos(4.0*phi);
		c.imag = (3.0 / 16)*sqrt(35.0 / (2 * M_PI))*pow(sin(theta), 4.0)*sin(4.0*phi);
			
	}
	else if (index == 25)
	{
		c.real = (3.0 / 32.0)*sqrt(77.0 / M_PI)*cos(-5.0*phi)*pow(sin(theta), 5.0);
		c.imag = (3.0 / 32.0)*sqrt(77.0 / M_PI)*sin(-5.0*phi)*pow(sin(theta), 5.0);
	}
	else if (index == 26)
	{
		c.real = (3.0 / 16.0)*sqrt(385.0 / (2.0*M_PI))*cos(-4.0*phi)*pow(sin(theta), 4.0)*cos(theta);
		c.imag = (3.0 / 16.0)*sqrt(385.0 / (2.0*M_PI))*sin(-4.0*phi)*pow(sin(theta), 4.0)*cos(theta);
	}
	else if (index == 27)
	{
		c.real = (1.0 / 32.0)*sqrt(385.0 / (M_PI))*cos(-3.0*phi)*pow(sin(theta), 3.0)*(9.0*pow(cos(theta), 2.0) - 1);
		c.imag = (1.0 / 32.0)*sqrt(385.0 / (M_PI))*sin(-3.0*phi)*pow(sin(theta), 3.0)*(9.0*pow(cos(theta), 2.0) - 1);
	}
	else if (index == 28)
	{
		c.real = (1.0 / 8.0)*sqrt(1155.0 / (2.0*M_PI))*cos(-2.0*phi)*pow(sin(theta), 2.0)*(3.0*pow(cos(theta), 3.0) - cos(theta));
		c.imag = (1.0 / 8.0)*sqrt(1155.0 / (2.0*M_PI))*sin(-2.0*phi)*pow(sin(theta), 2.0)*(3.0*pow(cos(theta), 3.0) - cos(theta));
	}
	else if (index == 29)
	{
		c.real = (1.0 / 16.0)*sqrt(165.0 / (2.0*M_PI))*cos(-1.0*phi)*sin(theta)*(21.0*pow(cos(theta), 4.0) - 14 * pow(cos(theta), 2.0) + 1);
		c.imag = (1.0 / 16.0)*sqrt(165.0 / (2.0*M_PI))*sin(-1.0*phi)*sin(theta)*(21.0*pow(cos(theta), 4.0) - 14 * pow(cos(theta), 2.0) + 1);
	}
	else if (index == 30)
	{
		c.real = (1.0 / 16.0)*sqrt(11.0 / (M_PI))*(63.0*pow(cos(theta), 5.0) - 70 * pow(cos(theta), 3.0) + 15*cos(theta));
		c.imag = 0;
	}
	else if (index == 31)
	{
		c.real = (-1.0 / 16.0)*sqrt(165.0 / (2.0*M_PI))*cos(phi)*sin(theta)*(21.0*pow(cos(theta), 4.0) - 14 * pow(cos(theta), 2.0) + 1);
		c.imag = (-1.0 / 16.0)*sqrt(165.0 / (2.0*M_PI))*sin(phi)*sin(theta)*(21.0*pow(cos(theta), 4.0) - 14 * pow(cos(theta), 2.0) + 1);
	}
	else if (index == 32)
	{
		c.real = (1.0 / 8.0)*sqrt(1155.0 / (2.0*M_PI))*cos(2.0*phi)*pow(sin(theta), 2.0)*(3.0*pow(cos(theta), 3.0) - cos(theta));
		c.imag = (1.0 / 8.0)*sqrt(1155.0 / (2.0*M_PI))*sin(2.0*phi)*pow(sin(theta), 2.0)*(3.0*pow(cos(theta), 3.0) - cos(theta));
	}
	else if (index == 33)
	{
		c.real = (-1.0 / 32.0)*sqrt(385.0 / (M_PI))*cos(3.0*phi)*pow(sin(theta), 3.0)*(9.0*pow(cos(theta), 2.0) - 1);
		c.imag = (-1.0 / 32.0)*sqrt(385.0 / (M_PI))*sin(3.0*phi)*pow(sin(theta), 3.0)*(9.0*pow(cos(theta), 2.0) - 1);
	}
	else if (index == 34)
	{
		c.real = (3.0 / 16.0)*sqrt(385.0 / (2.0*M_PI))*cos(4.0*phi)*pow(sin(theta), 4.0)*cos(theta);
		c.imag = (3.0 / 16.0)*sqrt(385.0 / (2.0*M_PI))*sin(4.0*phi)*pow(sin(theta), 4.0)*cos(theta);
	}
	else if (index == 35)
	{
		c.real = (-3.0 / 32.0)*sqrt(77.0 / M_PI)*cos(5.0*phi)*pow(sin(theta), 5.0);
		c.imag = (-3.0 / 32.0)*sqrt(77.0 / M_PI)*sin(5.0*phi)*pow(sin(theta), 5.0);
	}

	else if (index == 36)	//  I = 6
	{
		c.real = (1.0 / 64.0)*sqrt(3003.0 / M_PI)*pow(sin(theta), 6.0)*cos(-6.0*phi);
		c.imag = (1.0 / 64.0)*sqrt(3003.0 / M_PI)*pow(sin(theta), 6.0)*sin(-6.0*phi);
	}
	else if (index == 37)
	{
		c.real = (3.0 / 32.0)*sqrt(1001.0 / M_PI)*cos(theta)*pow(sin(theta), 5.0)*cos(-5.0*phi);
		c.imag = (3.0 / 32.0)*sqrt(1001.0 / M_PI)*cos(theta)*pow(sin(theta), 5.0)*sin(-5.0*phi);
	}
	else if (index == 38)
	{
		c.real = (3.0 / 32.0)*sqrt(91.0 / (2.0*M_PI))*(11.0*pow(cos(theta), 2.0) - 1.0)*pow(sin(theta), 4.0)*cos(-4.0*phi);
		c.imag = (3.0 / 32.0)*sqrt(91.0 / (2.0*M_PI))*(11.0*pow(cos(theta), 2.0) - 1.0)*pow(sin(theta), 4.0)*sin(-4.0*phi);
	}
	else if (index == 39)
	{
		c.real = (1.0 / 32.0)*sqrt(1365.0 / (M_PI))*(11.0*pow(cos(theta), 3.0) - 3*cos(theta))*pow(sin(theta), 3.0)*cos(-3.0*phi);
		c.imag = (1.0 / 32.0)*sqrt(1365.0 / (M_PI))*(11.0*pow(cos(theta), 3.0) - 3*cos(theta))*pow(sin(theta), 3.0)*sin(-3.0*phi);
	}
	else if (index == 40)
	{
		c.real = (1.0 / 64.0)*sqrt(1365.0 / (M_PI))*(33.0*pow(cos(theta), 4.0) - 18 * pow(cos(theta),2.0) + 1.0)*pow(sin(theta), 2.0)*cos(-2.0*phi);
		c.imag = (1.0 / 64.0)*sqrt(1365.0 / (M_PI))*(33.0*pow(cos(theta), 4.0) - 18 * pow(cos(theta),2.0) + 1.0)*pow(sin(theta), 2.0)*sin(-2.0*phi);
	}
	else if (index == 41)
	{
		c.real = (1.0 / 16.0)*sqrt(273.0 / (2.0*M_PI))*(33.0*pow(cos(theta), 5.0) - 30 * pow(cos(theta), 3.0) + 5.0*cos(theta))*pow(sin(theta), 1.0)*cos(-1.0*phi);
		c.imag = (1.0 / 16.0)*sqrt(273.0 / (2.0*M_PI))*(33.0*pow(cos(theta), 5.0) - 30 * pow(cos(theta), 3.0) + 5.0*cos(theta))*pow(sin(theta), 1.0)*sin(-1.0*phi);
	}
	else if (index == 42)
	{
		c.real = (1.0 / 32.0)*sqrt(13.0 / M_PI)*(231 * pow(cos(theta), 6.0) - 315 * pow(cos(theta), 4.0) + 105 * pow(cos(theta), 2.0) - 5.0);
		c.imag = 0.0;
	}
	else if (index == 43)
	{
		c.real = (-1.0 / 16.0)*sqrt(273.0 / (2.0*M_PI))*(33.0*pow(cos(theta), 5.0) - 30 * pow(cos(theta), 3.0) + 5.0*cos(theta))*pow(sin(theta), 1.0)*cos(1.0*phi);
		c.imag = (-1.0 / 16.0)*sqrt(273.0 / (2.0*M_PI))*(33.0*pow(cos(theta), 5.0) - 30 * pow(cos(theta), 3.0) + 5.0*cos(theta))*pow(sin(theta), 1.0)*sin(1.0*phi);
	}
	else if (index == 44)
	{
		c.real = (1.0 / 64.0)*sqrt(1365.0 / (M_PI))*(33.0*pow(cos(theta), 4.0) - 18 * pow(cos(theta), 2.0) + 1.0)*pow(sin(theta), 2.0)*cos(2.0*phi);
		c.imag = (1.0 / 64.0)*sqrt(1365.0 / (M_PI))*(33.0*pow(cos(theta), 4.0) - 18 * pow(cos(theta), 2.0) + 1.0)*pow(sin(theta), 2.0)*sin(2.0*phi);
	}
	else if (index == 45)
	{
		c.real = (-1.0 / 32.0)*sqrt(1365.0 / (M_PI))*(11.0*pow(cos(theta), 3.0) - 3 * cos(theta))*pow(sin(theta), 3.0)*cos(3.0*phi);
		c.imag = (-1.0 / 32.0)*sqrt(1365.0 / (M_PI))*(11.0*pow(cos(theta), 3.0) - 3 * cos(theta))*pow(sin(theta), 3.0)*sin(3.0*phi);
	}
	else if (index == 46)
	{
		c.real = (3.0 / 32.0)*sqrt(91.0 / (2.0*M_PI))*(11.0*pow(cos(theta), 2.0) - 1.0)*pow(sin(theta), 4.0)*cos(4.0*phi);
		c.imag = (3.0 / 32.0)*sqrt(91.0 / (2.0*M_PI))*(11.0*pow(cos(theta), 2.0) - 1.0)*pow(sin(theta), 4.0)*sin(4.0*phi);
	}
	else if (index == 47)
	{
		c.real = (-3.0 / 32.0)*sqrt(1001.0 / M_PI)*cos(theta)*pow(sin(theta), 5.0)*cos(5.0*phi);
		c.imag = (-3.0 / 32.0)*sqrt(1001.0 / M_PI)*cos(theta)*pow(sin(theta), 5.0)*sin(5.0*phi);
	}
	else if (index == 48)
	{
		c.real = (1.0 / 64.0)*sqrt(3003.0 / M_PI)*pow(sin(theta), 6.0)*cos(6.0*phi);
		c.imag = (1.0 / 64.0)*sqrt(3003.0 / M_PI)*pow(sin(theta), 6.0)*sin(6.0*phi);
	}

	else if (index == 49) // I = 7
	{
		c.real = (3.0 / 64.0)*sqrt(715.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(-7.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(715.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*sin(-7.0*phi);
	}
	else if (index == 50)
	{
		c.real = (3.0 / 64.0)*sqrt(5005.0 / (M_PI))*pow(sin(theta), 6.0)*cos(theta)*cos(-6.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(5005.0 / (M_PI))*pow(sin(theta), 6.0)*cos(theta)*sin(-6.0*phi);
	}
	else if (index == 51)
	{
		c.real = (3.0 / 64.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(13.0*pow(cos(theta), 2.0) - 1.0)*cos(-5.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(13.0*pow(cos(theta), 2.0) - 1.0)*sin(-5.0*phi);
	}
	else if (index == 52)
	{
		c.real = (3.0 / 32.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(13.0*pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(-4.0*phi);
		c.imag = (3.0 / 32.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(13.0*pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(-4.0*phi);
	}
	else if (index == 53)
	{
		c.real = (3.0 / 64.0)*sqrt(35.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(143.0*pow(cos(theta), 4.0) - 66.0*pow(cos(theta),2.0) + 3.0)*cos(-3.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(35.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(143.0*pow(cos(theta), 4.0) - 66.0*pow(cos(theta),2.0) + 3.0)*sin(-3.0*phi);
	}
	else if (index == 54)
	{
		c.real = (3.0 / 64.0)*sqrt(35.0 / M_PI)*pow(sin(theta), 2.0)*(143.0*pow(cos(theta), 5.0) - 110 * pow(cos(theta), 3.0) + 15 * cos(theta))*cos(-2.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(35.0 / M_PI)*pow(sin(theta), 2.0)*(143.0*pow(cos(theta), 5.0) - 110 * pow(cos(theta), 3.0) + 15 * cos(theta))*sin(-2.0*phi);
	}
	else if (index == 55)
	{
		c.real = (1.0 / 64.0)*sqrt(105.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(429.0*pow(cos(theta), 6.0) - 495.0 * pow(cos(theta), 4.0) + 135.0 * pow(cos(theta),2.0) - 5.0)*cos(-phi);
		c.imag = (1.0 / 64.0)*sqrt(105.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(429.0*pow(cos(theta), 6.0) - 495.0 * pow(cos(theta), 4.0) + 135.0 * pow(cos(theta),2.0) - 5.0)*sin(-phi);
	}
	else if (index == 56)
	{
		c.real = (1.0 / 32.0)*sqrt(15.0 / M_PI)*(429.0*pow(cos(theta), 7.0) - 693.0*pow(cos(theta), 5.0) + 315.0*pow(cos(theta), 3.0) - 35.0*cos(theta));
		c.imag = 0.0;
	}
	else if (index == 57)
	{
		c.real = (-1.0 / 64.0)*sqrt(105.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(429.0*pow(cos(theta), 6.0) - 495.0 * pow(cos(theta), 4.0) + 135.0 * pow(cos(theta), 2.0) - 5.0)*cos(phi);
		c.imag = (-1.0 / 64.0)*sqrt(105.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(429.0*pow(cos(theta), 6.0) - 495.0 * pow(cos(theta), 4.0) + 135.0 * pow(cos(theta), 2.0) - 5.0)*sin(phi);
	}
	else if (index == 58)
	{
		c.real = (3.0 / 64.0)*sqrt(35.0 / M_PI)*pow(sin(theta), 2.0)*(143.0*pow(cos(theta), 5.0) - 110 * pow(cos(theta), 3.0) + 15.0 * cos(theta))*cos(2.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(35.0 / M_PI)*pow(sin(theta), 2.0)*(143.0*pow(cos(theta), 5.0) - 110 * pow(cos(theta), 3.0) + 15.0 * cos(theta))*sin(2.0*phi);
	}
	else if (index == 59)
	{
		c.real = (-3.0 / 64.0)*sqrt(35.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(143.0*pow(cos(theta), 4.0) - 66.0*pow(cos(theta), 2.0) + 3.0)*cos(3.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(35.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(143.0*pow(cos(theta), 4.0) - 66.0*pow(cos(theta), 2.0) + 3.0)*sin(3.0*phi);
	}
	else if (index == 60)
	{
		c.real = (3.0 / 32.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(13.0*pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(4.0*phi);
		c.imag = (3.0 / 32.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(13.0*pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(4.0*phi);
	}
	else if (index == 61)
	{
		c.real = (-3.0 / 64.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(13.0*pow(cos(theta), 2.0) - 1.0)*cos(5.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(385.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(13.0*pow(cos(theta), 2.0) - 1.0)*sin(5.0*phi);
	}
	else if (index == 62)
	{
		c.real = (3.0 / 64.0)*sqrt(5005.0 / (M_PI))*pow(sin(theta), 6.0)*cos(theta)*cos(6.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(5005.0 / (M_PI))*pow(sin(theta), 6.0)*cos(theta)*sin(6.0*phi);
	}
	else if (index == 63)
	{
		c.real = (-3.0 / 64.0)*sqrt(715.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(7.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(715.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*sin(7.0*phi);
	}
	
	else if (index == 64)	// I = 8
	{
		c.real = (3.0 / 256.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(-8.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*sin(-8.0*phi);
	}
	else if (index == 65)
	{
		c.real = (3.0 / 64.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(theta)*cos(-7.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(theta)*sin(-7.0*phi);
	}
	else if (index == 66)
	{
		c.real = (1.0 / 128.0)*sqrt(7293.0 / (M_PI))*pow(sin(theta), 6.0)*(15.0 * pow(cos(theta), 2.0) - 1.0)*cos(-6.0*phi);
		c.imag = (1.0 / 128.0)*sqrt(7293.0 / (M_PI))*pow(sin(theta), 6.0)*(15.0 * pow(cos(theta), 2.0) - 1.0)*sin(-6.0*phi);
	}
	else if (index == 67)
	{
		c.real = (3.0 / 64.0)*sqrt(17017.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(5.0 * pow(cos(theta), 3.0) - cos(theta))*cos(-5.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(17017.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(5.0 * pow(cos(theta), 3.0) - cos(theta))*sin(-5.0*phi);
	}
	else if (index == 68)
	{
		c.real = (3.0 / 128.0)*sqrt(1309.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(65.0 * pow(cos(theta), 4.0) - 26.0*pow(cos(theta), 2.0) + 1.0)*cos(-4.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(1309.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(65.0 * pow(cos(theta), 4.0) - 26.0*pow(cos(theta), 2.0) + 1.0)*sin(-4.0*phi);
	}
	else if (index == 69)
	{
		c.real = (1.0 / 64.0)*sqrt(19635.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(39.0 * pow(cos(theta), 5.0) - 26.0*pow(cos(theta), 3.0) + 3.0*cos(theta))*cos(-3.0*phi);
		c.imag = (1.0 / 64.0)*sqrt(19635.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(39.0 * pow(cos(theta), 5.0) - 26.0*pow(cos(theta), 3.0) + 3.0*cos(theta))*sin(-3.0*phi);
	}
	else if (index == 70)
	{
		c.real = (3.0 / 128.0)*sqrt(595.0 / (M_PI))*pow(sin(theta), 2.0)*(143.0 * pow(cos(theta), 6.0) - 143.0*pow(cos(theta), 4.0) + 33.0*pow(cos(theta),2.0) - 1.0)*cos(-2.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(595.0 / (M_PI))*pow(sin(theta), 2.0)*(143.0 * pow(cos(theta), 6.0) - 143.0*pow(cos(theta), 4.0) + 33.0*pow(cos(theta),2.0) - 1.0)*sin(-2.0*phi);
	}
	else if (index == 71)
	{
		c.real = (3.0 / 64.0)*sqrt(17.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(715.0 * pow(cos(theta), 7.0) - 1001.0*pow(cos(theta), 5.0) + 385.0*pow(cos(theta), 3.0) - 35.0*cos(theta))*cos(-1.0*phi);
		c.imag = (3.0 / 64.0)*sqrt(17.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(715.0 * pow(cos(theta), 7.0) - 1001.0*pow(cos(theta), 5.0) + 385.0*pow(cos(theta), 3.0) - 35.0*cos(theta))*sin(-1.0*phi);
	}
	else if (index == 72)
	{
		c.real = (1.0 / 256.0)*sqrt(17.0 / (M_PI))*(6435.0*pow(cos(theta), 8.0) - 12012.0*pow(cos(theta), 6.0) + 6930 * pow(cos(theta), 4.0) - 1260 * pow(cos(theta), 2.0) + 35.0);
		c.imag = 0.0;
	}
	else if (index == 73)
	{
		c.real = (-3.0 / 64.0)*sqrt(17.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(715.0 * pow(cos(theta), 7.0) - 1001.0*pow(cos(theta), 5.0) + 385.0*pow(cos(theta), 3.0) - 35.0*cos(theta))*cos(1.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(17.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(715.0 * pow(cos(theta), 7.0) - 1001.0*pow(cos(theta), 5.0) + 385.0*pow(cos(theta), 3.0) - 35.0*cos(theta))*sin(1.0*phi);
	}
	else if (index == 74)
	{
		c.real = (3.0 / 128.0)*sqrt(595.0 / (M_PI))*pow(sin(theta), 2.0)*(143.0 * pow(cos(theta), 6.0) - 143.0*pow(cos(theta), 4.0) + 33.0*pow(cos(theta), 2.0) - 1.0)*cos(2.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(595.0 / (M_PI))*pow(sin(theta), 2.0)*(143.0 * pow(cos(theta), 6.0) - 143.0*pow(cos(theta), 4.0) + 33.0*pow(cos(theta), 2.0) - 1.0)*sin(2.0*phi);
	}
	else if (index == 75)
	{
		c.real = (-1.0 / 64.0)*sqrt(19635.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(39.0 * pow(cos(theta), 5.0) - 26.0*pow(cos(theta), 3.0) + 3.0*cos(theta))*cos(3.0*phi);
		c.imag = (-1.0 / 64.0)*sqrt(19635.0 / (2.0*M_PI))*pow(sin(theta), 3.0)*(39.0 * pow(cos(theta), 5.0) - 26.0*pow(cos(theta), 3.0) + 3.0*cos(theta))*sin(3.0*phi);
	}
	else if (index == 76)
	{
		c.real = (3.0 / 128.0)*sqrt(1309.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(65.0 * pow(cos(theta), 4.0) - 26.0*pow(cos(theta), 2.0) + 1.0)*cos(4.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(1309.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(65.0 * pow(cos(theta), 4.0) - 26.0*pow(cos(theta), 2.0) + 1.0)*sin(4.0*phi);
	}
	else if (index == 77)
	{
		c.real = (-3.0 / 64.0)*sqrt(17017.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(5.0 * pow(cos(theta), 3.0) - cos(theta))*cos(5.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(17017.0 / (2.0*M_PI))*pow(sin(theta), 5.0)*(5.0 * pow(cos(theta), 3.0) - cos(theta))*sin(5.0*phi);
	}
	else if (index == 78)
	{
		c.real = (1.0 / 128.0)*sqrt(7293.0 / (M_PI))*pow(sin(theta), 6.0)*(15.0 * pow(cos(theta), 2.0) - 1.0)*cos(6.0*phi);
		c.imag = (1.0 / 128.0)*sqrt(7293.0 / (M_PI))*pow(sin(theta), 6.0)*(15.0 * pow(cos(theta), 2.0) - 1.0)*sin(6.0*phi);
	}
	else if (index == 79)
	{
		c.real = (-3.0 / 64.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(theta)*cos(7.0*phi);
		c.imag = (-3.0 / 64.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 7.0)*cos(theta)*sin(7.0*phi);
	}
	else if (index == 80)
	{
		c.real = (3.0 / 256.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(8.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(12155.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*sin(8.0*phi);
	}
	else if (index == 81)	// I = 9
	{
		c.real = (1.0 / 512.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 9.0)*cos(-9.0*phi);
		c.imag = (1.0 / 512.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 9.0)*sin(-9.0*phi);
	}
	else if (index == 82)
	{
		c.real = (3.0 / 256.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(theta)*cos(-8.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(theta)*sin(-8.0*phi);
	}
	else if (index == 83)
	{
		c.real = (3.0 / 512.0)*sqrt(13585.0 / (M_PI))*pow(sin(theta), 7.0)*(17.0*pow(cos(theta),2.0) - 1.0)*cos(-7.0*phi);
		c.imag = (3.0 / 512.0)*sqrt(13585.0 / (M_PI))*pow(sin(theta), 7.0)*(17.0 * pow(cos(theta), 2.0) - 1.0)*sin(-7.0*phi);
	}
	else if (index == 84)
	{
		c.real = (1.0 / 128.0)*sqrt(40755.0 / (M_PI))*pow(sin(theta), 6.0)*(17.0 * pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(-6.0*phi);
		c.imag = (1.0 / 128.0)*sqrt(40755.0 / (M_PI))*pow(sin(theta), 6.0)*(17.0 * pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(-6.0*phi);
	}
	else if (index == 85)
	{
		c.real = (3.0 / 256.0)*sqrt(2717.0 / (M_PI))*pow(sin(theta), 5.0)*(85.0 * pow(cos(theta), 4.0) - 30.0*pow(cos(theta),2.0) + 1.0)*cos(-5.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(2717.0 / (M_PI))*pow(sin(theta), 5.0)*(85.0 * pow(cos(theta), 4.0) - 30.0*pow(cos(theta),2.0) + 1.0)*sin(-5.0*phi);
	}
	else if (index == 86)
	{
		c.real = (3.0 / 128.0)*sqrt(95095.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(17.0 * pow(cos(theta), 5.0) - 10.0*pow(cos(theta), 3.0) + cos(theta))*cos(-4.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(95095.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(17.0 * pow(cos(theta), 5.0) - 10.0*pow(cos(theta), 3.0) + cos(theta))*sin(-4.0*phi);
	}
	else if (index == 87)
	{
		c.real = (1.0 / 256.0)*sqrt(21945.0 / (M_PI))*pow(sin(theta), 3.0)*(221.0 * pow(cos(theta), 6.0) - 195.0*pow(cos(theta), 4.0) + 39*pow(cos(theta),2.0) - 1.0)*cos(-3.0*phi);
		c.imag = (1.0 / 256.0)*sqrt(21945.0 / (M_PI))*pow(sin(theta), 3.0)*(221.0 * pow(cos(theta), 6.0) - 195.0*pow(cos(theta), 4.0) + 39*pow(cos(theta),2.0) - 1.0)*sin(-3.0*phi);
	}
	else if (index == 88)
	{
		c.real = (3.0 / 128.0)*sqrt(1045.0 / (M_PI))*pow(sin(theta), 2.0)*(221.0 * pow(cos(theta), 7.0) - 273.0*pow(cos(theta), 5.0) + 91.0 * pow(cos(theta), 3.0) - 7.0*cos(theta))*cos(-2.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(1045.0 / (M_PI))*pow(sin(theta), 2.0)*(221.0 * pow(cos(theta), 7.0) - 273.0*pow(cos(theta), 5.0) + 91.0 * pow(cos(theta), 3.0) - 7.0*cos(theta))*sin(-2.0*phi);
	}
	else if (index == 89)
	{
		c.real = (3.0 / 256.0)*sqrt(95.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(2431.0 * pow(cos(theta), 8.0) - 4004.0*pow(cos(theta), 6.0) + 2002.0 * pow(cos(theta), 4.0) - 308.0*pow(cos(theta),2.0) + 7.0)*cos(-1.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(95.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(2431.0 * pow(cos(theta), 8.0) - 4004.0*pow(cos(theta), 6.0) + 2002.0 * pow(cos(theta), 4.0) - 308.0*pow(cos(theta),2.0) + 7.0)*sin(-1.0*phi);
	}
	else if (index == 90)
	{
		c.real = (1.0 / 256.0)*sqrt(19.0 / (M_PI))*(12155.0 * pow(cos(theta), 9.0) - 25740.0*pow(cos(theta), 7.0) + 18018.0 * pow(cos(theta), 5.0) - 4620.0*pow(cos(theta), 3.0) + 315.0*cos(theta));
		c.imag = 0.0;
	}
	else if (index == 91)
	{
		c.real = (-3.0 / 256.0)*sqrt(95.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(2431.0 * pow(cos(theta), 8.0) - 4004.0*pow(cos(theta), 6.0) + 2002.0 * pow(cos(theta), 4.0) - 308.0*pow(cos(theta), 2.0) + 7.0)*cos(1.0*phi);
		c.imag = (-3.0 / 256.0)*sqrt(95.0 / (2.0*M_PI))*pow(sin(theta), 1.0)*(2431.0 * pow(cos(theta), 8.0) - 4004.0*pow(cos(theta), 6.0) + 2002.0 * pow(cos(theta), 4.0) - 308.0*pow(cos(theta), 2.0) + 7.0)*sin(1.0*phi);
	}
	else if (index == 92)
	{
		c.real = (3.0 / 128.0)*sqrt(1045.0 / (M_PI))*pow(sin(theta), 2.0)*(221.0 * pow(cos(theta), 7.0) - 273.0*pow(cos(theta), 5.0) + 91.0 * pow(cos(theta), 3.0) - 7.0*cos(theta))*cos(2.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(1045.0 / (M_PI))*pow(sin(theta), 2.0)*(221.0 * pow(cos(theta), 7.0) - 273.0*pow(cos(theta), 5.0) + 91.0 * pow(cos(theta), 3.0) - 7.0*cos(theta))*sin(2.0*phi);
	}
	else if (index == 93)
	{
		c.real = (-1.0 / 256.0)*sqrt(21945.0 / (M_PI))*pow(sin(theta), 3.0)*(221.0 * pow(cos(theta), 6.0) - 195.0*pow(cos(theta), 4.0) + 39 * pow(cos(theta), 2.0) - 1.0)*cos(3.0*phi);
		c.imag = (-1.0 / 256.0)*sqrt(21945.0 / (M_PI))*pow(sin(theta), 3.0)*(221.0 * pow(cos(theta), 6.0) - 195.0*pow(cos(theta), 4.0) + 39 * pow(cos(theta), 2.0) - 1.0)*sin(3.0*phi);
	}
	else if (index == 94)
	{
		c.real = (3.0 / 128.0)*sqrt(95095.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(17.0 * pow(cos(theta), 5.0) - 10.0*pow(cos(theta), 3.0) + cos(theta))*cos(4.0*phi);
		c.imag = (3.0 / 128.0)*sqrt(95095.0 / (2.0*M_PI))*pow(sin(theta), 4.0)*(17.0 * pow(cos(theta), 5.0) - 10.0*pow(cos(theta), 3.0) + cos(theta))*sin(4.0*phi);
	}
	else if (index == 95)
	{
		c.real = (-3.0 / 256.0)*sqrt(2717.0 / (M_PI))*pow(sin(theta), 5.0)*(85.0 * pow(cos(theta), 4.0) - 30.0*pow(cos(theta), 2.0) + 1.0)*cos(5.0*phi);
		c.imag = (-3.0 / 256.0)*sqrt(2717.0 / (M_PI))*pow(sin(theta), 5.0)*(85.0 * pow(cos(theta), 4.0) - 30.0*pow(cos(theta), 2.0) + 1.0)*sin(5.0*phi);
	}
	else if (index == 96)
	{
		c.real = (1.0 / 128.0)*sqrt(40755.0 / (M_PI))*pow(sin(theta), 6.0)*(17.0 * pow(cos(theta), 3.0) - 3.0*cos(theta))*cos(6.0*phi);
		c.imag = (1.0 / 128.0)*sqrt(40755.0 / (M_PI))*pow(sin(theta), 6.0)*(17.0 * pow(cos(theta), 3.0) - 3.0*cos(theta))*sin(6.0*phi);
	}
	else if (index == 97)
	{
		c.real = (-3.0 / 512.0)*sqrt(13585.0 / (M_PI))*pow(sin(theta), 7.0)*(17.0*pow(cos(theta), 2.0) - 1.0)*cos(7.0*phi);
		c.imag = (-3.0 / 512.0)*sqrt(13585.0 / (M_PI))*pow(sin(theta), 7.0)*(17.0 * pow(cos(theta), 2.0) - 1.0)*sin(7.0*phi);
	}
	else if (index == 98)
	{
		c.real = (3.0 / 256.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(theta)*cos(8.0*phi);
		c.imag = (3.0 / 256.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 8.0)*cos(theta)*sin(8.0*phi);
	}
	else if (index == 99)
	{
		c.real = (-1.0 / 512.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 9.0)*cos(9.0*phi);
		c.imag = (-1.0 / 512.0)*sqrt(230945.0 / (2.0*M_PI))*pow(sin(theta), 9.0)*sin(9.0*phi);
	}
	
	//return c;
	
}