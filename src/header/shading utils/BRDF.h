#ifndef BRDF_H
#define BRDF_H

#include "stdlib.h"
#include "math.h"

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)
#define M_PI	3.1415926535897932384626433832795

class Brdf
{
public:
	Brdf(const char* filePath);
	void lookup_brdf_val(double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val);

private:
	double* __brdf;
	int __size;

	bool read_brdf(const char* fileName);
	void cross_product (double* v1, double* v2, double* out);
	void normalize(double* v);
	void rotate_vector(double* vector, double* axis, double angle, double* out);

	void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
								double& theta_half,double& fi_half,double& theta_diff,double& fi_diff );

	int theta_half_index(double theta_half);
	int theta_diff_index(double theta_diff);
	int phi_diff_index(double phi_diff);

};
#endif