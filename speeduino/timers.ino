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
#include "comms.h"

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

  //Tacho output check
  //Tacho is flagged as being ready for a pulse by the ignition outputs. 
  if(tachoOutputFlag == READY)
  {
    //Check for half speed tacho
    if( (configPage2.tachoDiv == 0) || (tachoAlt == true) ) 
    { 
      TACHO_PULSE_LOW();
      //ms_counter is cast down to a byte as the tacho duration can only be in the range of 1-6, so no extra resolution above that is required
      tachoEndTime = (uint8_t)ms_counter + configPage2.tachoDuration;
      tachoOutputFlag = ACTIVE;
    }
    else
    {
      //Don't run on this pulse (Half speed tacho)
      tachoOutputFlag = DEACTIVE;
    }
    tachoAlt = !tachoAlt; //Flip the alternating value incase half speed tacho is in use. 
  }
  else if(tachoOutputFlag == ACTIVE)
  {
    //If the tacho output is already active, check whether it's reached it's end time
    if((uint8_t)ms_counter == tachoEndTime)
    {
      TACHO_PULSE_HIGH();
      tachoOutputFlag = DEACTIVE;
    }
  }
  // Tacho sweep
  


  //30Hz loop
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

  //10Hz loop
  if (loop100ms == 100)
  {
    loop100ms = 0; //Reset counter
    BIT_SET(TIMER_mask, BIT_TIMER_10HZ);

    currentStatus.rpmDOT = (currentStatus.RPM - lastRPM_100ms) * 10; //This is the RPM per second that the engine has accelerated/decelleratedin the last loop
    lastRPM_100ms = currentStatus.RPM; //Record the current RPM for next calc

    if ( BIT_CHECK(currentStatus.engine, BIT_ENGINE_RUN) ) { runSecsX10++; }
    else { runSecsX10 = 0; }

    if ( (injPrimed == false) && (seclx10 == configPage2.primingDelay) && (currentStatus.RPM == 0) ) { beginInjectorPriming(); injPrimed = true; }
    seclx10++;
  }

  //4Hz loop
  if (loop250ms == 250)
  {
    loop250ms = 0; //Reset Counter
    BIT_SET(TIMER_mask, BIT_TIMER_4HZ);
  }

  //1Hz loop
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

