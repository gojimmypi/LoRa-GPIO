// Similar to 32u4 w/ra02 LoRA see https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio
// Gate COM8 (to left as viewed from front)

#include "GateMessage.h"
#include "GateOpened.h"
#include "GateClosed.h"
#include <SPI.h>
#include <RH_RF95.h>

#include "GlobalDefine.h"

#define BUTTON_PRESS_DURATION 250 // in milliseconds

#define RF95_FREQ 433.0 // MHz frequenct. Reminder: must match RX's freq at Console\!

// see https ://github.com/gojimmypi/LoRa-GPIO/blob/master/doc/DIY0031-LoRa32u4%20pinout%20diagram.pdf
// the values here are the bluish-gray "Arduino Pin" numbers, second column out from board
#define LED 13 // Blinky on receipt
#define REMOTE_BP1 6           // aka PD7  aka 27 aka T0, labeled on board as 6
#define REMOTE_BP2 12          // aka PD6  aka 26 aka T1, labeled on board as 12

#define RADIO_PACKET_SIZE  20
char tx_buf[RADIO_PACKET_SIZE]; // our transmit message buffer

unsigned long timerRefresh = 0;   // the dynamic counter refresh; are we ready to display the countdown? (typically updated every second)
unsigned long SendUpdate_Timeout; // the dynamic counter refresh for actually sending the update
unsigned long Rx_SendUpdate_ACK_Timeout; // the dynamic counter refresh for resending if no ACK received
unsigned int SendUpdate_Repeat_Counter = 0;
bool isWaitingOnAck = false;

unsigned const long SEND_UPDATE_TIMEOUT_MILLISECONDS = 100000; // how many miliseconds between sendinng LoRa gate status? TODO, this should be more flexible: frequent during opening and closing.... infrequent when idle
unsigned const long RX_SEND_UPDATE_ACK_TIMEOUT_MILLISECONDS = 2000; // we expected to receive an ACK to our update sent, if not, resend it with this frequency
unsigned const int SEND_UPDATE_REPEAT_MAX = 3; // howc many additional messages will be sent if the first one is not acknowledged
//
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
int b; // we need a better mechanism for detecting debug / release
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
// reset our counter for how often we actually send messages; set to current millis() value
//*******************************************************************************************************************************************
void Reset_SendUpdate_Timeout() {
    Serial.println(">> Reset_SendUpdate_Timeout");

    SendUpdate_Timeout = millis(); 
    Rx_SendUpdate_ACK_Timeout = millis();
}

//*******************************************************************************************************************************************
// if no Ack to our last send was received, resend it ever [RX_SEND_UPDATE_ACK_TIMEOUT_MILLISECONDS] milliseconds (typically 2 - 5 seconds)
//*******************************************************************************************************************************************
bool ReadyTxRepeat() {
    bool res;
    if (isWaitingOnAck) {
        if (SendUpdate_Repeat_Counter > SEND_UPDATE_REPEAT_MAX) {
            Serial.println(">> ReadyTxRepeat reset");
            Reset_SendUpdate_Timeout();
            SendUpdate_Repeat_Counter = 0;
            res = false; // we're never ready when we've sent too many repeats
        }
        else {
            res = ((millis() - Rx_SendUpdate_ACK_Timeout) > RX_SEND_UPDATE_ACK_TIMEOUT_MILLISECONDS);
        }
    }
    else {
        res = false;
    }
    return res;
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
    if (isGateOpenedChange() || isGateClosedChange() || ReadyTxRepeat() ) {
        Serial.println("ReadyTxRefresh true");
        Reset_SendUpdate_Timeout();
        return true;
    }
    else {
        return ((millis() - SendUpdate_Timeout) > SEND_UPDATE_TIMEOUT_MILLISECONDS);
    }
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
    // do a memory btye compare for the first [length of TheMessage] bytes, return true if equal
    // TODO determine length of param and compare that!
    return  (memcmp(rx_buf, (char*)TheMessage, sizeof((char*)"Click1")) == 0);
}

//*******************************************************************************************************************************************
// Press the button on [thisGPIO] pin for [BUTTON_PRESS_DURATION] milliseconds - BLOCKING
//*******************************************************************************************************************************************
void PressButton(uint8_t thisGPIO) {
    digitalWrite(thisGPIO, HIGH); // setting this pin high will drive current to the base of transistor, 
                                  // emulating the button being pressed (collector to ground)
    delay(BUTTON_PRESS_DURATION); // we're typically waiting 250 ms here 
    LORA_MESSAGE_DEBUG_PRINTLN("Clicked Button 1!");
    digitalWrite(thisGPIO, LOW);  // release the button by turning off the transistor 
}

const char prefix[] = "M5 ";
//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void PrepMessageToSend(const char str[RADIO_PACKET_SIZE]) {
    memset(tx_buf, 0, RADIO_PACKET_SIZE);
    int tx_buf_free = RADIO_PACKET_SIZE - strlen(tx_buf);
    int tx_buf_need = strlen(prefix) + strlen(str) + 1;
    if (tx_buf_need <= tx_buf_free) {
        strcat(tx_buf, prefix);
        strcat(tx_buf, str);
    }
    else {
        strcat(tx_buf, "tx_buf size");
    }
}

