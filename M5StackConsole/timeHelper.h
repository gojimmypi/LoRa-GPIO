// timeHelper.h

#ifndef _TIMEHELPER_h
#define _TIMEHELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <sys/time.h>                   // struct timeval
#include <time.h>

void setupLocalTime();
void printLocalTime();

#endif

