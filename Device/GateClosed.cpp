// 
// 
// 

#include "GateClosed.h"
#include "GlobalDefine.h"

int LastGateClosedState = 0; // what was the last known gate state? We only update this one a second
unsigned long CloseRefresh = 0; // counter to only detect Closed Gate sensor once every second
//

//*******************************************************************************************************************************************
// read sensor and return status of [gate fully closed] sensor
//*******************************************************************************************************************************************
bool isGateClosed() {
    int thisClosedState = digitalRead(GATE_CLOSED_SENSOR);
    SENSOR_DEBUG_PRINT("GateClosed Sensor = ");
    SENSOR_DEBUG_PRINTLN(thisClosedState);
    return (thisClosedState == 0); // when fully closed, the sensor should short to ground, giving us a zeroS
}

//*******************************************************************************************************************************************
// determine if the GateClosed sensor has changed
//*******************************************************************************************************************************************
bool isGateClosedChange() {
    bool res = false;
    if ((millis() - CloseRefresh) > 1000) { // we debounce by only reading exactly once, every 1000ms
        int thisGateState = 0;
        thisGateState = isGateClosed(); // just in case we are in the middle of a bounce, read the sensor only once
        if (thisGateState != LastGateClosedState) {
            LastGateClosedState = thisGateState;
            res = true;
        }
        CloseRefresh = millis();
    }
    return res;
}