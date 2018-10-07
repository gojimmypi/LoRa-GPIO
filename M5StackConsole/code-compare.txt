// LCD:320x240 Colorful TFT LCD, ILI9341
// Working RadioHead sample

// see RH_RF95.h (line 479)
// Caution: the performance of this radio, especially with narrow bandwidths is strongly dependent on the
// accuracy and stability of the chip clock. HopeRF and Semtech do not appear to 
// recommend bandwidths of less than 62.5 kHz 
// unless you have the optional Temperature Compensated Crystal Oscillator (TCXO) installed and 
// enabled on your radio module. See the refernece manual for more data.
// Also https://lowpowerlab.com/forum/rf-range-antennas-rfm69-library/lora-library-experiences-range/15/
// and https://www.semtech.com/uploads/documents/an120014-xo-guidance-lora-modulation.pdf
// TODO - investigate TCXO install / enable mode


#include "GateState.h"
#include <M5Stack.h>
// #include <M5LoRa.h> // due to naming (and probably hardware!) conflicts, we cannot use both M5LoRa and RadioHead concurrently
#include <SPI.h>
#include <RH_RF95.h> // #include <RadioHead.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <Preferences.h>

#include "GlobalDefine.h"
#include "time.h"
#include "Clock.h"

#include "WiFiHelper.h"

#ifndef MyConfig_PragmaRegion  // fake pragma region (supported in Visual Studio, but not gcc)

// Change to 434.0 or other frequency, must match RX's freq! RadioHead in MHz (e.g. 433); M5 in Hz (e.g. 433E6)
#define RF95_FREQ 433
#define DEVICEID "M5"

#ifndef NDEBUG
#define MAIN_SERIAL=1
#endif // !NDEBUG

#define MyConfig_PragmaRegion
#endif 

#ifndef PINDEFS_PragmaRegion  // fake pragma region (supported in Visual Studio, but not gcc)
// -------------------------------------------------------------------------------------------------------------------------------------
// M5Stack LoRa pin defs from M5Lora.h (Sandeep Mistry library)
// these INCORRECT defs can be found in LoRaReceiver example code for M5Stack in config.h
// -------------------------------------------------------------------------------------------------------------------------------------
//#define LORA_CS_PIN   5
//#define LORA_RST_PIN  26
//#define LORA_IRQ_PIN  36 // not actually used; registers are polled, not interrupt driven in this release of Sandeep

// -------------------------------------------------------------------------------------------------------------------------------------
// Actual RadioHead defs for M5Stack - NOTE some documentation incorrectly reversed the labels for RST & IRQ
// -------------------------------------------------------------------------------------------------------------------------------------
#define RFM95_CS 5   // LORA_CS_PIN
#define RFM95_RST 36 // LORA_RST_PIN is 36, TODO but it is read-only! so we'll need to short to another pin
#define RFM95_INT 26 // M5 LORA_IRQ_PIN 36 (jumper to 16)
//#define LORA_DEFAULT_DIO0_PIN  26 
#define TEST_GPIO (39) // 39 = button A; Lora Int = 36.


// -------------------------------------------------------------------------------------------------------------------------------------
//  ESP32 feather w/wing  see https://github.com/adafruit/RadioHead/blob/master/examples/feather/Feather9x_TX/Feather9x_TX.ino#L56
// -------------------------------------------------------------------------------------------------------------------------------------
// #define RFM95_RST     27   // "A"
// #define RFM95_CS      33   // "B"
// #define RFM95_INT     12   //  next to A
#define PINDEFS_PragmaRegion
#endif 




// Stock font and GFXFF reference handle
#define GFXFF 1
#define FF18 &FreeSans12pt7b

#define RADIO_PACKET_SIZE  20
#define SEND_MESSAGE_IS_REFRESH "Refresh"
Clock myClock;
struct tm timeinfo;

char tx_buf[RADIO_PACKET_SIZE]; // our transmit message buffer


// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// wifi config store
Preferences preferences;

GateState myGate;


// TODO - determine if RAM attributes are really needed for IRQs on ESP32
//static DRAM_ATTR   
int thisIndex = 0;
int lastIndex = 0;

