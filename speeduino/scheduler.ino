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
#include "timers.h"

static void fun_FUEL1_TIMER_DISABLE() { FUEL1_TIMER_DISABLE(); }
static void fun_FUEL1_TIMER_ENABLE() { FUEL1_TIMER_ENABLE(); }
FuelSchedule fuelSchedule1(FUEL1_COUNTER, FUEL1_COMPARE, fun_FUEL1_TIMER_DISABLE, fun_FUEL1_TIMER_ENABLE);

static void fun_FUEL2_TIMER_DISABLE() { FUEL2_TIMER_DISABLE(); }
static void fun_FUEL2_TIMER_ENABLE() { FUEL2_TIMER_ENABLE(); }
FuelSchedule fuelSchedule2(FUEL2_COUNTER, FUEL2_COMPARE, fun_FUEL2_TIMER_DISABLE, fun_FUEL2_TIMER_ENABLE);

static void fun_FUEL3_TIMER_DISABLE() { FUEL3_TIMER_DISABLE(); }
static void fun_FUEL3_TIMER_ENABLE() { FUEL3_TIMER_ENABLE(); }
FuelSchedule fuelSchedule3(FUEL3_COUNTER, FUEL3_COMPARE, fun_FUEL3_TIMER_DISABLE, fun_FUEL3_TIMER_ENABLE);

static void fun_FUEL4_TIMER_DISABLE() { FUEL4_TIMER_DISABLE(); }
static void fun_FUEL4_TIMER_ENABLE() { FUEL4_TIMER_ENABLE(); }
FuelSchedule fuelSchedule4(FUEL4_COUNTER, FUEL4_COMPARE, fun_FUEL4_TIMER_DISABLE, fun_FUEL4_TIMER_ENABLE);

#if (INJ_CHANNELS >= 5)
static void fun_FUEL5_TIMER_DISABLE() { FUEL5_TIMER_DISABLE(); }
static void fun_FUEL5_TIMER_ENABLE() { FUEL5_TIMER_ENABLE(); }
FuelSchedule fuelSchedule5(FUEL5_COUNTER, FUEL5_COMPARE, fun_FUEL5_TIMER_DISABLE, fun_FUEL5_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 6)
static void fun_FUEL6_TIMER_DISABLE() { FUEL6_TIMER_DISABLE(); }
static void fun_FUEL6_TIMER_ENABLE() { FUEL6_TIMER_ENABLE(); }
FuelSchedule fuelSchedule6(FUEL6_COUNTER, FUEL6_COMPARE, fun_FUEL6_TIMER_DISABLE, fun_FUEL6_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 7)
static void fun_FUEL7_TIMER_DISABLE() { FUEL7_TIMER_DISABLE(); }
static void fun_FUEL7_TIMER_ENABLE() { FUEL7_TIMER_ENABLE(); }
FuelSchedule fuelSchedule7(FUEL7_COUNTER, FUEL7_COMPARE, fun_FUEL7_TIMER_DISABLE, fun_FUEL7_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 8)
static void fun_FUEL8_TIMER_DISABLE() { FUEL8_TIMER_DISABLE(); }
static void fun_FUEL8_TIMER_ENABLE() { FUEL8_TIMER_ENABLE(); }
FuelSchedule fuelSchedule8(FUEL8_COUNTER, FUEL8_COMPARE, fun_FUEL8_TIMER_DISABLE, fun_FUEL8_TIMER_ENABLE);
#endif

