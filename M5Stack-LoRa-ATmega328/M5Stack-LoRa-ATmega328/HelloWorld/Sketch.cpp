/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
/*End of auto generated code by Atmel studio */

#define  F_CPU 16000000UL # does this actually do anything?

//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio

void setup() {
  // put your setup code here, to run once:
  //pinMode(2, OUTPUT);
  delay(100);
  Serial.begin(2400 * 15.75);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");       // Hello World. (just the "H" to keep things simple)
  //digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(10);               // wait  
  //digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                // wait  HH
}
