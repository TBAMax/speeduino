/*
Speeduino - Simple engine management for the Arduino Mega 2560 platform
Copyright (C) Josh Stewart
A full copy of the license may be found in the projects root directory
*/
/** @file
 * Injector and Ignition (on/off) scheduling (functions).
 * There is usually 8 functions for cylinders 1-8 with same naming pattern.
 * 
 * ## Scheduling structures
 * 
 * Structures @ref FuelSchedule and @ref Schedule describe (from scheduler.h) describe the scheduling info for Fuel and Ignition respectively.
 * They contain duration, current activity status, start timing, end timing, callbacks to carry out action, etc.
 * 
 * ## Scheduling Functions
 * 
 * For Injection:
 * - setFuelSchedule*(tout,dur) - **Setup** schedule for (next) injection on the channel
 * - inj*StartFunction() - Execute **start** of injection (Interrupt handler)
 * - inj*EndFunction() - Execute **end** of injection (interrupt handler)
 * 
 * For Ignition (has more complex schedule setup):
 * - setIgnitionSchedule*(cb_st,tout,dur,cb_end) - **Setup** schedule for (next) ignition on the channel
 * - ign*StartFunction() - Execute **start** of ignition (Interrupt handler)
 * - ign*EndFunction() - Execute **end** of ignition (Interrupt handler)
 */
#include "globals.h"
#include "scheduler.h"
#include "scheduledIO.h"
#include "crankMaths.h"

FuelSchedule fuelSchedule1;
FuelSchedule fuelSchedule2;
FuelSchedule fuelSchedule3;
FuelSchedule fuelSchedule4;
#if (INJ_CHANNELS >= 5)
FuelSchedule fuelSchedule5;
#endif
#if (INJ_CHANNELS >= 6)
FuelSchedule fuelSchedule6;
#endif
#if (INJ_CHANNELS >= 7)
FuelSchedule fuelSchedule7;
#endif
#if (INJ_CHANNELS >= 8)
FuelSchedule fuelSchedule8;
#endif

Schedule ignitionSchedule1;
Schedule ignitionSchedule2;
Schedule ignitionSchedule3;
Schedule ignitionSchedule4;
Schedule ignitionSchedule5;
#if IGN_CHANNELS >= 6
Schedule ignitionSchedule6;
#endif
#if IGN_CHANNELS >= 7
Schedule ignitionSchedule7;
#endif
#if IGN_CHANNELS >= 8
Schedule ignitionSchedule8;
#endif

