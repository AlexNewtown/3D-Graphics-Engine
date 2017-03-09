#pragma once

#include "math\Matrix.h"
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <lodepng.h>

class Image;
class Wavelet;


class Image
{
public:
	Image(int N, int M);
	~Image();
	Matrix* channel[3];
	Image* conv(Matrix* filter);
	Image* downsample(int rate);
	void exportImage(char* filePath);
};

class EnvironmentMap
{
public:
	EnvironmentMap(int N, int M);
	~EnvironmentMap();
	float basisX[3];
	float basisY[3];
	float basisZ[3];

	Image* map[5];
	void waveletTransform(int levels);
	void waveletEncode();
	void imageEncode(float thres);
	Wavelet* mapWavelet[5];
	//cv::Mat map[5];
};


class Wavelet
{
public:
	Wavelet(Image* im, int levels);
	~Wavelet();
	Image* highBand[3];
	Image* lowBand;
	Wavelet* next;

private:
	Matrix* filterHH;
	Matrix* filterGH;
	Matrix* filterHG;
	Matrix* filterGG;
	void createWaveletFilters();
	void waveletTreshold(float t);
};