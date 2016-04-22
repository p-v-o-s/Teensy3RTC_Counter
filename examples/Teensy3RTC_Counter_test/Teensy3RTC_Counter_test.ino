/*
 Teensy3RTC_Counter.ino - Arduino Library Example sketch for interacting 
  Created by Craig Wm. Versek, 2016-04-11
  Released into the public domain.
*/

#include "Teensy3RTC_Counter.h"


#define LED_PIN 13

//configure the ADS chip FIXME updated to Controller Board Av4
//Teensy3RTC_CounterClass RTC_counter();

bool led_state = HIGH;

void setup() {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_state);   // turn the LED on (HIGH is the voltage level)
  delay(1000);
  
  RTC_counter.begin();
}




void loop() {
  float timestamp;
  Serial.print("tick ");
  timestamp = RTC_counter.get_timestamp_millihundreths()/1e5; //covert to seconds
  Serial.println(timestamp,6);
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(999);
  Serial.print("tock ");
  timestamp = RTC_counter.get_timestamp_millihundreths()/1e5; //convert to seconds
  Serial.println(timestamp,6);
  digitalWrite(LED_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(999);
}