//static void myISR(void *args) {
// IRAM_ATTR 
void myISR() {
	thisIndex++;
}

 
// int DST_Offset()
//{
//     int StandardTimeOffset = 3600;
//     int res = 0; // DST_value = 0
//     if (!getLocalTime(&timeinfo)) {
//         Serial.println("Failed to obtain time");
//         return 0;
//     }
//
//     int previousSunday = timeinfo.tm_mday - timeinfo.tm_wday;
//     Serial.print(" timeinfo.tm_mday = ");
//     Serial.println(timeinfo.tm_mday);
//     Serial.print(" timeinfo.tm_wday = ");
//     Serial.println(timeinfo.tm_wday);
//     switch (timeinfo.tm_mon)
//     {
//     case 1: // January
//     case 2: // February
//         res = StandardTimeOffset;
//     case 3: // March is when the time changes
//         if (previousSunday >= 8) {
//             res = 0; // DST
//         }
//         else {
//             res = StandardTimeOffset;
//         }
//     case 4:
//     case 5:
//     case 6:
//     case 7:
//     case 8:
//     case 9:
//     case 10:
//         res = 0;
//     case 11:
//         if (previousSunday <= 0) {
//             // DST
//         }
//         else {
//             res = StandardTimeOffset;
//         }
//     case 12:
//         res = StandardTimeOffset;
//     default:
//         break;
//     }
//     Serial.print(" res = ");
//     Serial.println(res);
//     return res;
//}

//void GetNetworkTime() {
//    Serial.printf("Connecting to %s ", ssid);
//    WiFi.begin(ssid, password);
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//    }
//    if (WiFi.isConnected()) {
//        Serial.println(" CONNECTED");
//        //init and get the time
//        Serial.println(" Getting the time via configTime() ...");
//        configTime(gmtOffset_sec, DST_Offset(), ntpServer, ntpServer2, ntpServer3);
//        myClock.useNetworkTimeConfig = getLocalTime(&timeinfo); // if getLocalTime() is successfuly, we'll use network time
// 
//        printLocalTime();
//    }
//    //disconnect WiFi as it's no longer needed
//    WiFi.disconnect(true);
//    WiFi.mode(WIFI_OFF);
//}

void initialSetup() {
    // stuff that must be run first at power-up time
    delay(100);
                        
                         // see https://www.aliexpress.com/store/product/M5Stack-Official-Stock-Offer-LoRa-Module-for-ESP32-DIY-Development-Kit-Wireless-433MHz-Built-in-Antenna/3226069_32839736315.html
    // To avoid the problem that the screen can not display, 
    // GPIO5, as the NSS pin of the LoRa module, needs to be pulled up when the system is initialized.
    // M5Stack LorRa init
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    M5.begin(false); // init the M5 library; note we customized this to not have LoRa conflict with speaker
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Startup...");
    preferences.end();
    while (!Serial); //TODO 
    Serial.begin(19200); // Serial.begin after M5.begin 19200 or 57600 for debugging with Visual Micro; see http://www.visualmicro.com/forums/YaBB.pl?num=1365950696
    Serial.println("Startup...");
}