void initialiseSchedulers()
{
    //nullSchedule.Status = OFF;
    
    fuelSchedule1.Status = OFF;
    fuelSchedule1.setFuelCompare = setFuel1Compare;
    fuelSchedule1.getFuelCounter = getFuel1Counter;
    fuelSchedule1.fuelTimerDisable = fuel1TimerDisable;
    fuelSchedule1.fuelTimerEnable = fuel1TimerEnable;
    
    fuelSchedule2.Status = OFF;
    fuelSchedule2.setFuelCompare = setFuel2Compare;
    fuelSchedule2.getFuelCounter = getFuel2Counter;
    fuelSchedule2.fuelTimerDisable = fuel2TimerDisable;
    fuelSchedule2.fuelTimerEnable = fuel2TimerEnable;
    
    fuelSchedule3.Status = OFF;
    fuelSchedule3.setFuelCompare = setFuel3Compare;
    fuelSchedule3.getFuelCounter = getFuel3Counter;
    fuelSchedule3.fuelTimerDisable = fuel3TimerDisable;
    fuelSchedule3.fuelTimerEnable = fuel3TimerEnable;

    fuelSchedule4.Status = OFF;
    fuelSchedule4.setFuelCompare = setFuel4Compare;
    fuelSchedule4.getFuelCounter = getFuel4Counter;
    fuelSchedule4.fuelTimerDisable = fuel4TimerDisable;
    fuelSchedule4.fuelTimerEnable = fuel4TimerEnable;
  
    #if (INJ_CHANNELS >= 5)
    fuelSchedule5.Status = OFF;
    fuelSchedule5.setFuelCompare = setFuel5Compare;
    fuelSchedule5.getFuelCounter = getFuel5Counter;
    fuelSchedule5.fuelTimerDisable = fuel5TimerDisable;
    fuelSchedule5.fuelTimerEnable = fuel5TimerEnable;
    #endif 
    #if (INJ_CHANNELS >= 6)
    fuelSchedule6.Status = OFF;
    fuelSchedule6.setFuelCompare = setFuel6Compare;
    fuelSchedule6.getFuelCounter = getFuel6Counter;
    fuelSchedule6.fuelTimerDisable = fuel6TimerDisable;
    fuelSchedule6.fuelTimerEnable = fuel6TimerEnable;
    #endif
    #if (INJ_CHANNELS >= 7)
    fuelSchedule7.Status = OFF;
    fuelSchedule7.setFuelCompare = setFuel7Compare;
    fuelSchedule7.getFuelCounter = getFuel7Counter;
    fuelSchedule7.fuelTimerDisable = fuel7TimerDisable;
    fuelSchedule7.fuelTimerEnable = fuel7TimerEnable;
    #endif
    #if (INJ_CHANNELS >= 8)
    fuelSchedule8.Status = OFF;
    fuelSchedule8.setFuelCompare = setFuel8Compare;
    fuelSchedule8.getFuelCounter = getFuel8Counter;
    fuelSchedule8.fuelTimerDisable = fuel8TimerDisable;
    fuelSchedule8.fuelTimerEnable = fuel8TimerEnable;
    #endif      
    
    ignitionSchedule1.Status = OFF;
    ignitionSchedule1.getIgnCounter = getIgn1Counter;
    ignitionSchedule1.setIgnitionCompare = setIgnition1Compare;
    ignitionSchedule1.ignTimerEnable = ign1TimerEnable;
    ignitionSchedule1.ignTimerDisable = ign1TimerDisable;    

    ignitionSchedule2.Status = OFF;
    ignitionSchedule2.getIgnCounter = getIgn2Counter;
    ignitionSchedule2.setIgnitionCompare = setIgnition2Compare;
    ignitionSchedule2.ignTimerEnable = ign2TimerEnable;
    ignitionSchedule2.ignTimerDisable = ign2TimerDisable;   

    ignitionSchedule3.Status = OFF;
    ignitionSchedule3.getIgnCounter = getIgn3Counter;
    ignitionSchedule3.setIgnitionCompare = setIgnition3Compare;
    ignitionSchedule3.ignTimerEnable = ign3TimerEnable;
    ignitionSchedule3.ignTimerDisable = ign3TimerDisable;   

    ignitionSchedule4.Status = OFF;
    ignitionSchedule4.getIgnCounter = getIgn4Counter;
    ignitionSchedule4.setIgnitionCompare = setIgnition4Compare;
    ignitionSchedule4.ignTimerEnable = ign4TimerEnable;
    ignitionSchedule4.ignTimerDisable = ign4TimerDisable;   

    ignitionSchedule5.Status = OFF;
    ignitionSchedule5.getIgnCounter = getIgn5Counter;
    ignitionSchedule5.setIgnitionCompare = setIgnition5Compare;
    ignitionSchedule5.ignTimerEnable = ign5TimerEnable;
    ignitionSchedule5.ignTimerDisable = ign5TimerDisable;   

    #if IGN_CHANNELS >= 6
    ignitionSchedule6.Status = OFF;
    ignitionSchedule6.getIgnCounter = getIgn6Counter;
    ignitionSchedule6.setIgnitionCompare = setIgnition6Compare;
    ignitionSchedule6.ignTimerEnable = ign6TimerEnable;
    ignitionSchedule6.ignTimerDisable = ign6TimerDisable;   
    #endif
    #if IGN_CHANNELS >= 7
    ignitionSchedule7.Status = OFF;
    ignitionSchedule7.getIgnCounter = getIgn7Counter;
    ignitionSchedule7.setIgnitionCompare = setIgnition7Compare;
    ignitionSchedule7.ignTimerEnable = ign7TimerEnable;
    ignitionSchedule7.ignTimerDisable = ign7TimerDisable;   
    #endif
    #if IGN_CHANNELS >= 8
    ignitionSchedule8.Status = OFF;
    ignitionSchedule8.getIgnCounter = getIgn8Counter;
    ignitionSchedule8.setIgnitionCompare = setIgnition8Compare;
    ignitionSchedule8.ignTimerEnable = ign8TimerEnable;
    ignitionSchedule8.ignTimerDisable = ign8TimerDisable;   
    #endif   

}
//(convert macros to inline functions for now)
inline void setFuel1Compare(COMPARE_TYPE compareValue){FUEL1_COMPARE =compareValue;}
inline void setFuel2Compare(COMPARE_TYPE compareValue){FUEL2_COMPARE =compareValue;}
inline void setFuel3Compare(COMPARE_TYPE compareValue){FUEL3_COMPARE =compareValue;}
inline void setFuel4Compare(COMPARE_TYPE compareValue){FUEL4_COMPARE =compareValue;}
inline void setFuel5Compare(COMPARE_TYPE compareValue){FUEL5_COMPARE =compareValue;}
inline void setFuel6Compare(COMPARE_TYPE compareValue){FUEL6_COMPARE =compareValue;}
inline void setFuel7Compare(COMPARE_TYPE compareValue){FUEL7_COMPARE =compareValue;}
inline void setFuel8Compare(COMPARE_TYPE compareValue){FUEL8_COMPARE =compareValue;}

