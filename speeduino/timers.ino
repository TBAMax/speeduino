/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/

/*
Timers are used for having actions performed repeatedly at a fixed interval (Eg every 100ms)
They should not be confused with Schedulers, which are for performing an action once at a given point of time in the future

Timers are typically low resolution (Compared to Schedulers), with maximum frequency currently being approximately every 10ms
*/
#include "timers.h"
#include "globals.h"
#include "sensors.h"
#include "scheduler.h"
#include "speeduino.h"
#include "scheduler.h"
#include "auxiliaries.h"

#if defined(CORE_AVR)
  #include <avr/wdt.h>
#endif

void initialiseTimers()
{
  lastRPM_100ms = 0;
}


//Timer2 Overflow Interrupt Vector, called when the timer overflows.
//Executes every ~1ms.
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER2_OVF_vect,ISR_NOBLOCK)
#else
void oneMSInterval() //Most ARM chips can simply call a function
#endif
{
  static byte loop33ms=0;  //(static variables are initialized only once, value of variable in the previous call gets carried through the next function call)
  static byte loop66ms=0;
  static byte loop100ms=0;
  static byte loop250ms=0;
  static uint16_t loopSec=0;

  ms_counter++;  //equivalent to arduino millis() but we just use global variable

  //Increment Loop Counters
  loop33ms++;
  loop66ms++;
  loop100ms++;
  loop250ms++;
  loopSec++;

  //1000Hz loop flag, just set this every time
  BIT_SET(TIMER_mask, BIT_TIMER_1MS);

  //30Hz loop flag
  if (loop33ms == 33)
  {
    loop33ms = 0;
    BIT_SET(TIMER_mask, BIT_TIMER_30HZ);
  }

  //15Hz loop flag
  if (loop66ms == 66)
  {
    loop66ms = 0;
    BIT_SET(TIMER_mask, BIT_TIMER_15HZ);
  }

  //10Hz loop flag
  if (loop100ms == 100)
  {
    loop100ms = 0; //Reset counter
    BIT_SET(TIMER_mask, BIT_TIMER_10HZ);
  }

  //4Hz loop flag
  if (loop250ms == 250)
  {
    loop250ms = 0; //Reset Counter
    BIT_SET(TIMER_mask, BIT_TIMER_4HZ);
  }

  //every 1 second flag (1ms x 1000 = 1000ms)
  if (loopSec == 1000)
  {
    loopSec = 0; //Reset counter.
    BIT_SET(TIMER_mask, BIT_TIMER_1HZ);
  }
#if defined(CORE_AVR) //AVR chips use the ISR for this
    //Reset Timer2 to trigger in another ~1ms
    TCNT2 = 131;            //Preload timer2 with 100 cycles, leaving 156 till overflow.
#endif
}