void setup() {
    initialSetup();

    wifiConnect(20);
    myClock.init(); // we delared a global variable that setup the network before WiFi was available, so we call init again here when WiFi is active to ensure time is set

	WiFi.persistent(false); // Setting persistent to false will get SSID / password written to flash only if currently used values do not match what is already stored in flash.
	                        // see http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html#persistent

	// WiFi.forceSleepBegin(); // what happened to this feature? no longer available?
	WiFi.mode(WIFI_OFF); // if we're not using WiFi, turn it off

    // GetNetworkTime(); 

#ifdef ESP32_JS_WOW
	Serial.println("ESP32_JS_WOW");
#endif

	//int _interruptPin = TEST_GPIO;
	//int interruptNumber = digitalPinToInterrupt(_interruptPin);
	//pinMode(_interruptPin, INPUT);

	//int errInt = gpio_intr_enable((gpio_num_t)interruptNumber);
	//Serial.print("interruptNumber gpio_intr_enable=");
	//Serial.println(errInt);

	//attachInterrupt(interruptNumber, myISR, RISING);

	// pinMode(35, INPUT);

	//attachInterrupt(35, myISR, RISING);


	// TODO is this the preferred way to attach interrupts?
	//gpio_install_isr_service(0);
	//gpio_isr_handler_add((gpio_num_t)TEST_GPIO, myISR, NULL);


	// SS = 5; // RadioHead_h overwrites this TODO edit source def


	// setup LoRa reset line
	//pinMode(RFM95_RST, OUTPUT);
	//digitalWrite(RFM95_RST, HIGH);


	Serial.println("Startup! Version 1.01");

	Serial.println("Pins at setup start:");
	Serial.print("  SS:        "); Serial.println(SS);
	Serial.print("  MOSI:      "); Serial.println(MOSI);
	Serial.print("  MISO:      "); Serial.println(MISO);
	Serial.print("  SCK:       "); Serial.println(SCK);
	Serial.print("  A0:        "); Serial.println(A0); // typically 36
	Serial.print("  RFM95_CS:  "); Serial.println(RFM95_CS);
	Serial.print("  RFM95_RST: "); Serial.println(RFM95_RST);
	Serial.print("  RFM95_INT: "); Serial.println(RFM95_INT);
	Serial.print("  RF95_FREQ: "); Serial.println(RF95_FREQ);

    Serial.println("Reminder that we didn't actually reset LoRa!");
	// manual reset our LoRa module (note reset is on IO36 which is READ ONLY
	//digitalWrite(RFM95_RST, LOW);
	//delay(10);
	//digitalWrite(RFM95_RST, HIGH);
	//delay(10);

	// manual reverse reset; desperation attempt in case reversed logic (does not appear that way) TODO confirm with oscilloscope
	//digitalWrite(RFM95_RST, HIGH);
	//delay(10);
	//digitalWrite(RFM95_RST, LOW);
	//delay(10);


	//errInt = gpio_intr_enable((gpio_num_t) RFM95_INT);
	//Serial.print("RFM95_INT gpio_intr_enable=");
	//Serial.println(errInt);

	// RadioHead
	while (!rf95.init()) {
		Serial.println("RadioHead LoRa radio init failed!");
		while (1); // TODO - we don't really want to wait here forever
	}
    //Serial.print("  rf95.lastRssi: "); Serial.println(rf95.lastRssi());
    delay(1000);
	Serial.print("Using SlaveSelectPin=");
	Serial.println(rf95.getSlaveSelectPin());
	Serial.println("RadioHead LoRa radio init OK!");

	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
	if (!rf95.setFrequency(RF95_FREQ)) {
		Serial.println("RadioHead setFrequency failed");
		while (1); // TODO - we don't really want to wait here forever
	}
	Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);


    // rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
    
    // do we need to match modulation settings? (or just frequency?)
	//if (!rf95.setModemConfig(Bw125Cr48Sf4096)) { // Bw125Cr48Sf4096
	//	Serial.println("RadioHead setModemConfig failed");
	//	while (1);
	//}
	// End Radio head
	// rf95.setModeRx(); // do we need to set Rx mode? Sample code does not: https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio


	// PA_OUTPUT_RFO_PIN = 0
	// override the default CS, reset, and IRQ pins (optional)
	////LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN); // set CS, reset, IRQ pin

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    // rf95.setTxPower(13, false);
    rf95.setTxPower(23,false);
	Serial.println("LoRa Receiver");
	M5.Lcd.println("LoRa Receiver");
    // rf95.send((uint8_t *)"hello", 5);
	// frequency in Hz (433E6, 866E6, 915E6)
	//// This is Sandeep Mistry library code
	////if (!LoRa.begin(433.772280E6)) {
	////	Serial.println("Starting LoRa failed!");
	////	M5.Lcd.println("Starting LoRa failed!");
	////	while (1);
	////}



	// LoRa.setSyncWord(0x69);
	Serial.println("LoRa init succeeded.");
	M5.Lcd.println("LoRa init succeeded.");
	//sleep(2);

    M5.Lcd.clearDisplay();
	M5.Lcd.setCursor(0, 0);

	//Serial.print("SS:  "); Serial.println(SS);
	//Serial.print("MOSI:"); Serial.println(MOSI);
	//Serial.print("MISO:"); Serial.println(MISO);
	//Serial.print("SCK: "); Serial.println(SCK);
	//Serial.print("A0:  "); Serial.println(A0);
    char data[20] = "Polling!";
    data[8] = 0;
    // memset((char*)data, 0, sizeof(data));
	Serial.println("Calling send...");

    rf95.setModeIdle();
	rf95.send((uint8_t *)data, 20);
	Serial.println("waiting to send...");
	if (rf95.waitPacketSent(10000)) {
		Serial.println("Packet send complete!"); delay(10);
	}
	else
	{
		// gave up waiting for packet to complete
		Serial.println("Packet FAILED to complete!"); delay(10);
	}
	// rf95.waitPacketSent();
	// Serial.println("Send complete!");

	Serial.println("Setup Complete. Waiting for packets...");

	Serial.print("rf95 mode: ");
	Serial.println(rf95.mode());
}



