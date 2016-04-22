/*
 Teensy3RTC_Counter.ino - Arduino Library Example sketch for interacting 
  Created by Craig Wm. Versek, 2016-04-11
  Released into the public domain.
*/

#include "Teensy3RTC_Counter.h"


#define LED_PIN 13
#define LOOP_DELAY_MILLIS 1001

//configure the ADS chip FIXME updated to Controller Board Av4
//Teensy3RTC_CounterClass RTC_counter();

bool led_state = HIGH;

void setup() {
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_state);   // turn the LED on (HIGH is the voltage level)
  delay(5000);
  Serial.println("#Before RTC_counter.begin()");
  Serial.println("rtc_registers:");
  Serial.print("    RTC_TCR: "); Serial.println(RTC_TCR,HEX);
  Serial.print("    RTC_CR: ");  Serial.println(RTC_CR,HEX);
  Serial.print("    RTC_SR: ");  Serial.println(RTC_SR,HEX);
  Serial.print("    RTC_IER: "); Serial.println(RTC_IER,HEX);
  RTC_counter.begin();
  Serial.println("#After RTC_counter.begin()");
  Serial.println("rtc_registers:");
  Serial.print("    RTC_TCR: "); Serial.println(RTC_TCR,HEX);
  Serial.print("    RTC_CR: ");  Serial.println(RTC_CR,HEX);
  Serial.print("    RTC_SR: ");  Serial.println(RTC_SR,HEX);
  Serial.print("    RTC_IER: "); Serial.println(RTC_IER,HEX);
}



int loop_counter = 0;
void loop() {
  Serial.print("loop: ");Serial.println(loop_counter);
  RTC_counter.store(); //lock in a value for this moment
  //now export the value in all the possible ways
  Serial.print("    seconds:                  "); Serial.println(RTC_counter.get_seconds());
  Serial.print("    microseconds:             "); Serial.println(RTC_counter.get_microseconds());
  Serial.print("    timestamp_float32:        "); Serial.println(RTC_counter.get_timestamp_float32(),6);
  Serial.print("    timestamp_float64:        "); Serial.println(RTC_counter.get_timestamp_float64(),6);
  Serial.print("    timestamp_millis:         "); Serial.println(RTC_counter.get_timestamp_millis()/1e3,6);   //convert to seconds
  Serial.print("    timestamp_millitenths:    "); Serial.println(RTC_counter.get_timestamp_millitenths()/1e4,6); //convert to seconds
  Serial.print("    timestamp_millihundreths: "); Serial.println(RTC_counter.get_timestamp_millihundreths()/1e5,6); //convert to seconds
  Serial.print("    timestamp_micros:         "); Serial.println(RTC_counter.get_timestamp_micros()/1e6,6); //convert to seconds
  Serial.print("    timestamp_micros64:       "); Serial.println(RTC_counter.get_timestamp_micros64()/1e6,6); //convert to seconds
  //pulse LED on-off each cycle
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  digitalWrite(LED_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  loop_counter++;
}