//*******************************************************************************************************************************************
// SendUpdate; here we transmit the current gate status to home console on a periodic basis (see ReadyTxRefresh)
// note that ReadyTxRefresh may be ready sooner than polling period, if a gate state change is detected
//*******************************************************************************************************************************************
void SendUpdate() {
    if (ReadyTxRefresh()) {
        if (isWaitingOnAck) {
            SendUpdate_Repeat_Counter++;
            Serial.print("Sending REPEAT message! ");
            Serial.println(SendUpdate_Repeat_Counter);
        }
        else {
            Serial.println("not a repeat");
        }
        unsigned long TransmitStartTime = millis();
        LORA_DEBUG_PRINTLN("Sending to rf95 message!");
        rf95.setModeIdle();
        memset(rx_buf, 0, 20); // clear our receive buffer when sending



        if (isGateOpened()) {
            if (isGateClosed()) {
                // if the gate is both open and closed, we have a malfunction!
                PrepMessageToSend(GATE_MESSAGE_IS_ERROR);
            }
            else {
                // the gate is open, and known to not be closed; Valid Open State
                PrepMessageToSend(GATE_MESSAGE_IS_OPEN);
            }
        }
        else { // the gate is not open
            if (isGateClosed()) {
                PrepMessageToSend(GATE_MESSAGE_IS_CLOSED);
            }
            else {
                // the gate is not open, nor closed; either in motion or stuck?
                PrepMessageToSend(GATE_MESSAGE_IS_MOVING);
            }
        }

        // itoa(packetnum++, tx_buf + 13, 10); // put the packet number into the string
        LORA_DEBUG_PRINT("Sending "); LORA_DEBUG_PRINTLN(tx_buf);
        tx_buf[19] = 0;

        delay(10); // TODO - do we really need this delay?
        Serial.print("Len=");
        Serial.println(strlen(tx_buf));
        rf95.send((uint8_t *)tx_buf, strlen(tx_buf));

        LORA_DEBUG_PRINTLN("Waiting for packet to complete..."); delay(10);
        yield();
        if (rf95.waitPacketSent(1000)) {
            Serial.print("isWaitingOnAck ="); 
            Serial.println(isWaitingOnAck);
            if (SendUpdate_Repeat_Counter < SEND_UPDATE_REPEAT_MAX) {
                isWaitingOnAck = true;
            }
            else {
                SendUpdate_Repeat_Counter = 0;
                isWaitingOnAck = false;

            }
            LORA_DEBUG_PRINTLN("Packet send complete!");
            LORA_DEBUG_PRINT("rf95.waitPacketSent = "); LORA_DEBUG_PRINT(millis() - TransmitStartTime); LORA_DEBUG_PRINTLN("ms. ");
        }
        else
        {
            // gave up waiting for packet to complete
            LORA_DEBUG_PRINTLN("Packet FAILED to complete!"); delay(10);
        }
        memset(tx_buf,0, 20); // clear our transmit buffer after transmit is complete
        rf95.setModeRx(); // ready to receive
        if (!isWaitingOnAck) {
            Serial.print("no waiting on ack, reset");
            Reset_SendUpdate_Timeout(); // SendUpdate_Timeout = millis(); // reset our counter for how often we actually send messages
        }
        Serial.println();
        Serial.println();
    }
    else {
        yield(); // <puff, puff>
    }


}


void resetLoRa() {
    delay(100);

    // manual reset of LoRa
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
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
	Serial.println("Gate Device Startup!");
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

    resetLoRa();

	while (!rf95.init()) {
        LORA_DEBUG_PRINT("LoRa radio init failed");
        // TODO - what now? reboot?
        delay(10000);
        resetLoRa();
	}
	//Serial.println("LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
        LORA_DEBUG_PRINT("setFrequency failed");
        // TODO - what now? reboot?
		while (1);
	}
	delay(250);

    // rf95.setModemConfig(RH_RF95::Bw78Cr48Sf4096);
    LORA_DEBUG_PRINT("Set Freq to: "); LORA_DEBUG_PRINTLN(RF95_FREQ);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin 
	// then you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
	delay(250);

    Reset_SendUpdate_Timeout(); // for setup:  SendUpdate_Timeout = millis(); and Rx_SendUpdate_ACK_Timeout = Millis()
}

//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// LOOP - main application loop; periodically sent status and continuously listen for commands to push a button on the remote
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
        else if (ReceivedMessage("Click2")) {
            digitalWrite(REMOTE_BP2, HIGH);
            delay(BUTTON_PRESS_DURATION); // we're typically waiting 250 ms here BLOCKING
            LORA_MESSAGE_DEBUG_PRINTLN("Clicked Button 2!");
            digitalWrite(REMOTE_BP2, LOW);
        }
        else if (ReceivedMessage("ACK")) {
            isWaitingOnAck = false;
            LORA_MESSAGE_DEBUG_PRINTLN("isWaitingOnAck = false!");
        }
        else {
            LORA_MESSAGE_DEBUG_PRINT("Ignored message: ");
            LORA_MESSAGE_DEBUG_PRINTLN((char*)rx_buf);
        }
    }
    else
    {
        yield(); // <puff, puff />
    }
}