static void fun_IGN1_TIMER_DISABLE() { IGN1_TIMER_DISABLE(); }
static void fun_IGN1_TIMER_ENABLE() { IGN1_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule1(IGN1_COUNTER, IGN1_COMPARE, fun_IGN1_TIMER_DISABLE, fun_IGN1_TIMER_ENABLE);

static void fun_IGN2_TIMER_DISABLE() { IGN2_TIMER_DISABLE(); }
static void fun_IGN2_TIMER_ENABLE() { IGN2_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule2(IGN2_COUNTER, IGN2_COMPARE, fun_IGN2_TIMER_DISABLE, fun_IGN2_TIMER_ENABLE);

static void fun_IGN3_TIMER_DISABLE() { IGN3_TIMER_DISABLE(); }
static void fun_IGN3_TIMER_ENABLE() { IGN3_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule3(IGN3_COUNTER, IGN3_COMPARE, fun_IGN3_TIMER_DISABLE, fun_IGN3_TIMER_ENABLE);

static void fun_IGN4_TIMER_DISABLE() { IGN4_TIMER_DISABLE(); }
static void fun_IGN4_TIMER_ENABLE() { IGN4_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule4(IGN4_COUNTER, IGN4_COMPARE, fun_IGN4_TIMER_DISABLE, fun_IGN4_TIMER_ENABLE);

static void fun_IGN5_TIMER_DISABLE() { IGN5_TIMER_DISABLE(); }
static void fun_IGN5_TIMER_ENABLE() { IGN5_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule5(IGN5_COUNTER, IGN5_COMPARE, fun_IGN5_TIMER_DISABLE, fun_IGN5_TIMER_ENABLE);

#if IGN_CHANNELS >= 6
static void fun_IGN6_TIMER_DISABLE() { IGN6_TIMER_DISABLE(); }
static void fun_IGN6_TIMER_ENABLE() { IGN6_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule6(IGN6_COUNTER, IGN6_COMPARE, fun_IGN6_TIMER_DISABLE, fun_IGN6_TIMER_ENABLE);
#endif
#if IGN_CHANNELS >= 7
static void fun_IGN7_TIMER_DISABLE() { IGN7_TIMER_DISABLE(); }
static void fun_IGN7_TIMER_ENABLE() { IGN7_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule7(IGN7_COUNTER, IGN7_COMPARE, fun_IGN7_TIMER_DISABLE, fun_IGN7_TIMER_ENABLE);
#endif
#if IGN_CHANNELS >= 8
static void fun_IGN8_TIMER_DISABLE() { IGN8_TIMER_DISABLE(); }
static void fun_IGN8_TIMER_ENABLE() { IGN8_TIMER_ENABLE(); }
IgnSchedule ignitionSchedule8(IGN8_COUNTER, IGN8_COMPARE, fun_IGN8_TIMER_DISABLE, fun_IGN8_TIMER_ENABLE);
#endif

void initialiseSchedulers()
{
    //nullSchedule.Status = OFF;    
    fuelSchedule1.Status = OFF;    
    fuelSchedule2.Status = OFF;    
    fuelSchedule3.Status = OFF;
    fuelSchedule4.Status = OFF;  
    #if (INJ_CHANNELS >= 5)
    fuelSchedule5.Status = OFF;
    #endif 
    #if (INJ_CHANNELS >= 6)
    fuelSchedule6.Status = OFF;
    #endif
    #if (INJ_CHANNELS >= 7)
    fuelSchedule7.Status = OFF;
    #endif
    #if (INJ_CHANNELS >= 8)
    fuelSchedule8.Status = OFF;
    #endif      
    
    ignitionSchedule1.Status = OFF;  
    ignitionSchedule2.Status = OFF; 
    ignitionSchedule3.Status = OFF; 
    ignitionSchedule4.Status = OFF;
    ignitionSchedule5.Status = OFF;
    #if IGN_CHANNELS >= 6
    ignitionSchedule6.Status = OFF; 
    #endif
    #if IGN_CHANNELS >= 7
    ignitionSchedule7.Status = OFF;
    #endif
    #if IGN_CHANNELS >= 8
    ignitionSchedule8.Status = OFF; 
    #endif 
}

/*
New generic function.
*/
void setFuelSchedule (struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration)
{
  while (injectorEndAngle <= crankAngle)   { injectorEndAngle += CRANK_ANGLE_MAX_INJ; } //calculate into the next cycle
  if (isRunning(*targetSchedule))
  {
    //If the schedule is already running, we can set the next schedule so it is ready to go
    //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
    injectorEndAngle += CRANK_ANGLE_MAX_INJ;
  }
  unsigned long timeout = (injectorEndAngle - crankAngle) * (unsigned long)timePerDegree; 
  setFuelSchedule(targetSchedule, timeout, duration);
}

void setFuelSchedule(struct FuelSchedule *targetSchedule, unsigned long timeout, unsigned long duration)
{
  //Time in uS that the refresh functions will check to ensure there is enough time before changing the start or end compare
  constexpr uint8_t INJECTION_REFRESH_TRESHOLD = 230U; 

  if (!isRunning(*targetSchedule)) //Check that we're not already part way through a schedule
  {
    if((timeout < MAX_TIMER_PERIOD) && (timeout > duration + INJECTION_REFRESH_TRESHOLD)) //Need to check that the timeout doesn't exceed the overflow, also allow for fixed 230us safety between setting the schedule and running it
    {      
      noInterrupts(); // make sure start and end values are updated simultaneously
      targetSchedule->endCompare = targetSchedule->counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout)); //As there is a tick every 4uS, there are timeout/4 ticks until the interrupt should be triggered ( >>2 divides by 4)   
      SET_COMPARE(targetSchedule->compare, targetSchedule->endCompare - uS_TO_TIMER_COMPARE(duration)); // previously startCompare
      targetSchedule->Status = PENDING; //Turn this schedule on
      interrupts(); 
      targetSchedule->pTimerEnable();
    }
  }
  else 
  {
    if((timeout < MAX_TIMER_PERIOD) && (timeout > duration + INJECTION_REFRESH_TRESHOLD)&&((COMPARE_TYPE)(targetSchedule->endCompare-targetSchedule->counter)>400U))
    {
      noInterrupts();
      targetSchedule->nextEndCompare = targetSchedule->counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout));
      targetSchedule->nextStartCompare = targetSchedule->nextEndCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration));
      targetSchedule->Status = RUNNINGHASNEXT;
      interrupts();
    }
  }
}

//separate function for setting the fuel schedules at priming
void setFuelSchedule (struct FuelSchedule *targetSchedule, unsigned long duration)
{
  if(!isRunning(*targetSchedule)) //Check that we're not already part way through a schedule
  {      
      targetSchedule->pStartFunction();
      noInterrupts(); // make sure start and end values are updated simultaneously
      SET_COMPARE(targetSchedule->compare, targetSchedule->counter + (COMPARE_TYPE)uS_TO_TIMER_COMPARE(duration));
      targetSchedule->Status = RUNNING; //RUN this schedule immediately
      interrupts(); 
      targetSchedule->pTimerEnable();
  }
}

//New generic function
void setIgnitionSchedule(struct IgnSchedule *targetSchedule ,  int16_t crankAngle, int ignitionEndAngle, unsigned long duration)
{
  while (ignitionEndAngle <= crankAngle)   { ignitionEndAngle += CRANK_ANGLE_MAX_IGN; } //calculate into the next cycle
  if (isRunning(*targetSchedule))
  {
    //If the schedule is already running, we can set the next schedule so it is ready to go
    //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
    ignitionEndAngle += CRANK_ANGLE_MAX_IGN;
  }
  unsigned long timeout = angleToTime((ignitionEndAngle - crankAngle), CRANKMATH_METHOD_INTERVAL_REV);
  setIgnitionSchedule(targetSchedule, timeout, duration);
}

void setIgnitionSchedule(struct IgnSchedule *targetSchedule , unsigned long timeout, unsigned long duration)
{
  constexpr COMPARE_TYPE IGNITION_REFRESH_THRESHOLD = 230U; //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare

  if (!isRunning(*targetSchedule)) //Check that we're not already part way through a schedule
  {
    if((timeout < MAX_TIMER_PERIOD) && (timeout > duration + IGNITION_REFRESH_THRESHOLD)) //Need to check that the timeout doesn't exceed the overflow, also allow for fixed 230us safety between setting the schedule and running it
    {      
      noInterrupts(); // make sure start and end values are updated simultaneously
      targetSchedule->endCompare =  (COMPARE_TYPE)targetSchedule->counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout)); //As there is a tick every 4uS, there are timeout/4 ticks until the interrupt should be triggered ( >>2 divides by 4)   
      SET_COMPARE(targetSchedule->compare, (targetSchedule->endCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration)))); // previously startCompare
      targetSchedule->Status = PENDING; //Turn this schedule on
      interrupts(); 
      targetSchedule->pTimerEnable();
    }
  }
  else 
  {
    if(timeout < MAX_TIMER_PERIOD)
    {
      noInterrupts();
      targetSchedule->nextEndCompare = (COMPARE_TYPE)targetSchedule->counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(timeout));
      targetSchedule->nextStartCompare = targetSchedule->nextEndCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration)); 
      targetSchedule->Status = RUNNINGHASNEXT;
      interrupts();
    }
  }
}

//overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition
void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule)
{            
  ignitionSchedule->pStartFunction(); //start coil charging
  SET_COMPARE(ignitionSchedule->compare, (COMPARE_TYPE)ignitionSchedule->counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(currentStatus.dwell)));
  ignitionSchedule->Status = RUNNING;
  ignitionSchedule->pTimerEnable();
}

void beginInjectorPriming()
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


static void fuelScheduleInterrupt(struct FuelSchedule *fuelSchedule)
{
  constexpr uint8_t INJECTION_OVERLAP_TRESHOLD  = 96U; //Time in us, basically minimum injector off time that is allowed.

  if (isPending(*fuelSchedule)) //Check to see if this schedule is turn on
  {
    SET_COMPARE(fuelSchedule->compare, fuelSchedule->endCompare);
    fuelSchedule->pStartFunction();
    fuelSchedule->Status = RUNNING; //Set the status to be in progress (ie The start callback has been called, but not the end callback)    
  }
  else if (isRunning(*fuelSchedule))
  {
    //If there is a next schedule queued up, activate it
    if(fuelSchedule->Status == RUNNINGHASNEXT)
    {
      if((fuelSchedule->nextEndCompare-fuelSchedule->nextStartCompare)+ uS_TO_TIMER_COMPARE(INJECTION_OVERLAP_TRESHOLD)>=(fuelSchedule->nextEndCompare-fuelSchedule->endCompare)) //check for possible overlap
      {
        SET_COMPARE(fuelSchedule->compare, fuelSchedule->nextEndCompare);
        fuelSchedule->endCompare = fuelSchedule->nextEndCompare;
        fuelSchedule->Status = RUNNING;
      }
      else //no overlap
      {
        fuelSchedule->pEndFunction();
        SET_COMPARE(fuelSchedule->compare, fuelSchedule->nextStartCompare);
        fuelSchedule->endCompare = fuelSchedule->nextEndCompare;
        fuelSchedule->Status = PENDING;
      }
    }
    else //no next schedule
    {
      fuelSchedule->pEndFunction();
      fuelSchedule->Status = OFF; //Turn off the schedule        
    }
  }
  else //(fuelSchedule->Status == OFF)
  {
    fuelSchedule->pEndFunction();
    fuelSchedule->pTimerDisable(); //Safety check. Turn off this output compare unit and return without performing any action
  } 
}