//fuel counter getting functions(convert macros to inline functions)
inline COMPARE_TYPE getFuel1Counter() {return FUEL1_COUNTER;}
inline COMPARE_TYPE getFuel2Counter() {return FUEL2_COUNTER;}
inline COMPARE_TYPE getFuel3Counter() {return FUEL3_COUNTER;}
inline COMPARE_TYPE getFuel4Counter() {return FUEL4_COUNTER;}
inline COMPARE_TYPE getFuel5Counter() {return FUEL5_COUNTER;}
inline COMPARE_TYPE getFuel6Counter() {return FUEL6_COUNTER;}
inline COMPARE_TYPE getFuel7Counter() {return FUEL7_COUNTER;}
inline COMPARE_TYPE getFuel8Counter() {return FUEL8_COUNTER;}

//(convert macros to inline functions for now)
inline void fuel1TimerDisable(){FUEL1_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel2TimerDisable(){FUEL2_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel3TimerDisable(){FUEL3_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel4TimerDisable(){FUEL4_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel5TimerDisable(){FUEL5_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel6TimerDisable(){FUEL6_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel7TimerDisable(){FUEL7_TIMER_DISABLE();}//fuel timer disable functions 
inline void fuel8TimerDisable(){FUEL8_TIMER_DISABLE();}//fuel timer disable functions

//(convert macros to inline functions for now)
inline void fuel1TimerEnable(){FUEL1_TIMER_ENABLE();}//fuel timer enable functions 
inline void fuel2TimerEnable(){FUEL2_TIMER_ENABLE();}//fuel timer enable functions
inline void fuel3TimerEnable(){FUEL3_TIMER_ENABLE();}//fuel timer enable functions 
inline void fuel4TimerEnable(){FUEL4_TIMER_ENABLE();}//fuel timer enable functions 
inline void fuel5TimerEnable(){FUEL5_TIMER_ENABLE();}//fuel timer enable functions
inline void fuel6TimerEnable(){FUEL6_TIMER_ENABLE();}//fuel timer enable functions
inline void fuel7TimerEnable(){FUEL7_TIMER_ENABLE();}//fuel timer enable functions
inline void fuel8TimerEnable(){FUEL8_TIMER_ENABLE();}//fuel timer enable functions

/*
These 8 function turn a schedule on, provides the time to start and the duration and gives it callback functions.
All 8 functions operate the same, just on different schedules
Args:
duration: The number of uS after startCallback is called before endCallback is called
*/

//New generic function.
void setFuelSchedule (struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration)
{
  unsigned long timeout;

  while (injectorEndAngle <= crankAngle)   
    { 
      injectorEndAngle += CRANK_ANGLE_MAX_INJ; //calculate into the next cycle
    } 
  timeout=(injectorEndAngle - crankAngle) * (unsigned long)timePerDegree;
  
  if(targetSchedule->Status != RUNNING) //Check that we're not already part way through a schedule
  {
    if((timeout < MAX_TIMER_PERIOD) && (timeout > duration + INJECTION_REFRESH_TRESHOLD)) //Need to check that the timeout doesn't exceed the overflow, also allow for fixed 230us safety between setting the schedule and running it
    {      
      noInterrupts(); // make sure start and end values are updated simultaneously
      targetSchedule->endCompare = targetSchedule->getFuelCounter() + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout)); //As there is a tick every 4uS, there are timeout/4 ticks until the interrupt should be triggered ( >>2 divides by 4)   
      targetSchedule->setFuelCompare(targetSchedule->endCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration))); // previously startCompare
      targetSchedule->Status = PENDING; //Turn this schedule on
      interrupts(); 
      targetSchedule->fuelTimerEnable();
    }
  }
  else 
  {
    //If the schedule is already running, we can set the next schedule so it is ready to go
    //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
    injectorEndAngle += CRANK_ANGLE_MAX_INJ;
    timeout=(injectorEndAngle - crankAngle) * (unsigned long)timePerDegree;
      if(timeout < MAX_TIMER_PERIOD)
      {
      noInterrupts();
      targetSchedule->nextEndCompare = targetSchedule->getFuelCounter() + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout));
      targetSchedule->nextStartCompare = targetSchedule->nextEndCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration));      
      targetSchedule->hasNextSchedule = true;
      interrupts();
      }
  }
}

