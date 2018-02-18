// LCD:320x240 Colorful TFT LCD, ILI9341

#include <RH_RF95.h>
#include <RadioHead.h>

#include <M5Stack.h>
// #include <M5LoRa.h> // due to naming (and probably hardware!) conflicts, we cannot use both

#include <SPI.h>
#include "Clock.h"

#define LORA_CS_PIN   5
#define LORA_RST_PIN  26
#define LORA_IRQ_PIN  36

// Stock font and GFXFF reference handle
#define GFXFF 1
#define FF18 &FreeSans12pt7b

#define DEVICEID "M5"

Clock myClock;

/* M5Stack LoRa pin defs from M5Lora.h (Sandeep Mistry library) */
//#define LORA_DEFAULT_SS_PIN    5
//#define LORA_DEFAULT_RESET_PIN 26
//#define LORA_DEFAULT_DIO0_PIN  36

// RadioHead defs for M5Stack (TODO are these correct?) 
// we assume the "wing" for the ESP32 feather is connected to *different* pins as compared to M5Stack
#define RFM95_CS 5   // 5// LORA_CS_PIN
#define RFM95_RST 26 // LORA_RST_PIN
#define RFM95_INT 36 // M5 LORA_IRQ_PIN

//  ESP32 feather w/wing  see https://github.com/adafruit/RadioHead/blob/master/examples/feather/Feather9x_TX/Feather9x_TX.ino#L56
// #define RFM95_RST     27   // "A"
// #define RFM95_CS      33   // "B"
// #define RFM95_INT     12   //  next to A


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433  // RadioHead in MHz, M5 in Hz (e.g. 433E6)


// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
	// see https://www.aliexpress.com/store/product/M5Stack-Official-Stock-Offer-LoRa-Module-for-ESP32-DIY-Development-Kit-Wireless-433MHz-Built-in-Antenna/3226069_32839736315.html
	// To aviod the problem that the screen can not display, 
	// GPIO5, as the NSS pin of the LoRa module, needs to be pulled up when the system is initialized.
	// M5Stack LorRa init
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	M5.begin();

	Serial.begin(115200); // Serial.begin after M5.begin
	Serial.println("Startup!");

	Serial.println("Pins at setup start:");
	Serial.print("SS:        "); Serial.println(SS);
	Serial.print("MOSI:      "); Serial.println(MOSI);
	Serial.print("MISO:      "); Serial.println(MISO);
	Serial.print("SCK:       "); Serial.println(SCK);
	Serial.print("A0:        "); Serial.println(A0);
	Serial.print("RFM95_CS:  "); Serial.println(RFM95_CS);
	Serial.print("RFM95_RST: "); Serial.println(RFM95_RST);
	Serial.print("RFM95_INT: "); Serial.println(RFM95_INT);
	Serial.print("RF95_FREQ: "); Serial.println(RF95_FREQ);




	pinMode(RFM95_RST, OUTPUT);
	//digitalWrite(RFM95_RST, HIGH);

	Serial.println("M5Stack LoRa RX/TX Test!");

	// manual reset
	digitalWrite(RFM95_RST, LOW);
	delay(10);
	digitalWrite(RFM95_RST, HIGH);
	delay(10);

	// RadioHead
	while (!rf95.init()) {
		Serial.println("RadioHead LoRa radio init failed");
		while (1);
	}
	Serial.println("RadioHead LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		Serial.println("RadioHead setFrequency failed");
		while (1);
	}
	Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

	//if (!rf95.setModemConfig(Bw125Cr48Sf4096)) { // Bw125Cr48Sf4096
	//	Serial.println("RadioHead setModemConfig failed");
	//	while (1);
	//}
	// End Radio head
	// rf95.setModeRx(); // do we need to set Rx mode? Sample code does not: https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio




	// PA_OUTPUT_RFO_PIN = 0
	// override the default CS, reset, and IRQ pins (optional)
	////LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN); // set CS, reset, IRQ pin
	////LoRa.setTxPower(17,1);
	Serial.println("LoRa Receiver");
	M5.Lcd.println("LoRa Receiver");

	// frequency in Hz (433E6, 866E6, 915E6)
	//// This is Sandeep Mistry library code
	////if (!LoRa.begin(433.772280E6)) {
	////	Serial.println("Starting LoRa failed!");
	////	M5.Lcd.println("Starting LoRa failed!");
	////	while (1);
	////}

	// The default transmitter power is 13dBm, using PA_BOOST.
	// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
	// you can set transmitter powers from 5 to 23 dBm:
	// rf95.setTxPower(13, false);

	// LoRa.setSyncWord(0x69);
	Serial.println("LoRa init succeeded.");
	M5.Lcd.println("LoRa init succeeded.");
	sleep(2);
	M5.Lcd.clearDisplay();
	M5.Lcd.setCursor(0, 0);

	Serial.print("SS:  "); Serial.println(SS);
	Serial.print("MOSI:"); Serial.println(MOSI);
	Serial.print("MISO:"); Serial.println(MISO);
	Serial.print("SCK: "); Serial.println(SCK);
	Serial.print("A0:  "); Serial.println(A0);
	Serial.println("Setup Complete. Waiting for packets...");
}

