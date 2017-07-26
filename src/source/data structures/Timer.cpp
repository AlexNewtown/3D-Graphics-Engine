#include "data structures\Timer.h"

Timer::Timer()
{

}
Timer::~Timer()
{

}

void Timer::start()
{
	startTime = clock();
}
bool Timer::isTimeOut(unsigned long t)
{
	return t < elapsedTime();
}
unsigned long Timer::elapsedTime()
{
	return ((unsigned long)clock() - startTime);
}