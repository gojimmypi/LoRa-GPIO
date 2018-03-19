// 
// 
// 

#include "GateState.h"

GateState::GateState()
{
	_currentGateState = UNKNOWN;
}

GateState::GateStateChoice GateState::buttonPress()
{
	if (_currentGateState == UNKNOWN) {
		// we still don't know
	}
	else if (_currentGateState == OPENING)
	{
		_currentGateState = STOPPED_WHILE_OPENING;
	}
	else if (_currentGateState == STOPPED_WHILE_OPENING) // gate is stopped not fully open or closed
	{
		_currentGateState = REVERSED_WHILE_OPENING; // aka closing, but different amount of time
	}
	else if (_currentGateState == REVERSED_WHILE_OPENING) // aka closing
	{
		_currentGateState = REVERSED_WHILE_CLOSING; //  aka opening
	}
	else if (_currentGateState == OPEN)
	{
		_currentGateState = CLOSING;
	}
	else if (_currentGateState == CLOSING)
	{
		_currentGateState = STOPPED_WHILE_CLOSING; // gate is stopped not fully open or closed
	}
	else if (_currentGateState == STOPPED_WHILE_CLOSING)  
	{
		_currentGateState = REVERSED_WHILE_CLOSING; // aka opening
	}
	else if (_currentGateState == REVERSED_WHILE_CLOSING) // aka opening
	{
		_currentGateState = REVERSED_WHILE_CLOSING; // aka opening
	}
	else if (_currentGateState == CLOSED)
	{
		_currentGateState = OPENING;
	}
	else 
	{
		_currentGateState == UNKNOWN;
	}
	return _currentGateState;
}

String GateState::StateMessage() {
	return StateMessage(DEFAULT_LANGUAGE);
}

String GateState::StateMessage(String language) {
	String msg = "unknown";
	if (_currentGateState == UNKNOWN) {
		msg = "unknown";
	}
	else if (_currentGateState == OPENING)
	{
		msg = "Opening";
	}
	else if (_currentGateState == STOPPED_WHILE_OPENING) // gate is stopped not fully open or closed
	{
		msg = "Stopped";
	}
	else if (_currentGateState == REVERSED_WHILE_OPENING) // aka closing
	{
		msg = "Closing";
	}
	else if (_currentGateState == OPEN)
	{
		msg = "Open";
	}
	else if (_currentGateState == CLOSING)
	{
		msg = "Closing";
	}
	else if (_currentGateState == STOPPED_WHILE_CLOSING)
	{
		msg = "Stopped";
	}
	else if (_currentGateState == REVERSED_WHILE_CLOSING) // aka opening
	{
		msg = "Opening";
	}
	else if (_currentGateState == CLOSED)
	{
		msg = "Closed";
	}
	else
	{
		msg = "unknown";
	}
	return msg;
}


GateState::GateStateChoice GateState::getCurrentState() {
	return _currentGateState;
}

void GateState::setCurrentState(GateStateChoice newGateState) {
	_currentGateState = newGateState;
}

void GateState::refresh() {
}


GateState::~GateState()
{
}