void operationMessage(String msg) {
    // display is 320 x 240 
	int xpos = 0;
	int ypos = 85; // Top left corner of Operation text, about half way down = 85


	String str = (String)msg + " RSSI = " + rf95.lastRssi() ;

	M5.Lcd.setFreeFont(FF18);                 // Select the font
	M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour yellow with black background
    M5.Lcd.fillRect(0, 85, 319, 20, TFT_BLACK); // erase the enntire old line
	M5.Lcd.drawString(str, xpos, ypos, GFXFF);// Print the string name of the font

	Serial.println(str);
}

//void buttons_test() {
//	if (M5.BtnA.wasPressed()) {
//		M5.Lcd.printf("A");
//		Serial.printf("A");
//		myGate.buttonPress();
//		Serial.println(myGate.StateMessage());
//	}
//	if (M5.BtnB.wasPressed()) {
//		M5.Lcd.printf("B");
//		Serial.printf("B");
//	}
//	if (M5.BtnC.wasPressed()) {
//		M5.Lcd.printf("C");
//		Serial.printf("C");
//	}
//}

void SendACK() {
    char data[4] = "ACK";
    data[3] = 0;
    rf95.setModeIdle();
    Serial.print("Size =");  Serial.println(sizeof(data));
    rf95.send((uint8_t *)data, sizeof(data));
    //if (rf95.waitPacketSent(10000)) {
    //    Serial.println("ACK Packet send complete!"); delay(10);
    //    Serial.print("Millis = "); Serial.println(millis());
    //    Serial.println(millis());
    //}
    //else
    //{
    //    // gave up waiting for packet to complete
    //}
    rf95.setModeIdle();
}

const char prefix[] = "M5 ";
//*******************************************************************************************************************************************
//  PrepMessageToSend
//*******************************************************************************************************************************************
void PrepMessageToSend(const char str[RADIO_PACKET_SIZE]) {
    memset(tx_buf, 0, RADIO_PACKET_SIZE);  //TODO ptr?               // clear the transmit buffer
    int tx_buf_free = RADIO_PACKET_SIZE - strlen(tx_buf); // confirm we have free space
    int tx_buf_need = strlen(prefix) + strlen(str) + 1;   // determine how much space is needed
    if (tx_buf_need <= tx_buf_free) {
        strcat(tx_buf, prefix);                           // append the prefix to the transmit buffer
        strcat(tx_buf, str);                              // then append the string we want to send after the prefix
    }
    else {
        strcat(tx_buf, "tx_buf size");                    // if there's not enough space, put in error message
    }
}

void SendMessage(char data[RADIO_PACKET_SIZE]) {
    Serial.print("Request to send message: ");
    Serial.println(data);
    rf95.setModeIdle();
    PrepMessageToSend(data);
    Serial.print("Size =");  Serial.println(strlen(tx_buf));
    LORA_DEBUG_PRINT("Sending "); LORA_DEBUG_PRINTLN(tx_buf);
    tx_buf[RADIO_PACKET_SIZE - 1] = 0;
 
    rf95.send((uint8_t *)tx_buf, sizeof(tx_buf));

    //if (rf95.waitPacketSent(10000)) {
    //    Serial.println("Packet send complete!"); delay(10);
    //    Serial.print("Millis = "); Serial.println(millis());
    //    Serial.println(millis());
    //}
    //else
    //{
    //    // gave up waiting for packet to complete
    //}
    rf95.setModeIdle();
}

void buttonsProcess() {
	if (M5.BtnA.wasPressed()) {
		operationMessage(myGate.StateMessage());
		myGate.buttonPress();
		Serial.println(myGate.StateMessage());
	}

	if (M5.BtnB.wasPressed()) {
		M5.Lcd.clearDisplay();
		operationMessage((char*)SEND_MESSAGE_IS_REFRESH);
        SendMessage(SEND_MESSAGE_IS_REFRESH);
    }

    if (M5.BtnC.wasPressed()) {
        operationMessage((char*)"Button C");
        char data[20] = "Click1";
        data[19] = 0;
        //Serial.println("Set rf95.setModeTx");
        rf95.setModeIdle();
        Serial.println("sending data...");
        Serial.print(sizeof(data));
        Serial.println(" bytes.");
        rf95.send((uint8_t *)data, sizeof(data));
        Serial.println("waiting for send to complete...");
        if (rf95.waitPacketSent(3000)) { // time to wait in ms
            Serial.println("Packet send complete!"); delay(10);
            Serial.print("Millis = "); Serial.println(millis());
            Serial.println(millis());
        }
        else
        {
            Serial.println("Packet send gave up!");
            // gave up waiting for packet to complete
        }
        rf95.setModeIdle();    
    }
}



