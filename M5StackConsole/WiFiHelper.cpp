#include "GlobalDefine.h"

#include "WiFiHelper.h"

//#include <ESP8266HTTPClient.h> // includes WiFiClient.h
//#include <ESP8266WiFi.h>

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD


String myMacAddress;

String wifiMacAddress() {
    return myMacAddress;
}

//*******************************************************************************************************************************************
// wifiConnect 
// 
//   WiFi.begin with repeated attempts with TFT screen and optional serial progress indication
//
//*******************************************************************************************************************************************
int wifiConnect(int maxAttempts) {
	WIFI_DEBUG_PRINTLN(DEBUG_SEPARATOR);
	WIFI_DEBUG_PRINT("wifiConnect: Connecting to ");
	WIFI_DEBUG_PRINTLN(WIFI_SSID);
	WIFI_DEBUG_PRINTLN(DEBUG_SEPARATOR);

	int countAttempt = 0;
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	WiFi.mode(WIFI_STA);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	WiFi.begin(WIFI_SSID, WIFI_PWD);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	myMacAddress = WiFi.macAddress(); // this returns 6 hex bytes, delimited by colons

	WIFI_DEBUG_PRINTLN("Starting WiFi Connection Loop...");
	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset

											 // this tft code is not actualy DOING anything yet

		WIFI_DEBUG_PRINT(".");
		delay(250);
		countAttempt++;
		if (countAttempt > maxAttempts) {
			countAttempt = 0;
			WIFI_DEBUG_PRINTLN("WiFi Disconnect... ");
			WiFi.disconnect();
			delay(5000);
			WIFI_DEBUG_PRINTLN("WiFi Retrying. ");
			WiFi.mode(WIFI_STA);
			WiFi.begin(WIFI_SSID, WIFI_PWD);
		}
	}
	WIFI_DEBUG_PRINTLN("Connected!");
	delay(5000);
	myMacAddress.replace(":", "");
	myMacAddress.replace("-", ""); // probably not used, but just in case they MAC address starts returning other well known delimiters such as dash
	myMacAddress.replace(" ", ""); // or perhaps even a space

	Serial.println("MAC Address=" + myMacAddress);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	return 0;
}