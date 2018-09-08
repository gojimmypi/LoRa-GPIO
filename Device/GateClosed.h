// GateClosed.h

#ifndef _GATECLOSED_h
#define _GATECLOSED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GATE_CLOSED_SENSOR 22  // aka ADC1 aka 40 aka PF1, labeled on board as A4

bool isGateClosed();
bool isGateClosedChange();

#endif