//separate function for setting the fuel schedules at priming
void setFuelSchedule (struct FuelSchedule *targetSchedule, unsigned long duration)
{
  if(targetSchedule->Status != RUNNING) //Check that we're not already part way through a schedule
  {      
      targetSchedule->injStartFunction();
      noInterrupts(); // make sure start and end values are updated simultaneously
      targetSchedule->setFuelCompare(targetSchedule->getFuelCounter() + (COMPARE_TYPE)uS_TO_TIMER_COMPARE(duration));
      targetSchedule->Status = RUNNING; //RUN this schedule immediately
      targetSchedule->hasNextSchedule = false;
      interrupts(); 
      targetSchedule->fuelTimerEnable();
  }
}

//ignition timer enable functions
inline void ign1TimerEnable(){IGN1_TIMER_ENABLE();}
inline void ign2TimerEnable(){IGN2_TIMER_ENABLE();}
inline void ign3TimerEnable(){IGN3_TIMER_ENABLE();}
inline void ign4TimerEnable(){IGN4_TIMER_ENABLE();}
inline void ign5TimerEnable(){IGN5_TIMER_ENABLE();}
inline void ign6TimerEnable(){IGN6_TIMER_ENABLE();}
inline void ign7TimerEnable(){IGN7_TIMER_ENABLE();}
inline void ign8TimerEnable(){IGN8_TIMER_ENABLE();}

