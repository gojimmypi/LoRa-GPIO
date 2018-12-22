// Similar to 32u4 w/ra02 LoRA see https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/using-the-rfm-9x-radio
// Gate COM8 (to left as viewed from front)

#include <SPI.h>
#include <RH_RF95.h>

#include "GlobalDefine.h"

#define BUTTON_PRESS_DURATION 250 // in milliseconds

#define RF95_FREQ 433.0 // MHz frequency. Reminder: must match RX's freq at Console!

// see https ://github.com/gojimmypi/LoRa-GPIO/blob/master/doc/DIY0031-LoRa32u4%20pinout%20diagram.pdf
// the values here are the bluish-gray "Arduino Pin" numbers, second column out from board
#define LED 13 // Blink  
#define REMOTE_BP1 6           // aka PD7  aka 27 aka T0, labeled on board as 6
#define REMOTE_BP2 12          // aka PD6  aka 26 aka T1, labeled on board as 12

#define RADIO_PACKET_SIZE  20
char tx_buf[RADIO_PACKET_SIZE]; // our transmit message buffer
// LoRa receive buffer
char rx_buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t rx_len = sizeof(rx_buf);

//
/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define DEVICEID "M5"


// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

//*******************************************************************************************************************************************
// Hello World, blink the LED for [duration] milliseconds  BLOCKING
//*******************************************************************************************************************************************
void blinkLED(int duration) {
    digitalWrite(LED, HIGH);
    delay(duration);
    digitalWrite(LED, LOW);
}

void resetLoRa() {
    delay(100);

    // manual reset of LoRa
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
    delay(250);

    Serial.begin(9600);
    // while (!Serial);
    delay(100);
    Serial.println("Gate Device Startup V0.001!");
    blinkLED(10);


    // LoRa32u4 GPIO init
    pinMode(LED, OUTPUT);

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
    // rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
    LORA_DEBUG_PRINT("Set Freq to: "); LORA_DEBUG_PRINTLN(RF95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin 
    // then you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);
    delay(250);
}

// Add the main program code into the continuous loop() function
void loop()
{
    // continually see if we have a message
    if (rf95.available())
    {
        blinkLED(200); // this also serves as delay to await all characters
        uint8_t len = sizeof(rx_buf);
        rf95.recv((uint8_t *)rx_buf, &len);
        RH_RF95::printBuffer("Received: ", (uint8_t *)rx_buf, len);
        LORA_MESSAGE_DEBUG_PRINT("Got reply: ");
        LORA_MESSAGE_DEBUG_PRINTLN(rx_buf);
        LORA_MESSAGE_DEBUG_PRINT("RSSI: ");
        LORA_MESSAGE_DEBUG_PRINTLN(rf95.lastRssi());
    }
    else
    {
        yield(); // <puff, puff />
        delay(1);
    }

}
