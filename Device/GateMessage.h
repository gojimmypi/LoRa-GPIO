// GateMessage.h

#ifndef _GATEMESSAGE_h
#define _GATEMESSAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define GATE_LANGUAGE_EN

#if defined(GATE_LANGUAGE_EN)
#define GATE_MESSAGE_IS_OPEN     "Open"
#define GATE_MESSAGE_IS_CLOSED   "Closed"
#define GATE_MESSAGE_IS_MOVING   "Moving"
#define GATE_MESSAGE_IS_OPENING  "Opening"
#define GATE_MESSAGE_IS_CLOSING  "Closing"
#define GATE_MESSAGE_IS_ERROR    "Error"

#elif defined(GATE_LANGUAGE_FR)
#define GATE_MESSAGE_IS_OPEN     "Overt"
#define GATE_MESSAGE_IS_CLOSED   "Ferme"
#define GATE_MESSAGE_IS_MOVING   "En marche"
#define GATE_MESSAGE_IS_OPENING  "S'ouvre"
#define GATE_MESSAGE_IS_CLOSING  "Se ferme"
#define GATE_MESSAGE_IS_ERROR    "Erreur"

#else
#pragma message(Reminder "Error: GATE_LANGUAGE_xx not defined in GateMessage.h")
#endif


#endif

