// Similar to 32u4 w/ra02 LoRA see https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio
// Gate COM8 (to left as viewed from front)

#include <SPI.h>
#include <RH_RF95.h>

#include "GlobalDefine.h"

#define BUTTON_PRESS_DURATION 250 // in milliseconds

#define RF95_FREQ 433.0 // MHz frequenct. Reminder: must match RX's freq at Console\!

// see https ://github.com/gojimmypi/LoRa-GPIO/blob/master/doc/DIY0031-LoRa32u4%20pinout%20diagram.pdf
// the values here are the bluish-gray "Arduino Pin" numbers, second column out from board
#define LED 13 // Blinky on receipt
#define GATE_OPENED_SENSOR 23  // aka ADC0 aka 41 aka PF0, labeled on board as A5
#define GATE_CLOSED_SENSOR 22  // aka ADC1 aka 40 aka PF1, labeled on board as A4
#define REMOTE_BP1 6           // aka PD7  aka 27 aka T0, labeled on board as 6
#define REMOTE_BP2 12          // aka PD6  aka 26 aka T1, labeled on board as 12


const int RADIO_PACKET_SIZE = 20;
char tx_buf[20]; // our transmit message buffer

unsigned long timerRefresh = 0;   // the dynamic counter refresh; are we ready to display the countdown? (typically updated every second)
unsigned long SendUpdate_Timeout; // the dynamic counter refresh for actually sending the update
unsigned const long SEND_UPDATE_TIMEOUT_MILLISECONDS = 10000; // how many miliseconds between sendinng LoRa gate status? TODO, this should be more flexible: frequent during opening and closing.... infrequent when idle

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define DEVICEID "M5"

/* for feather m0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
*/

/* for shield
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/


/* for ESP w/featherwing
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

/* Teensy 3.x w/wing
#define RFM95_RST     9   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     4    // "C"
*/


// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// LoRa receive buffer
uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t rx_len = sizeof(rx_buf);

int16_t packetnum = 0;  // packet counter, we increment per xmission
                        // Should be a message for us now   

// https://stackoverflow.com/questions/2290509/debug-vs-ndebug/2290616#2290616
#ifdef NDEBUG 
int b; // we need a better mechanism for detecting debud / release
#endif


//*******************************************************************************************************************************************
// Hello World, blink the LED for [duration] milliseconds  BLOCKING
//*******************************************************************************************************************************************
void blinkLED(int duration) {
	digitalWrite(RFM95_RST, HIGH);
	delay(duration);
	digitalWrite(RFM95_RST, LOW);
}

//*******************************************************************************************************************************************
// read sensor and return status of [gate fully opened] sensor
//*******************************************************************************************************************************************
int isGateOpened() {
	int a = digitalRead(GATE_OPENED_SENSOR);
    SENSOR_DEBUG_PRINTLN(a);
	return a;
}

//*******************************************************************************************************************************************
// read sensor and return status of [gate fully closed] sensor
//*******************************************************************************************************************************************
int isGateClosed() {
    int a = digitalRead(GATE_CLOSED_SENSOR);
    SENSOR_DEBUG_PRINTLN(a);
    return a;
}


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
bool ReadyTxRefresh() {
#ifdef TIMER_DEBUG
    if ((millis() - timerRefresh) > 1000) {
        // Serial.print("Ready in: ");
        Serial.print((int)((SEND_UPDATE_TIMEOUT_MILLISECONDS - (millis() - SendUpdate_Timeout)) / 1000));
        Serial.print(".");
        timerRefresh = millis();
    }
#endif
    return ((millis() - SendUpdate_Timeout) > SEND_UPDATE_TIMEOUT_MILLISECONDS);
}

//*******************************************************************************************************************************************
// returns true if a message has been received
// note we should return fairly quickly here, regardless of results, as receiver is interrupt driven
//*******************************************************************************************************************************************
bool isMessageReceived() {
    rf95.setModeRx(); // reminder if we are already in Rx mode, this does nothing
    return rf95.recv(rx_buf, &rx_len);
}

//*******************************************************************************************************************************************
// ReceivedMessage - return true if the message most recently received is [TheMessage]
//*******************************************************************************************************************************************
bool ReceivedMessage(const char * TheMessage) {
    // do a memory bye compare for the first [length of TheMessage] bytes, return true if equal
    return  (memcmp(rx_buf, (char*)TheMessage, sizeof((char*)"Click1")) == 0);
}

//*******************************************************************************************************************************************
// Press the button on [thisGPIO] pin for [BUTTON_PRESS_DURATION] milliseconds - BLOCKING
//*******************************************************************************************************************************************
void PressButton(uint8_t thisGPIO) {
    digitalWrite(thisGPIO, HIGH);
    delay(BUTTON_PRESS_DURATION); // we're typically waiting 250 ms here 
    LORA_MESSAGE_DEBUG_PRINTLN("Clicked Button 1!");
    digitalWrite(thisGPIO, LOW);
}


