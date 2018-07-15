// This "Console" is based on 32u4 board, no display at this time

// Console COM6 (right; as viewed from front)

// get the time https://github.com/espressif/esp-idf/blob/master/examples/protocols/sntp/main/sntp_example_main.c
// WiFi AP & web server (show status)

// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

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
// ETSI has defined 433 to 434 MHz frequency band for LoRa application. It uses 433.175 MHz, 433.375 MHz and 433.575 MHz frequency channels.
// see http://www.rfwireless-world.com/Tutorials/LoRa-frequency-bands.html
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13
#define GATE 23 // aka A5 aka 41 aka PF0 

int isGateOpen() {
	int a = digitalRead(GATE);
	Serial.print(a);
	return a;
}

void setup()
{
	delay(250);
	pinMode(GATE, INPUT);

	pinMode(LED, OUTPUT);
	pinMode(RFM95_RST, OUTPUT);
	digitalWrite(RFM95_RST, HIGH);

	while (!Serial);
	Serial.begin(9600);
	delay(100);

	Serial.println("Console LoRa RX Test! V0.001");

	// manual reset
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	while (!rf95.init()) {
		Serial.println("LoRa radio init failed");
		while (1);
	}
	delay(250);
	Serial.println("LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		Serial.println("setFrequency failed");
		while (1);
	}
	delay(250);
	Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

	// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
	// you can set transmitter powers from 5 to 23 dBm:
	rf95.setTxPower(23, false);
	delay(250);
}
int x = 0;


// Should be a message for us now   
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

bool isMessageReceived() {
	return rf95.recv(buf, &len);
}


void loop()
{
	x++;
	//Serial.print(isGateOpen());
	//Serial.print(':');
	//if (isGateOpen) {
	//	Serial.println("Gate is open");
	//}
	//else {
	//	Serial.println("Gate is closed");
	//}
	YIELD;
	//Serial.println(x);
	if (rf95.available())
	{
		x = 0;

		if (isMessageReceived())
		{
			digitalWrite(LED, HIGH);
			RH_RF95::printBuffer("Received: ", buf, len);
			Serial.print("Got: ");
			Serial.println((char*)buf);
			Serial.print("RSSI: ");
			Serial.println(rf95.lastRssi(), DEC);
			delay(10);
			// Send a reply
			delay(200); // TODO may or may not be needed
			uint8_t data[] = "And hello back to you";
			rf95.send(data, sizeof(data));
			if (rf95.waitPacketSent()) {
				Serial.println("Sent a reply");
			}
			else {
				Serial.println("Reply failed!");
			}
			digitalWrite(LED, LOW);
		}
		else
		{
			Serial.println("Receive failed");
		}
	}
}