// Should be a message for us now   
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
bool isMessageReceived() {
    // This will return true if all (or part!) of a messages has been received; it will NOT know if message still in process of being received
	return rf95.recv(buf, &len);
}

int notCount = 0;
void checkPacketReceipt(int timeToWait) {
	rf95.setModeRx();
	delay(10);  // ms delay; Receiver Startup Time 250.0 kHz = 63us; 2.5kHz = 2.33 ms
                // TS_RE or later after setting the device in Receive mode, any incoming packet will be detected and demodulated by the transceiver.
	yield();

	// Serial.print("Waiting checkPacketReceipt...");
    if (rf95.waitAvailableTimeout(timeToWait)) // if (rf95.available()) // return true if a new, complete, error-free uncollected message is available to be retreived by recv()
	{
		Serial.println("Available! ");
		if (isMessageReceived())
		{
			Serial.print("Got: ");
			Serial.println((char*)buf);
			Serial.print("RSSI: ");
			Serial.println(rf95.lastRssi(), DEC);
			String str = (char*)buf;
			//str = str.substring(2, str.length() - 2);
			//Serial.print(" String length=");
			Serial.println(str);

			Serial.println(str.length());

            delay(10);
            SendACK();

			if (myGate.getCurrentState() ==  myGate.UNKNOWN) {
				if (str == "M5 Closed") {
					myGate.setCurrentState(myGate.CLOSED);
				}
				else if (str == "M5 Open") {
					myGate.setCurrentState(myGate.OPEN);
				}
				else {
					// other message
				}

			}
			operationMessage((char*)buf);
			// Send a reply
			//delay(200); // TODO may or may not be needed
			//uint8_t data[] = "And hello back to you";
			//rf95.send(data, sizeof(data));
			//if (rf95.waitPacketSent(2000)) {
			//	Serial.println("Sent a reply");
			//}
			//else {
			//	Serial.println("Reply failed!");
			//}
			//digitalWrite(LED, LOW);
		}
		else
		{
			Serial.println("Receive failed");
		}
	}
	else
	{
		// Serial.println(" No Data Available! ");
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
	// rf95.sleep();
	delay(10);
}

 
int lastCountInterrupt = 0;
int intThisEventCount = 0;
int intLastEventCount = 0;

void loop() {
	//if (thisIndex != lastIndex) {
	//	Serial.print(thisIndex);
	//	Serial.println(" Interrupt!");
	//	lastIndex = thisIndex;
	//}

    // this is a debug section that shows the count of interrupts.
    // if interrupts are firing, but we are not receiving a message: why?

	// buttons_test();

	// Serial.println(digitalRead(TEST_GPIO));
	myClock.refreshDisplay();

	buttonsProcess();

	checkPacketReceipt(200);

	delay(10);
	yield();

	M5.update();

	intThisEventCount = rf95.countInterrupt() + rf95.txGood() + rf95.rxBad() + rf95.rxInvalid();
	if (intThisEventCount != intLastEventCount) {
        Serial.print(":: interrupt: ");
        Serial.println(rf95.countInterrupt(), DEC);
 
		Serial.print(":: txGood: ");
		Serial.println(rf95.txGood(), DEC); // transmit counter

		Serial.print(":: rxBad: ");         // receive counter
		Serial.println(rf95.rxBad(), DEC);

		Serial.print(":: rxInvalid: ");     // receive counter
		Serial.println(rf95.rxInvalid(), DEC);

        Serial.print(":: rxGood: ");        // receive counter
        Serial.println(rf95.rxGood(), DEC);
        
        //Serial.print("0x");
		//Serial.print(rf95.readRegister((uint8_t)0x1d), HEX);
		//Serial.print(", 0x");
		//Serial.print(rf95.readRegister((uint8_t)0x1e), HEX);
		//Serial.print(", 0x");
		//Serial.print(rf95.readRegister((uint8_t)0x26), HEX);

		Serial.println();
		Serial.println("*********************************************");
		Serial.println("  LOOP!");
		Serial.println("*********************************************");
		Serial.println();
		intLastEventCount = intThisEventCount;
        // GetNetworkTime();
	}
    // printLocalTime();
	yield();
}