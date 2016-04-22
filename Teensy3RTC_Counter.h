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
#define RTC_COMPENSATE 0
#define TWO_POW_20 1048576

class _Teensy3RTC_CounterClass{
public:
  _Teensy3RTC_CounterClass(){};
  //Configuration methods
  void begin(){
     reset();
  }
  //Functionality methods
  void reset(){
    //while (_mutex){}; //spin until released
    _mutex = true;
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    RTC_IER &= ~0x10;                  //clear the TSIE bit (Time Seconds Interrupt Enable)
    RTC_SR = 0;                        //reset Status Register and disable seconds counter (!TCE)
    RTC_TPR = 0;                       //reset Time Prescaler Register
    _seconds = 0;
    _microseconds = 0;
    _leftover_microseconds = 0;
    RTC_SR = RTC_SR_TCE;               //renable the seconds counter (TCE)
    RTC_IER |= 0x10;                   //set the TSIE bit (Time Seconds Interrupt Enable)
    Teensy3Clock.compensate(RTC_COMPENSATE);
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);   //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    _mutex = false;
  }
  uint32_t get_seconds(){
    while (_mutex){}; //spin until released
    return _seconds;
  }
  uint32_t get_microseconds(){
    //while (_mutex){}; //spin until released
    return _compensated_micros();
  }
  float get_timestamp_float32(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    float timestamp = _seconds + _compensated_micros()/1e6;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  double get_timestamp_float64(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    double timestamp = (double) _seconds + (double) _compensated_micros()/1e6;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millis(){
    //warning: rolls over in ~49.7 days
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 1000*_seconds + (uint32_t) _compensated_micros()/1000;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millitenths(){
    //warning: rolls over in ~4.97 days
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 10000*_seconds + (uint32_t) _compensated_micros()/100;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millihundreths(){
    //warning: rolls over in ~11.9 hours
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 100000*_seconds + (uint32_t) _compensated_micros()/10;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_micros(){
    //warning: rolls over in ~1.19 hours
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 1000000*_seconds + (uint32_t) _compensated_micros();
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint64_t get_timestamp_micros64(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint64_t timestamp = 1000000*_seconds + (uint64_t) _compensated_micros();
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  void _seconds_tick(){
    _mutex = true;
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    //_leftover_microseconds = 1000000 - _microseconds;  //save for compensation
    _seconds++;
    _microseconds = 0;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    _mutex = false;
  }
  uint32_t _compensated_micros(){
    return _microseconds;
    //return (_microseconds*(TWO_POW_20 + _leftover_microseconds)) >> 20;  //OPTIMIZED DO NOT CHANGE
  }
  
private:
  //Attributes
  bool _mutex = false;           //controls access to the data for atomic reads
  volatile uint32_t _seconds;
  elapsedMicros _microseconds;
  volatile int32_t _leftover_microseconds;  //must be signed or weird things may happen!
} RTC_counter;

void rtc_seconds_isr(void)
{
  RTC_counter._seconds_tick();
}

#endif /* _TEENSY3RTC_COUNTER_H_INCLUDED */
