// 
// 
// 

#include "GateOpened.h"
#include "GlobalDefine.h"

int LastGateOpenedState = 0; // what was the last known gate state? We only update this one a second
unsigned long OpenedRefresh = 0; // counter to only detect Closed Gate sensor once every second
                                 //
                                 //*******************************************************************************************************************************************
                                 // determine if the GateClosed sensor has changed
                                 //*******************************************************************************************************************************************
bool isGateOpenedChange() {
    bool res = false;
    if ((millis() - OpenedRefresh) > 1000) { // we debounce by only reading exactly once, every 1000ms
        int thisGateState = 0;
        thisGateState = isGateOpened(); // just in case we are in the middle of a bounce, readd the sensor only once
        if (thisGateState != LastGateOpenedState) {
            LastGateOpenedState = thisGateState;
            res = true;
        }
        OpenedRefresh = millis();
    }
    return res;
}


//*******************************************************************************************************************************************
// read sensor and return status of [gate fully opened] sensor
//*******************************************************************************************************************************************
int isGateOpened() {
    int a = digitalRead(GATE_OPENED_SENSOR);
    SENSOR_DEBUG_PRINT("GateOpened Sensor = ");
    SENSOR_DEBUG_PRINTLN(a);
    return a;
}

