// GateState.h

#ifndef _GATESTATE_h
#define _GATESTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma once
class GateState
{
public:
	const String DEFAULT_LANGUAGE = "EN";
	GateState();

	void refresh();

	String StateMessage(String language);
	String StateMessage();

	typedef enum
	{
		UNKNOWN = 0,
		OPENING,
		STOPPED_WHILE_OPENING,
		REVERSED_WHILE_OPENING,
		OPEN,
		CLOSING,
		STOPPED_WHILE_CLOSING,
		REVERSED_WHILE_CLOSING,
		CLOSED,
	} GateStateChoice;

	GateStateChoice getCurrentState();
	void setCurrentState(GateStateChoice newGateState);

	GateStateChoice buttonPress(); // tell the state engine that a button press has occured and return the expected new state 

	~GateState();

private:
	uint32_t targetTime = 0;  

	GateStateChoice _currentGateState;

};




#endif

