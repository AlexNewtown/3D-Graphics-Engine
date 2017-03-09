#pragma once

class Complex
{
public:
	Complex();
	Complex(float real, float imag);
	Complex(float exponent);
	float real;
	float imag;
	float magnitude();
	float phase();
};