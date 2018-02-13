// LCD:320x240 Colorful TFT LCD, ILI9341

#include <M5Stack.h>
#include <M5LoRa.h>

#include "Clock.h"

#define LORA_CS_PIN   5
#define LORA_RST_PIN  26
#define LORA_IRQ_PIN  36

// Stock font and GFXFF reference handle
#define GFXFF 1
#define FF18 &FreeSans12pt7b

#define DEVICEID "M5"

Clock myClock;

void setup() {

	// see https://www.aliexpress.com/store/product/M5Stack-Official-Stock-Offer-LoRa-Module-for-ESP32-DIY-Development-Kit-Wireless-433MHz-Built-in-Antenna/3226069_32839736315.html
	// To aviod the problem that the screen can not display, 
	// GPIO5, as the NSS pin of the LoRa module, needs to be pulled up when the system is initialized.
	// M5Stack LorRa init
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	M5.begin(); 

	Serial.begin(115200);
	// PA_OUTPUT_RFO_PIN = 0
	// override the default CS, reset, and IRQ pins (optional)
	LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN); // set CS, reset, IRQ pin
	LoRa.setTxPower(17,1);
	Serial.println("LoRa Receiver");
	M5.Lcd.println("LoRa Receiver");

	// frequency in Hz (433E6, 866E6, 915E6)
	if (!LoRa.begin(433.772280E6)) {
		Serial.println("Starting LoRa failed!");
		M5.Lcd.println("Starting LoRa failed!");
		while (1);
	}

	// LoRa.setSyncWord(0x69);
	Serial.println("LoRa init succeeded.");
	M5.Lcd.println("LoRa init succeeded.");
	sleep(2);
	M5.Lcd.clearDisplay();
	M5.Lcd.setCursor(0, 0);
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

void checkPacketReceipt() {
	// try to parse packet
	M5.Lcd.setCursor(0, 160);
	char msg[20] = "";
	int ptr = 0;
	char thisDevice[20];
	int packetSize = LoRa.parsePacket();
	if (packetSize) {
		// read packet
		while (LoRa.available()) {
			char ch = (char)LoRa.read();
			if (ch > 0) {
				msg[ptr] = ch;
				ptr++;
			}
		}
		msg[ptr] = 0;

	}
	if (ptr > 0) {

		strncpy(thisDevice, msg, strlen(DEVICEID)) + '\0';
		// received a packet

		//M5.Lcd.setTextPadding(80);
		//M5.Lcd.setTextDatum(MC_DATUM);
		Serial.print("Received packet: \"");
		M5.Lcd.print("Received packet: \"");

		if (thisDevice == DEVICEID) { // TODO check crc as well

			Serial.println(msg);
		}
		else {
			Serial.println("ALERT");
			Serial.println(msg);
			M5.Lcd.print(msg);
		}
		// print RSSI of packet
		Serial.print("\" with RSSI ");
		Serial.println(LoRa.packetRssi());
		M5.Lcd.print("\" with RSSI ");
		M5.Lcd.println(LoRa.packetRssi());
	}
}

void loop() {
	myClock.refreshDisplay();

	buttonsProcess();

	checkPacketReceipt();

	M5.update();
}