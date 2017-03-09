#pragma once

#include <time.h>

class Timer
{
public:
	Timer();
	~Timer();
	void start();
	bool isTimeOut(unsigned long t);
	unsigned long elapsedTime();
private:
	unsigned long startTime;


};