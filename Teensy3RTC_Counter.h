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

#define RTC_MILLIS         ((RTC_TPR*125)   >> 12)  /* OPTIMIZED = RTC_TPR*10^3/2^15 */
#define RTC_MILLITENTHS    ((RTC_TPR*625)   >> 11)  /* OPTIMIZED = RTC_TPR*10^4/2^15 */
#define RTC_MILLIHUNDRETHS ((RTC_TPR*3125)  >> 10)  /* OPTIMIZED = RTC_TPR*10^5/2^15 */
#define RTC_MICROS         ((RTC_TPR*15625) >> 9)   /* OPTIMIZED = RTC_TPR*10^6/2^15 */

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
    while (_mutex){}; //spin until released
    return RTC_MICROS;
  }
  float get_timestamp_float32(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    float timestamp = _seconds + RTC_MICROS/1e6;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  double get_timestamp_float64(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    double timestamp = (double) _seconds + (double) RTC_MICROS/1e6;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millis(){
    //warning: rolls over in ~49.7 days
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 1000*_seconds + (uint32_t) RTC_MILLIS;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millitenths(){
    //warning: rolls over in ~4.97 days
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 10000*_seconds + (uint32_t) RTC_MILLITENTHS;
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_millihundreths(){
    //warning: rolls over in ~11.9 hours
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 100000*_seconds + (uint32_t) RTC_MILLIHUNDRETHS;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint32_t get_timestamp_micros(){
    //warning: rolls over in ~1.19 hours
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint32_t timestamp = 1000000*_seconds + (uint32_t) RTC_MICROS;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  uint64_t get_timestamp_micros64(){
    while (_mutex){}; //spin until released
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    uint64_t timestamp = 1000000*_seconds + (uint64_t) RTC_MICROS;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    return timestamp;
  }
  void _seconds_tick(){
    _mutex = true;
    // CRITICAL SECTION --------------------------------------------------------
    NVIC_DISABLE_IRQ(IRQ_RTC_SECOND);  //disable interrupt
    _seconds++;
    NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);  //enable interrupt
    // END CRITICAL SECTION ----------------------------------------------------
    _mutex = false;
  }

  
private:
  //Attributes
  bool _mutex = false;           //controls access to the data for atomic reads
  volatile uint32_t _seconds;
} RTC_counter;

void rtc_seconds_isr(void)
{
  RTC_counter._seconds_tick();
}

#endif /* _TEENSY3RTC_COUNTER_H_INCLUDED */
