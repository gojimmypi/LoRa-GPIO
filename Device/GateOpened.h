// GateOpened.h
// There's a sensor to detect an open date. This is the code that supports it


#ifndef _GATEOPENED_h
#define _GATEOPENED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GATE_OPENED_SENSOR 23  // aka ADC0 aka 41 aka PF0, labeled on board as A5

bool isGateOpenedChange();
bool isGateOpened();

#endif

