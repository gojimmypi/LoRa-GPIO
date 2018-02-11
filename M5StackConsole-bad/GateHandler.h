#pragma once
class GateHandler
{
public:
	GateHandler();
	virtual ~GateHandler();

	enum class gateStates {
		closed,
		opening,
		open,
		closing,
		pausedOnOpen,
		pausedOnClose,
		unknown
	};
	gateStates gateState;

};

