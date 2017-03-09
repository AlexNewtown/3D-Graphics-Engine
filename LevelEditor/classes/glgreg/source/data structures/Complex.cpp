#include "data structures\Complex.h"
#include <math.h>

Complex::Complex()
{
	real = 0.0;
	imag = 0.0;
}

Complex::Complex(float reals, float imags)
{
	this->real = reals;
	this->imag = imags;
	
}

float Complex::magnitude()
{
	return sqrt(this->real*this->real + this->imag*this->imag);
}

float Complex::phase()
{
	if (real == 0.0)
	{
		if (imag < 0)
			return -3.14159265359 / 2.0;
		else if (imag > 0)
			return 3.14159265359 / 2.0;
		else
			return 0.0;
	}
	return tan(imag / real);
}

Complex::Complex(float exponent)
{
	real = cos(exponent);
	imag = sin(exponent);
}