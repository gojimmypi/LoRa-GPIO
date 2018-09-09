// GlobalDefine.h

#ifndef _GLOBALDEFINE_h
#define _GLOBALDEFINE_h

// arduino.h allows some handy types, such as String
#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


//*************************************************************************************************************************************************************************************************
// Begin user config
//*******************************************************************************************************************************************
// My config is stored in myPrivateSettings.h file 
// if you choose not to use such a file, set this to false:
#define USE_myPrivateSettings true

// Note the two possible file name string formats.
#if USE_myPrivateSettings == true 
#include "/workspace-git/myPrivateSettings.h"
static const char* ntpServer = "time.nist.gov";
static const char* ntpServer2 = "pool.ntp.org";
static const char* ntpServer3 = "time.windows.com";
static const long  gmtOffset_sec = -28800; //  San Francisco (GMT-8) = -25200; Paris (GMT+1) = 3600
static const int   daylightOffset_sec = 0;

#else
#pragma message(Reminder "Settings needed !")
static const char* WIFI_SSID = "mySSID";
static const char* WIFI_PWD = "myPASSWORD";
static const char* ntpServer = "time.nist.gov";
static const char* ntpServer2 = "pool.ntp.org";
static const char* ntpServer3 = "time.windows.com";
const long  gmtOffset_sec = 0; //  San Francisco (GMT-7) = ; Paris (GMT+2) = 3600
const int   daylightOffset_sec = 0;
#endif
//**************************************************************************************************************
//**************************************************************************************************************
// End user config
//**************************************************************************************************************
//**************************************************************************************************************

//**************************************************************************************************************
// debugging options
//**************************************************************************************************************
// #define SCREEN_DEBUG // when defined, display low level screen debug info 
// #define SCREEN_DATA_DEBUG // when defined, print screen data also to serial terminal
// #define JSON_DEBUG // when defined, display JSON debug info 
#define WIFI_DEBUG // when defined, display WiFi debug info 
#define SERIAL_SCREEN_DEBUG // when defined, display screen messages to serial port
#define HTTP_DEBUG // when defined, display WiFi debug info 
#define DEBUG_SEPARATOR "***********************************"
#define TIMER_DEBUG // when defined, display diagnostic timer info
#define HEAP_DEBUG // when defined, display diagnostic heap info
#define HARDWARE_DEBUG
// #define SPIFFS_DEBUG
// #define SENSOR_DEBUG
#define LORA_DEBUG
#define LORA_MESSAGE_DEBUG

//**************************************************************************************************************
// board-architecture-dependent WIFI_CLIENT_CLASS type selector 
//**************************************************************************************************************
#define USE_TLS_SSL // when defined, JSON data will use SSL
#define WIFI_CLIENT_CLASS WiFiClient  // the name of the WiFi class may vary depending on (1) architecture and (2) using TLS or not

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
    #undef  WIFI_CLIENT_CLASS
    #ifdef USE_TLS_SSL
        #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure
    #else
        #define THE_CLIENT_TYPE WiFiClient // no TLS 
    #endif // USE_TLS_SSL

    //#define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure
    #define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
// #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure not supprted in ESP32 ?
    #undef  WIFI_CLIENT_CLASS
    #ifdef USE_TLS_SSL
        #define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure default
    #else
        #define THE_CLIENT_TYPE WiFiClient // no TLS 
    #endif // USE_TLS_SSL
    #define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined! (currently only supporting ESP32 and ESP8266)")
#endif // ! FOUND_BOARD


//**************************************************************************************************************
//
//**************************************************************************************************************

// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
//
// see https://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
//

//#define Stringize( L )     #L 
//#define MakeString( M, L ) M(L)
//#define $Line MakeString( Stringize, __LINE__ )
//#define Reminder __FILE__ "(" $Line ") : Reminder: "


//**************************************************************************************************************
// some optional Serial.print() statements...
//**************************************************************************************************************
#ifdef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           (Serial.print(string))
#define WIFI_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           ((void)0)
#define WIFI_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************
#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           (Serial.print(string))
#define HTTP_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           ((void)0)
#define HTTP_DEBUG_PRINTLN(string)         ((void)0)
#endif
//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         (Serial.print(string))
#define SCREEN_DEBUG_PRINTLN(string)       (Serial.println(string))
#endif

#ifndef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         ((void)0)
#define SCREEN_DEBUG_PRINTLN(string)       ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    (Serial.print(string))
#define SCREEN_DATA_DEBUG_PRINTLN(string)  (Serial.println(string))
#endif

#ifndef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    ((void)0)
#define SCREEN_DATA_DEBUG_PRINTLN(string)  ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           (Serial.print(string))
#define JSON_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           ((void)0)
#define JSON_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           (Serial.print(string))
#define TIMER_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           ((void)0)
#define TIMER_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef HEAP_DEBUG

//static char * HEAP_DEBUG_MSG = "Heap = ";
//#define HEAP_DEBUG_PRINT(string)           (Serial.print  ( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTLN(string)         (Serial.println( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t)                                                   )

#define DEFAULT_DEBUG_MESSAGE DefaultDebugMessage()
#define SET_HEAP_MESSAGE(thisStr)	       (setHeapMsg(thisStr))
#define HEAP_DEBUG_PRINT(thisStr)          (Serial.print  (  (DefaultDebugMessage().compareTo(thisStr) == 0) ? (getHeapMsg() + (String)ESP.getFreeHeap()) : (String)thisStr + " Heap = " + (String)ESP.getFreeHeap() + "; ") )
#define HEAP_DEBUG_PRINTLN(thisStr)        (Serial.println(  (DefaultDebugMessage().compareTo(thisStr) == 0) ? (getHeapMsg() + (String)ESP.getFreeHeap()) : (String)thisStr + " Heap = " + (String)ESP.getFreeHeap() + "; ") )
#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t) )
#endif

#ifndef HEAP_DEBUG
static const char *  HEAP_DEBUG_MSG = "";
#define SET_HEAP_MESSAGE(string)		   ((void)0)
#define HEAP_DEBUG_PRINT(string)           ((void)0)
#define HEAP_DEBUG_PRINTF(string)          ((void)0)
#define HEAP_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SPIFFS_DEBUG
#define SPIFFS_DEBUG_PRINT(string)           (Serial.print(string))
#define SPIFFS_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef SPIFFS_DEBUG
#define SPIFFS_DEBUG_PRINT(string)           ((void)0)
#define SPIFFS_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SENSOR_DEBUG
#define SENSOR_DEBUG_PRINT(string)           (Serial.print(string))
#define SENSOR_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef SENSOR_DEBUG
#define SENSOR_DEBUG_PRINT(string)           ((void)0)
#define SENSOR_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************
// LORA_DEBUG - for low level operations
//**************************************************************************************************************
#ifdef LORA_DEBUG
#define LORA_DEBUG_PRINT(string)           (Serial.print(string))
#define LORA_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef LORA_DEBUG
#define LORA_DEBUG_PRINT(string)           ((void)0)
#define LORA_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************
// LORA_MESSAGE_DEBUG - for application message information
//**************************************************************************************************************
#ifdef LORA_MESSAGE_DEBUG
#define LORA_MESSAGE_DEBUG_PRINT(string)           (Serial.print(string))
#define LORA_MESSAGE_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef LORA_MESSAGE_DEBUG
#define LORA_MESSAGE_DEBUG_PRINT(string)           ((void)0)
#define LORA_MESSAGE_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************

    void setHeapMsg(String str);

	String getHeapMsg();

	String DefaultDebugMessage();

 


#endif // _GLOBALDEFINE_h

