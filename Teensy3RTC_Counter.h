/*
  Teensy3RTC_Counter.h - Library for accurate timing
  Created by Craig Wm. Versek, 2016-04-11
 */
#ifndef _TEENSY3RTC_COUNTER_H_INCLUDED
#define _TEENSY3RTC_COUNTER_H_INCLUDED
#include <Arduino.h>



void rtc_configure_load_capacitance(uint8_t pF){
  RTC_CR &= ~0x3c00;  //clear load capacitor settings
  switch(pF){
  case 1:
  case 2:
  case 3:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    break;
  case 4:
  case 5:
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    break;
  case 6:
  case 7:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    break;
  case 8:
  case 9:
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    break;
  case 10:
  case 11:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    break;
  case 12:
  case 13:
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    break;
  case 14:
  case 15:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    break;
  case 16:
  case 17:
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 18:
  case 19:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 20:
  case 21:
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 22:
  case 23:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 24:
  case 25:
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 26:
  case 27:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 28:
  case 29:
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  case 30:
  default:
    RTC_CR |=  0x2000;  //set the 2pF capacitor
    RTC_CR |=  0x1000;  //set the 4pF capacitor
    RTC_CR |=  0x0800;  //set the 8pF capacitor
    RTC_CR |=  0x0400;  //set the 16pF capacitor
    break;
  }
}

uint32_t rtc_compensate_min_interval_min_error(float adjust_ppm)
{
  uint32_t interval, tcr, err;
 
  // We want to choose the smallest interval that minimizes the
  // error (remainder) in the calculation 
  if (adjust_ppm >= 0) {
    uint32_t min_err = 256;         //initialize to one more than largest possible error
    uint32_t tcr_min_err = 0;       //initialize to no compensation
    uint32_t interval_min_err = 0;  //initialize to shortest compensation interval 0, means 1 second

    float comp = adjust_ppm*8.0;
    for(interval = 1; interval < 256; interval++) {
      tcr = (int) (comp*interval + 0.5); //round to nearest integer
      err = tcr % 256;                   //this is the error due to discretization
      if (err < min_err){                //only save the best with the smallest interval
        min_err     = err;
        tcr_min_err = tcr;
        interval_min_err = interval;
      }
    }
    tcr = tcr_min_err >> 8;
    interval = interval_min_err;
  } 
  else {
    uint32_t min_err = 256;         //initialize to one more than largest possible error
    uint32_t tcr_min_err = 0;       //initialize to no compensation
    uint32_t interval_min_err = 0;  //initialize to shortest compensation interval 0, means 1 second
    
    float comp = -adjust_ppm*8.0;   //convert to a positie quantity
    for(interval = 1; interval < 256; interval++) {
      tcr = (int) (comp*interval + 0.5); //round to nearest integer
      err = tcr % 256;                   //this is the error due to discretization
      if (err < min_err){                //only save the best with the smallest interval
        min_err     = err;
        tcr_min_err = tcr;
        interval_min_err = interval;
      }
    }
    tcr = tcr_min_err >> 8;
    tcr = 256 - tcr;
    interval = interval_min_err;
  }
  RTC_TCR = ((interval - 1) << 8) | tcr;
  return interval;
}

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