//Timer3A (fuel schedule 1) Compare Vector
#if (INJ_CHANNELS >= 1)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPA_vect) //fuelSchedules 1 and 5
#else
void fuelSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule1);
}
#endif

#if (INJ_CHANNELS >= 2)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //fuelSchedule2
#else
void fuelSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule2);
}
#endif

#if (INJ_CHANNELS >= 3)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //fuelSchedule3
#else
void fuelSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule3);
}
#endif

#if (INJ_CHANNELS >= 4)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //fuelSchedule4
#else
void fuelSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule4);
}
#endif

#if (INJ_CHANNELS >= 5)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //fuelSchedule5
#else
void fuelSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule5);
}
#endif

#if (INJ_CHANNELS >= 6)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //fuelSchedule6
#else
void fuelSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule6);
}
#endif

#if (INJ_CHANNELS >= 7)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //fuelSchedule7
#else
void fuelSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule7);
}
#endif

#if (INJ_CHANNELS >= 8)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //fuelSchedule8
#else
void fuelSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
fuelScheduleInterrupt(&fuelSchedule8);
}
#endif


static void ignitionScheduleInterrupt(struct IgnSchedule *targetSchedule) // common function that all ignition channel interrupts use
{
  if (isPending(*targetSchedule)) //Check to see if this schedule is turn on
  {
    targetSchedule->pStartFunction();
    targetSchedule->Status = RUNNING; //Set the status to be in progress (ie The start callback has been called, but not the end callback)
    SET_COMPARE(targetSchedule->compare, targetSchedule->endCompare);
  }
  else if (isRunning(*targetSchedule))
  {
    targetSchedule->pEndFunction(); //Moment of spark 

      //If there is a next schedule queued up, activate it
    if(targetSchedule->Status == RUNNINGHASNEXT)
    {
      SET_COMPARE(targetSchedule->compare, targetSchedule->nextStartCompare);
      targetSchedule->endCompare = targetSchedule->nextEndCompare;
      targetSchedule->Status = PENDING;
    }
    else
    {
      targetSchedule->pTimerDisable();
      targetSchedule->Status = OFF; //Turn off the schedule
    }
  }
  else //Safety check. Turn off this output compare unit and return without performing any action
  {
    targetSchedule->pEndFunction();
    targetSchedule->pTimerDisable(); 
  } 
}

#if IGN_CHANNELS >= 1
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPA_vect) //ignitionSchedule1
#else
void ignitionSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule1);
}
#endif

#if IGN_CHANNELS >= 2
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //ignitionSchedule2
#else
void ignitionSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule2);
}
#endif

#if IGN_CHANNELS >= 3
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //ignitionSchedule3
#else
void ignitionSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule3);
}
#endif

#if IGN_CHANNELS >= 4
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //ignitionSchedule4
#else
void ignitionSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule4);
}
#endif

#if IGN_CHANNELS >= 5
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //ignitionSchedule5
#else
void ignitionSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule5);
}
#endif

#if IGN_CHANNELS >= 6
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //ignitionSchedule6
#else
void ignitionSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule6);
}
#endif

#if IGN_CHANNELS >= 7
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //ignitionSchedule6
#else
void ignitionSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule7);
}
#endif

#if IGN_CHANNELS >= 8
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //ignitionSchedule8
#else
void ignitionSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
ignitionScheduleInterrupt(&ignitionSchedule8);
}
#endif
