/*
  Teensy3RTC_Counter.h - Library for accurate timing
  Created by Craig Wm. Versek, 2016-04-11
 */
#ifndef _TEENSY3RTC_COUNTER_H_INCLUDED
#define _TEENSY3RTC_COUNTER_H_INCLUDED
#include <Arduino.h>

/*******************************************************************************
  Teensy3RTC_CounterClass
  
*******************************************************************************/

class _Teensy3RTC_CounterClass{
public:
  _Teensy3RTC_CounterClass(){};
  //Configuration methods
  void begin(){
     reset();
  }
  //Functionality methods
  void reset(){
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    _prescaler_offset  = RTC_TPR;      //save the current state of the Time Prescaler Register
    _seconds_offset    = RTC_TSR;      //save the current state of the Time Seconds Register
    _prescaler_stored  = _prescaler_offset; //ensure that the first "stored" timing is 0
    _seconds_stored    = _seconds_offset;   //ensure that the first "stored" timing is 0
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
  }
  void store(){
    //use to quickly mark the time for use in timestamps
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    _prescaler_stored = RTC_TPR;       //save the current state of the Time Prescaler Register
    _seconds_stored   = RTC_TSR;       //save the current state of the Time Seconds Register
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
  }
  uint32_t get_seconds(){
    return _seconds_count();
  }
  uint32_t get_microseconds(){
    return _micros_count();
  }
  float  get_timestamp_float32(){
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    float timestamp = _seconds_count() + _micros_count()/1e6;
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  double get_timestamp_float64(){
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    double timestamp = _seconds_count() + _micros_count()/1e6;
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millis(){
    //warning: rolls over in ~49.7 days
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    uint32_t timestamp = 1000*_seconds_count();
    timestamp +=  _millis_count();
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millitenths(){
    //warning: rolls over in ~4.97 days
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    uint32_t timestamp = 10000*_seconds_count();
    timestamp += _millitenths_count();
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millihundreths(){
    //warning: rolls over in ~11.9 hours
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    uint32_t timestamp = 100000*_seconds_count();
    timestamp +=  _millihundreths_count();
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_micros(){
    //warning: rolls over in ~1.19 hours
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    uint32_t timestamp = 1000000*_seconds_count();
    timestamp += _micros_count();
    return timestamp;
  }
  uint64_t get_timestamp_micros64(){
    // CRITICAL SECTION --------------------------------------------------------
    noInterrupts();
    uint64_t timestamp = 1000000*_seconds_count();
    timestamp += _micros_count();
    interrupts();
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }

private:
  uint32_t _seconds_count(){
    return  _seconds_stored - _seconds_offset;
  }
  int32_t _prescaler_count(){                    //NOTE: the return type must be signed!
    int32_t prescaler_value = _prescaler_stored; //convert to signed integer
    return prescaler_value - _prescaler_offset;
  }
  int32_t _micros_count(){                       // NOTE: the return type must be signed!
    return ((_prescaler_count()*15625) >> 9);    // OPTIMIZED = pc*10^6/2^15
  }
  int32_t _millihundreths_count(){               // NOTE: the return type must be signed!
    return ((_prescaler_count()*3125)  >> 10);   // OPTIMIZED = pc*10^5/2^15
  }
  int32_t _millitenths_count(){                  // NOTE: the return type must be signed!
    return ((_prescaler_count()*625)   >> 11);   // OPTIMIZED = pc*10^4/2^15
  }
  int32_t _millis_count(){                       // NOTE: the return type must be signed!
    return ((_prescaler_count()*125)   >> 12);   // OPTIMIZED = pc*10^3/2^15
  }
  //Attributes
  uint32_t _seconds_offset    = 0;
  uint32_t _prescaler_offset  = 0;
  uint32_t _seconds_stored    = 0;
  uint32_t _prescaler_stored  = 0;
  bool _mutex = false;           //controls access to the data for atomic reads
} RTC_counter;


#endif /* _TEENSY3RTC_COUNTER_H_INCLUDED */