//*******************************************************************************************************************************************
// SendUpdate; here we transmit the current gate status to home console on a perioud basis (see ReadyTxRefresh)
//*******************************************************************************************************************************************
void SendUpdate() {
    if (ReadyTxRefresh()) {
        int TransmitStartTime = millis();
        LORA_DEBUG_PRINTLN("Sending to rf95 message!");
        rf95.setModeTx();
        if (isGateOpened()) {
            Serial.println("Open!");
            strncpy(tx_buf, DEVICEID" Open" + '\0', RADIO_PACKET_SIZE);
        }
        else {
            Serial.println("Close!");
            strncpy(tx_buf, DEVICEID" Closed" + '\0', RADIO_PACKET_SIZE);
        }

        itoa(packetnum++, tx_buf + 13, 10);
        LORA_DEBUG_PRINT("Sending "); LORA_DEBUG_PRINTLN(tx_buf);
        tx_buf[19] = 0;

        delay(10); // TODO - do we really need this delay?
        rf95.send((uint8_t *)tx_buf, 20);

        LORA_DEBUG_PRINTLN("Waiting for packet to complete..."); delay(10);

        if (rf95.waitPacketSent(1000)) {
            LORA_DEBUG_PRINTLN("Packet send complete!");
            LORA_DEBUG_PRINT("rf95.waitPacketSent = "); LORA_DEBUG_PRINT(millis() - TransmitStartTime); LORA_DEBUG_PRINTLN("ms. ");
        }
        else
        {
            // gave up waiting for packet to complete
            LORA_DEBUG_PRINTLN("Packet FAILED to complete!"); delay(10);
        }
        SendUpdate_Timeout = millis(); // reset our counter for how often we actually send messages
    }
    else {
        yield(); // <puff, puff>
    }


}


//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// SETUP - main application initialization
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
void setup()
{
	delay(250);

 #ifdef _DEBUG
	while (!Serial);
	delay(100);
	Serial.println("Gate TX Test!");
	blinkLED(10);
 #endif
    Serial.begin(9600);


	// LoRa32u4 GPIO init
	pinMode(LED, OUTPUT);
	pinMode(GATE_OPENED_SENSOR, INPUT);
    pinMode(GATE_CLOSED_SENSOR, INPUT);
    
    // remote button push GPIO are both outputs
    pinMode(REMOTE_BP1, OUTPUT);
    pinMode(REMOTE_BP2, OUTPUT);

    // both Remote lines BP1 and BP2 are set to low (off)
    digitalWrite(REMOTE_BP1, LOW);
    digitalWrite(REMOTE_BP2, LOW);

     // LoRa init 
    pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	// manual reset of LoRa
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	while (!rf95.init()) {
        LORA_DEBUG_PRINT("LoRa radio init failed");
        // TODO - what now? reboot?
        while (1);
	}
	//Serial.println("LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
        LORA_DEBUG_PRINT("setFrequency failed");
        // TODO - what now? reboot?
		while (1);
	}
	delay(250);

    LORA_DEBUG_PRINT("Set Freq to: "); LORA_DEBUG_PRINTLN(RF95_FREQ);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin 
	// then you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
	delay(250);

    SendUpdate_Timeout = millis();
}

//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// LOOP - main application loop; per periodically sent status and continuously listen for commands to push a button on the remote
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
void loop()
{
    // Periodically send a message; if it is not time to send a message, we'll immediately return
    SendUpdate();

    // continually see if we have a message
    if (isMessageReceived())
    {
        LORA_MESSAGE_DEBUG_PRINT("Got reply: ");
        LORA_MESSAGE_DEBUG_PRINTLN((char*)rx_buf);
        LORA_MESSAGE_DEBUG_PRINT("RSSI: ");
        LORA_MESSAGE_DEBUG_PRINTLN(rf95.lastRssi());
        if (ReceivedMessage("Click1")) {
            PressButton(REMOTE_BP2); // BLOCKING
        //if (memcmp(rx_buf, (char*)"Click1", sizeof((char*)"Click1")) == 0) {
            //digitalWrite(REMOTE_BP2, HIGH);
            //delay(BUTTON_PRESS_DURATION); // we're typically waiting 250 ms here BLOCKING
            //LORA_MESSAGE_DEBUG_PRINTLN("Clicked Button 1!");
            //digitalWrite(REMOTE_BP2, LOW);
        }
        else if ((char*)rx_buf == (char*)"Click2") {
            digitalWrite(REMOTE_BP2, HIGH);
            delay(BUTTON_PRESS_DURATION); // we're typically waiting 250 ms here BLOCKING
            LORA_MESSAGE_DEBUG_PRINTLN("Clicked Button 2!");
            digitalWrite(REMOTE_BP2, LOW);
        }
        else {
            LORA_MESSAGE_DEBUG_PRINT("Ignored message: ");
            LORA_MESSAGE_DEBUG_PRINTLN((char*)rx_buf);
        }
    }
    else
    {
        yield(); // <puff, puff>
    }
}

