// 32u4 w/ra02 LoRA see https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio
// Gate COM8 (to left as viewed from front)

// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

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

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0


#define LED 13 // Blinky on receipt
#define GATE 23 // aka A5 aka 41 aka PF0 
#define GATE_OPEN_SENSOR 23  // aka A5  aka 41 aka PF0
#define GATE_CLOSE_SENSOR 22 // aka A4  aka 40 aka PF1
#define REMOTE_BP1 6         // aka PD7 aka 27 aka T0, labeled on board as 6
#define REMOTE_BP2 12        // aka PD6 aka 26 aka T1, labeled on board as 12

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// https://stackoverflow.com/questions/2290509/debug-vs-ndebug/2290616#2290616
#ifdef NDEBUG 
int b;
#endif
void blinkLED(int duration) {
	digitalWrite(RFM95_RST, HIGH);
	delay(duration);
	digitalWrite(RFM95_RST, LOW);
}

int isGateOpen() {
	int a = digitalRead(GATE);
	Serial.print(a);
	return a;
}

const int RADIO_PACKET_SIZE = 20;
char radiopacket[20];// = "Hello World #      ";;
unsigned long SendUpdate_Timeout;
unsigned const long SEND_UPDATE_TIMEOUT_MILLISECONDS = 10000;

bool ReadyTxRefresh() {
#ifdef TIMER_DEBUG
    if ((millis() - timerRefresh) > 1000) {
        // Serial.print("Ready in: ");
        Serial.print((int)((DATA_REFRESH_INTERVAL - (millis() - timeoutDashboardDataRefresh)) / 1000));
        Serial.print(".");
        timerRefresh = millis();
    }
#endif
    return ((millis() - SendUpdate_Timeout) > SEND_UPDATE_TIMEOUT_MILLISECONDS);
}


void setup()
{
	delay(250);


// #ifdef _DEBUG
	//while (!Serial);
	Serial.begin(9600);
	delay(100);
	Serial.println("Gate TX Test!");
	blinkLED(10);
// #endif


	// LoRa32u4 GPIO init
	pinMode(LED, OUTPUT);
	pinMode(GATE, INPUT);
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
		//Serial.println("LoRa radio init failed");
		while (1);
	}
	//Serial.println("LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		//Serial.println("setFrequency failed");
		while (1);
	}
	delay(250);

	//Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin 
	// then you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
	delay(250);

    SendUpdate_Timeout = millis();

}

int16_t packetnum = 0;  // packet counter, we increment per xmission
                        // Should be a message for us now   
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

bool isMessageReceived() {
    return rf95.recv(buf, &len);
}


void SendUpdate() {
    //Serial.println("Sending to rf95_server");
    // Send a message to rf95_server
    //                   12345678901234567890
    if (ReadyTxRefresh()) {
        rf95.setModeTx();
        if (isGateOpen()) {
            Serial.println("Open!");
            // digitalWrite(REMOTE_BP2, HIGH);
            strncpy(radiopacket, DEVICEID" Open" + '\0', RADIO_PACKET_SIZE);
        }
        else {
            Serial.println("Close!");
            // digitalWrite(REMOTE_BP2, LOW);
            strncpy(radiopacket, DEVICEID" Closed" + '\0', RADIO_PACKET_SIZE);
        }
        //char radiopacket[20] = "Hello World #      ";

        itoa(packetnum++, radiopacket + 13, 10);
        Serial.print("Sending "); Serial.println(radiopacket);
        radiopacket[19] = 0;

        Serial.print("Millis = ");
        Serial.println(millis());
        delay(10);
        rf95.send((uint8_t *)radiopacket, 20);

        Serial.println("Waiting for packet to complete..."); delay(10);

        if (rf95.waitPacketSent(1000)) {
            Serial.println("Packet send complete!"); delay(10);
            Serial.print("Millis = "); Serial.println(millis());
            Serial.println(millis());
        }
        else
        {
            // gave up waiting for packet to complete
            Serial.println("Packet FAILED to complete!"); delay(10);
        }
        SendUpdate_Timeout = millis();
    }
    else {
        yield();
    }


}

uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t rx_len = sizeof(rx_buf);
uint8_t * interim_len;
void loop()
{
    SendUpdate();

    // Now wait for a reply
    rf95.setModeRx();

    //Serial.println("Waiting for reply..."); delay(10);
    //if (rf95.recv(rx_buf, interim_len)) {
    //    Serial.println("Reply!...");
    //    if (rf95.waitAvailableTimeout(1000))
    //    {
            // Should be a reply message for us now   
            if (rf95.recv(rx_buf, &rx_len))
            {
                Serial.print("Got reply: ");
                Serial.println((char*)rx_buf);
                Serial.print("RSSI: ");
                Serial.println(rf95.lastRssi(), DEC);
            }
            else
            {
                yield();
            }
        //}
        //else
        //{
        //    Serial.println("No reply, is there a listener around?");
        //}
    //}
}