inline void ign1TimerDisable(){IGN1_TIMER_DISABLE();}
inline void ign2TimerDisable(){IGN2_TIMER_DISABLE();}
inline void ign3TimerDisable(){IGN3_TIMER_DISABLE();}
inline void ign4TimerDisable(){IGN4_TIMER_DISABLE();}
inline void ign5TimerDisable(){IGN5_TIMER_DISABLE();}
inline void ign6TimerDisable(){IGN6_TIMER_DISABLE();}
inline void ign7TimerDisable(){IGN7_TIMER_DISABLE();}
inline void ign8TimerDisable(){IGN8_TIMER_DISABLE();}

inline COMPARE_TYPE getIgn1Counter() {return IGN1_COUNTER;}
inline COMPARE_TYPE getIgn2Counter() {return IGN2_COUNTER;}
inline COMPARE_TYPE getIgn3Counter() {return IGN3_COUNTER;}
inline COMPARE_TYPE getIgn4Counter() {return IGN4_COUNTER;}
inline COMPARE_TYPE getIgn5Counter() {return IGN5_COUNTER;}
inline COMPARE_TYPE getIgn6Counter() {return IGN6_COUNTER;}
inline COMPARE_TYPE getIgn7Counter() {return IGN7_COUNTER;}
inline COMPARE_TYPE getIgn8Counter() {return IGN8_COUNTER;}

inline void setIgnition1Compare(COMPARE_TYPE compareValue){IGN1_COMPARE =compareValue;}
inline void setIgnition2Compare(COMPARE_TYPE compareValue){IGN2_COMPARE =compareValue;}
inline void setIgnition3Compare(COMPARE_TYPE compareValue){IGN3_COMPARE =compareValue;}
inline void setIgnition4Compare(COMPARE_TYPE compareValue){IGN4_COMPARE =compareValue;}
inline void setIgnition5Compare(COMPARE_TYPE compareValue){IGN5_COMPARE =compareValue;}
inline void setIgnition6Compare(COMPARE_TYPE compareValue){IGN6_COMPARE =compareValue;}
inline void setIgnition7Compare(COMPARE_TYPE compareValue){IGN7_COMPARE =compareValue;}
inline void setIgnition8Compare(COMPARE_TYPE compareValue){IGN8_COMPARE =compareValue;}

//Ignition schedulers use Timer 5
void setIgnitionSchedule(struct Schedule *targetSchedule ,  int16_t crankAngle, int ignitionEndAngle, unsigned long duration)
{
  unsigned long timeout;

  while (ignitionEndAngle <= crankAngle)   { ignitionEndAngle += CRANK_ANGLE_MAX_IGN; } //calculate into the next cycle
//  timeout=(tempEndAngle - crankAngle) * (unsigned long)timePerDegree;
  timeout= angleToTime((ignitionEndAngle - crankAngle), CRANKMATH_METHOD_INTERVAL_REV);
  
  if (targetSchedule->Status != RUNNING) //Check that we're not already part way through a schedule
  {
    if((timeout < MAX_TIMER_PERIOD) && (timeout > duration + IGNITION_REFRESH_THRESHOLD)) //Need to check that the timeout doesn't exceed the overflow, also allow for fixed 230us safety between setting the schedule and running it
    {      
      noInterrupts(); // make sure start and end values are updated simultaneously
      targetSchedule->endCompare = targetSchedule->getIgnCounter() + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout)); //As there is a tick every 4uS, there are timeout/4 ticks until the interrupt should be triggered ( >>2 divides by 4)   
      targetSchedule->setIgnitionCompare(targetSchedule->endCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration))); // previously startCompare
      targetSchedule->Status = PENDING; //Turn this schedule on
      interrupts(); 
      targetSchedule->ignTimerEnable();
    }
  }
  else 
  {
    //If the schedule is already running, we can set the next schedule so it is ready to go
    //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
    ignitionEndAngle += CRANK_ANGLE_MAX_IGN;
    //timeout=(tempEndAngle - crankAngle) * (unsigned long)timePerDegree;
    timeout= angleToTime((ignitionEndAngle - crankAngle), CRANKMATH_METHOD_INTERVAL_REV);
      if(timeout < MAX_TIMER_PERIOD)
      {
      noInterrupts();
      targetSchedule->nextEndCompare = targetSchedule->getIgnCounter() + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout));
      targetSchedule->nextStartCompare = targetSchedule->nextEndCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration));      
      targetSchedule->hasNextSchedule = true;
      interrupts();
      }
  }
}

//overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition
void setIgnitionSchedule(struct Schedule *ignitionSchedule)
{            
             ignitionSchedule->StartFunction(); //start coil charging
             ignitionSchedule->setIgnitionCompare(ignitionSchedule->getIgnCounter()+ (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(currentStatus.dwell)));
             ignitionSchedule->Status=RUNNING;
             ignitionSchedule->ignTimerEnable();
}

extern void beginInjectorPriming()
{
  unsigned long primingValue = table2D_getValue(&PrimingPulseTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET);
  if( (primingValue > 0) && (currentStatus.TPS < configPage4.floodClear) )
  {
    primingValue = primingValue * 100 * 5; //to acheive long enough priming pulses, the values in tuner studio are divided by 0.5 instead of 0.1, so multiplier of 5 is required.
    if ( channel1InjEnabled == true ) { setFuelSchedule(&fuelSchedule1, primingValue); }
#if (INJ_CHANNELS >= 2)
    if ( channel2InjEnabled == true ) { setFuelSchedule(&fuelSchedule2, primingValue); }
#endif
#if (INJ_CHANNELS >= 3)
    if ( channel3InjEnabled == true ) { setFuelSchedule(&fuelSchedule3, primingValue); }
#endif
#if (INJ_CHANNELS >= 4)
    if ( channel4InjEnabled == true ) { setFuelSchedule(&fuelSchedule4, primingValue); }
#endif
#if (INJ_CHANNELS >= 5)
    if ( channel5InjEnabled == true ) { setFuelSchedule(&fuelSchedule5, primingValue); }
#endif
#if (INJ_CHANNELS >= 6)
    if ( channel6InjEnabled == true ) { setFuelSchedule(&fuelSchedule6, primingValue); }
#endif
#if (INJ_CHANNELS >= 7)
    if ( channel7InjEnabled == true) { setFuelSchedule(&fuelSchedule7, primingValue); }
#endif
#if (INJ_CHANNELS >= 8)
    if ( channel8InjEnabled == true ) { setFuelSchedule(&fuelSchedule8, primingValue); }
#endif
  }
}

/*******************************************************************************************************************************************************************************************************/
/** fuelSchedule*Interrupt (All 8 ISR functions below) get called (as timed interrupts) when either the start time or the duration time are reached.
* This calls the relevant callback function (startCallback or endCallback) depending on the status (PENDING => Needs to run, RUNNING => Needs to stop) of the schedule.
* The status of schedule is managed here based on startCallback /endCallback function called:
* - startCallback - change scheduler into RUNNING state
* - endCallback - change scheduler into OFF state (or PENDING if schedule.hasNextSchedule is set)
*/
//Timer3A (fuel schedule 1) Compare Vector
#if (INJ_CHANNELS >= 1)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPA_vect) //fuelSchedules 1 and 5
#else
static inline void fuelSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule1);
}
#endif

#if (INJ_CHANNELS >= 2)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //fuelSchedule2
#else
static inline void fuelSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule2);
}
#endif

#if (INJ_CHANNELS >= 3)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //fuelSchedule3
#else
static inline void fuelSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule3);
}
#endif

#if (INJ_CHANNELS >= 4)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //fuelSchedule4
#else
static inline void fuelSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule4);
}
#endif

#if (INJ_CHANNELS >= 5)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //fuelSchedule5
#else
static inline void fuelSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule5);
}
#endif

