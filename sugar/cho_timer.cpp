#include "cho_timer.h"
#include <windows.h>

void							cho::STimer::Reset											()								{
	QueryPerformanceFrequency( ( ::LARGE_INTEGER* )&CountsPerSecond );
	SecondsPerCount					= (1.0 / (CountsPerSecond));
	MicrosecondsPerCount			= (1.0 / (CountsPerSecond / 1000000.0));
	QueryPerformanceCounter( ( ::LARGE_INTEGER* )&PrevTimeStamp );		
	LastTimeSeconds					= 0;
	LastTimeMicroseconds			= 0;
}

void							cho::STimer::Frame											()								{
	QueryPerformanceCounter( ( ::LARGE_INTEGER* ) &CurrentTimeStamp );
	LastTimeSeconds					=  (float)	(( CurrentTimeStamp - PrevTimeStamp ) * SecondsPerCount);
	LastTimeMicroseconds			=  uint64_t	(( CurrentTimeStamp - PrevTimeStamp ) / (CountsPerSecond / 1000000.0));
	PrevTimeStamp					= CurrentTimeStamp;
}