void operationMessage(char * msg) {
	int xpos = 0;
	int ypos = 85; // Top left corner of Operation text, about half way down = 85

	M5.Lcd.setFreeFont(FF18);                 // Select the font
	M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour yellow with black background
	M5.Lcd.drawString(msg, xpos, ypos, GFXFF);// Print the string name of the font

	Serial.printf(msg);
}


void buttonsProcess() {
	if (M5.BtnA.wasPressed()) {
		operationMessage("Button A");
	}

	if (M5.BtnB.wasPressed()) {
		M5.Lcd.clearDisplay();
		operationMessage("Refresh");
	}

	if (M5.BtnC.wasPressed()) {
		operationMessage("Button C");
	}
}

// Should be a message for us now   
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
bool isMessageReceived() {
	return rf95.recv(buf, &len);
}

int notCount = 0;
void checkPacketReceipt() {
	int x = 0;
	// try to parse packet
	M5.Lcd.setCursor(0, 160);
	char msg[20] = "";
	int ptr = 0;
	char thisDevice[20];

	//// This is Sandeep Mistry library code
	////int packetSize = LoRa.parsePacket();
	////if (packetSize) {
	////	// read packet
	////	while (LoRa.available()) {
	////		char ch = (char)LoRa.read();
	////		if (ch > 0) {
	////			msg[ptr] = ch;
	////			ptr++;
	////		}
	////	}
	////	msg[ptr] = 0;

	////}


	YIELD;
	//Serial.println(x);
	if (rf95.available())
	{
		x = 0;
		Serial.print("Available! ");
		if (isMessageReceived())
		{
			//digitalWrite(LED, HIGH);
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
			//digitalWrite(LED, LOW);
		}
		else
		{
			Serial.println("Receive failed");
		}
	}
	else
	{
		//notCount++;
		//Serial.print(notCount);
		//Serial.println(" Not Available! ");
	}




	//// working propr Sandeep/M5LoRa library code
	////if (ptr > 0) {
	////	strncpy(thisDevice, msg, strlen(DEVICEID)) + '\0';
	////	// received a packet

	////	//M5.Lcd.setTextPadding(80);
	////	//M5.Lcd.setTextDatum(MC_DATUM);
	////	Serial.print("Received packet: \"");
	////	M5.Lcd.print("Received packet: \"");

	////	if (thisDevice == DEVICEID) { // TODO check crc as well

	////		Serial.println(msg);
	////	}
	////	else {
	////		Serial.println("ALERT");
	////		Serial.println(msg);
	////		M5.Lcd.print(msg);
	////	}
	////	// print RSSI of packet
	////	Serial.print("\" with RSSI ");
	////	////Serial.println(LoRa.packetRssi());
	////	M5.Lcd.print("\" with RSSI ");
	////	//M5.Lcd.println(LoRa.packetRssi());
	////}
}

void loop() {
	myClock.refreshDisplay();

	buttonsProcess();

	checkPacketReceipt();

	M5.update();
}