#if (INJ_CHANNELS >= 6)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //fuelSchedule6
#else
static inline void fuelSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule6);
}
#endif

#if (INJ_CHANNELS >= 7)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //fuelSchedule7
#else
static inline void fuelSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule7);
}
#endif

#if (INJ_CHANNELS >= 8)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //fuelSchedule8
#else
static inline void fuelSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule8);
}
#endif

void fuelScheduleInterrupt(struct FuelSchedule *fuelSchedule)
{
    if (fuelSchedule->Status == PENDING) //Check to see if this schedule is turn on
    {
      fuelSchedule->injStartFunction();
      fuelSchedule->Status = RUNNING; //Set the status to be in progress (ie The start callback has been called, but not the end callback)
      fuelSchedule->setFuelCompare(fuelSchedule->endCompare);
    }
    else if (fuelSchedule->Status == RUNNING)
    {
       fuelSchedule->injEndFunction();
       fuelSchedule->Status = OFF; //Turn off the schedule

       //If there is a next schedule queued up, activate it
       if(fuelSchedule->hasNextSchedule == true)
       {
         fuelSchedule->setFuelCompare(fuelSchedule->nextStartCompare);
         fuelSchedule->endCompare = fuelSchedule->nextEndCompare;
         fuelSchedule->Status = PENDING;
         fuelSchedule->hasNextSchedule = false;
       }
       else {fuelSchedule->fuelTimerDisable(); }
    }
    else {
      fuelSchedule->injEndFunction();
      fuelSchedule->fuelTimerDisable(); } //Safety check. Turn off this output compare unit and return without performing any action
}

#if IGN_CHANNELS >= 1
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPA_vect) //ignitionSchedule1
#else
static inline void ignitionSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule1);
}
#endif

#if IGN_CHANNELS >= 2
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //ignitionSchedule2
#else
static inline void ignitionSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule2);
}
#endif

#if IGN_CHANNELS >= 3
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //ignitionSchedule3
#else
static inline void ignitionSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule3);
}
#endif

#if IGN_CHANNELS >= 4
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //ignitionSchedule4
#else
static inline void ignitionSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule4);
}
#endif

#if IGN_CHANNELS >= 5
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //ignitionSchedule5
#else
static inline void ignitionSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule5);
}
#endif

#if IGN_CHANNELS >= 6
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //ignitionSchedule6
#else
static inline void ignitionSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule6);
}
#endif

#if IGN_CHANNELS >= 7
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //ignitionSchedule6
#else
static inline void ignitionSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule7);
}
#endif

#if IGN_CHANNELS >= 8
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //ignitionSchedule8
#else
static inline void ignitionSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule8);
}
#endif

void ignitionScheduleInterrupt(struct Schedule *targetSchedule) // common function that all ignition channel interrupts use
{
    if (targetSchedule->Status == PENDING) //Check to see if this schedule is turn on
    {
      targetSchedule->StartFunction();
      targetSchedule->Status = RUNNING; //Set the status to be in progress (ie The start callback has been called, but not the end callback)
      targetSchedule->setIgnitionCompare(targetSchedule->endCompare);
    }
    else if (targetSchedule->Status == RUNNING)
    {
      targetSchedule->EndFunction(); //Moment of spark 
      targetSchedule->Status = OFF; //Turn off the schedule

       //If there is a next schedule queued up, activate it
      if(targetSchedule->hasNextSchedule == true)
      {
        targetSchedule->setIgnitionCompare(targetSchedule->nextStartCompare);
        targetSchedule->endCompare = targetSchedule->nextEndCompare;
        targetSchedule->Status = PENDING;
        targetSchedule->hasNextSchedule = false;
      }
      else {targetSchedule->ignTimerDisable(); }
    }
    else //Safety check. Turn off this output compare unit and return without performing any action
    {
      targetSchedule->EndFunction();
      targetSchedule->ignTimerDisable(); 
      } 
}