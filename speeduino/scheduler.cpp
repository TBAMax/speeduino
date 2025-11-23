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
 * Structures @ref FuelSchedule and @ref IgnitionSchedule describe the scheduling info for Fuel and Ignition respectively.
 * They contain duration, current activity status, start timing, end timing, callbacks to carry out action, etc.
 * 
 * ## Scheduling Functions
 * 
 * - setSchedule*(EndAngle, duration) - **Setup** schedule for (next) pulse on the channel
 * - pStartCallback() - Execute at the **start** of event (Interrupt handler)
 * - pEndCallback() - Execute at the **end** of event (Interrupt handler)
 */
#include "globals.h"
#include "decoders.h"
#include "scheduler.h"
#include "timers.h"
#include "schedule_calcs.h"
#include "utilities.h"
#include "units.h"
#include "crankMaths.h"

FuelSchedule fuelSchedule1(FUEL1_COUNTER, FUEL1_COMPARE); //cppcheck-suppress misra-c2012-8.4
FuelSchedule fuelSchedule2(FUEL2_COUNTER, FUEL2_COMPARE); //cppcheck-suppress misra-c2012-8.4
FuelSchedule fuelSchedule3(FUEL3_COUNTER, FUEL3_COMPARE); //cppcheck-suppress misra-c2012-8.4
FuelSchedule fuelSchedule4(FUEL4_COUNTER, FUEL4_COMPARE); //cppcheck-suppress misra-c2012-8.4
#if (INJ_CHANNELS >= 5)
FuelSchedule fuelSchedule5(FUEL5_COUNTER, FUEL5_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif
#if (INJ_CHANNELS >= 6)
FuelSchedule fuelSchedule6(FUEL6_COUNTER, FUEL6_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif
#if (INJ_CHANNELS >= 7)
FuelSchedule fuelSchedule7(FUEL7_COUNTER, FUEL7_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif
#if (INJ_CHANNELS >= 8)
FuelSchedule fuelSchedule8(FUEL8_COUNTER, FUEL8_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif

IgnitionSchedule ignitionSchedule1(IGN1_COUNTER, IGN1_COMPARE); //cppcheck-suppress misra-c2012-8.4
IgnitionSchedule ignitionSchedule2(IGN2_COUNTER, IGN2_COMPARE); //cppcheck-suppress misra-c2012-8.4
IgnitionSchedule ignitionSchedule3(IGN3_COUNTER, IGN3_COMPARE); //cppcheck-suppress misra-c2012-8.4
IgnitionSchedule ignitionSchedule4(IGN4_COUNTER, IGN4_COMPARE); //cppcheck-suppress misra-c2012-8.4
IgnitionSchedule ignitionSchedule5(IGN5_COUNTER, IGN5_COMPARE); //cppcheck-suppress misra-c2012-8.4
#if IGN_CHANNELS >= 6
IgnitionSchedule ignitionSchedule6(IGN6_COUNTER, IGN6_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif
#if IGN_CHANNELS >= 7
IgnitionSchedule ignitionSchedule7(IGN7_COUNTER, IGN7_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif
#if IGN_CHANNELS >= 8
IgnitionSchedule ignitionSchedule8(IGN8_COUNTER, IGN8_COMPARE); //cppcheck-suppress misra-c2012-8.4
#endif

void Schedule::setSchedule(int16_t EndAngle, unsigned long duration, int crankAngleMax)
{  
  noInterrupts(); // make sure start and end values are updated simultaneously and that schedule do not change state
  //decoder.getLastToothAngle();
  if(toothCurrentCount == 0) { toothCurrentCount = configPage4.triggerTeeth; }
  int16_t LastToothAngle = ((toothCurrentCount - 1) * triggerToothAngle) + configPage4.triggerAngle; //Number of teeth that have passed since tooth 1, multiplied by the angle each tooth represents, plus the angle that tooth 1 is ATDC. This gives accuracy only to the nearest tooth.   
  //Sequential check (simply sets whether we're on the first or 2nd revolution of the cycle)
  if ( (revolutionOne == true) && (configPage4.TrigSpeed == CRANK_SPEED) ) { LastToothAngle += 360; }
  LastToothAngle = nudge(0, crankAngleMax, LastToothAngle, crankAngleMax);

  int16_t deltaAngle = (int16_t)(EndAngle - LastToothAngle);//Calculate angle difference from last tooth seen to pulse end angle   
  if(deltaAngle < 0)// Only allow positive angle difference numbers 
  { 
    deltaAngle += crankAngleMax; //calculate into the next cycle
  }
  uint16_t timeFromLastTooth = (uint16_t)((uint16_t)micros() - toothLastToothTime);//Time since last tooth in uS

  if(Status != RUNNING) //Check that we're not already part way through a schedule
  {
    //Calculate when the injection should end
    uint32_t endTimeout = ((uint16_t)deltaAngle * (uint32_t)microsPerDegree >> microsPerDegree_Shift) - timeFromLastTooth;//time in uS until the injector should be turned off

    if((endTimeout < MAX_TIMER_PERIOD) && (endTimeout > (uint32_t)(duration + SCHEDULE_REFRESH_THRESHOLD))) //Allow for fixed safety between setting the schedule and running it
    {          
      endCompare = (COMPARE_TYPE)_counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(endTimeout)); //As there is a tick every 4uS, there are timeout/4 ticks until the interrupt should be triggered ( >>2 divides by 4)   
      _compare = (COMPARE_TYPE)(endCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration))); // set pulse start Compare value
      Status = PENDING; //Turn this schedule on
    }
  }
  else
  {
    //When the schedule is already running set the next schedule
    //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule(or no time in case of 100% DC)
    if(deltaAngle < crankAngleMax-180 )//Avoid jumping over the cycle, allow for up to 180 degree overlap
    {
      deltaAngle += crankAngleMax;
    } 
    uint32_t nextEndTimeout=((uint16_t)deltaAngle * (uint32_t)microsPerDegree >> microsPerDegree_Shift) - timeFromLastTooth;       
    //use half of the maximum period here because next schedule is really only used at high RPM
    if(nextEndTimeout < MAX_TIMER_PERIOD/2)
    {
      nextEndCompare = _counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(nextEndTimeout));
      nextStartCompare = nextEndCompare - (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration));                
      hasNextSchedule = true;
      //check for overlap of schedules(100% duty cycle case)
      if((COMPARE_TYPE)(nextEndCompare-nextStartCompare)+ (COMPARE_TYPE)uS_TO_TIMER_COMPARE(SCHEDULE_OVERLAP_THRESHOLD) >= (COMPARE_TYPE)(nextEndCompare-endCompare)){
        overlap = true;//interrupts use this
      }
      else
      {
        overlap = false;//interrupts use this
      }             
    }
    else
    {
      hasNextSchedule = false;
    }
  }
  interrupts();
}

//Set schedule to run immediately for duration specified
void Schedule::setSchedule(unsigned long duration)
{
  noInterrupts(); // make sure start and end values are updated simultaneously and that schedule do not change state
  if(Status != RUNNING) //Check that we're not already part way through a schedule
  {
    pStartCallback();    
    _compare = (COMPARE_TYPE)_counter + (COMPARE_TYPE)(uS_TO_TIMER_COMPARE(duration));
    Status = RUNNING; //Turn this schedule on
  }
  interrupts();
}

static table2D_u8_u8_4 PrimingPulseTable(&configPage2.primeBins, &configPage2.primePulse);

/** Perform the injector priming pulses.
 * Set these to run at an arbitrary time in the future (100us).
 * The prime pulse value is in ms*10, so need to multiple by 100 to get to uS
 */
extern void beginInjectorPriming(void)
{
  unsigned long primingValue = table2D_getValue(&PrimingPulseTable, temperatureAddOffset(currentStatus.coolant));
  if( (primingValue > 0) && (currentStatus.TPS <= configPage4.floodClear) )
  {
    primingValue = primingValue * 100 * 5; //to achieve long enough priming pulses, the values in tuner studio are divided by 0.5 instead of 0.1, so multiplier of 5 is required.
    if ( maxInjOutputs >= 1 ) {fuelSchedule1.setSchedule(primingValue); }
#if (INJ_CHANNELS >= 2)
    if ( maxInjOutputs >= 2 ) {fuelSchedule2.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 3)
    if ( maxInjOutputs >= 3 ) {fuelSchedule3.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 4)
    if ( maxInjOutputs >= 4 ) {fuelSchedule4.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 5)
    if ( maxInjOutputs >= 5 ) {fuelSchedule5.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 6)
    if ( maxInjOutputs >= 6 ) {fuelSchedule6.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 7)
    if ( maxInjOutputs >= 7) {fuelSchedule7.setSchedule(primingValue); }
#endif
#if (INJ_CHANNELS >= 8)
    if ( maxInjOutputs >= 8 ) {fuelSchedule8.setSchedule(primingValue); }
#endif
  }
}

// Shared ISR function for all fuel and ignition timers.
// This is completely inlined into the ISR - there is no function call
// overhead.
static inline __attribute__((always_inline)) void scheduleISR(Schedule &schedule)
{
  if (schedule.Status == PENDING) //Check to see if this schedule is turn on
  {
    schedule.pStartCallback();
    schedule.Status = RUNNING; //Set the status to be in progress (ie The start callback has been called, but not the end callback)
    schedule._compare = schedule.endCompare;
  }
  else if (schedule.Status == RUNNING)
  {
    //If there is a next schedule queued up, activate it
    if(schedule.hasNextSchedule == true)
      {      
      if(schedule.overlap == true) //if there is overlap with the last pulse, keep the schedule running
      {
        schedule._compare = schedule.nextEndCompare;  
      }
      else //no overlap
      {
        schedule.pEndCallback();
        schedule._compare = schedule.nextStartCompare;        
        schedule.Status = PENDING;
      }
      schedule.endCompare = schedule.nextEndCompare;
      schedule.hasNextSchedule = false;
    }
    else
    {
      schedule.pEndCallback();
      schedule.Status = OFF; //Turn off the schedule    
    }
  }
  else // if (schedule.Status == OFF) 
  { 
    //no action required
  } 
} 

/*******************************************************************************************************************************************************************************************************/
/** fuelSchedule*Interrupt (All 8 ISR functions below) get called (as timed interrupts) when either the start time or the duration time are reached.
* This calls the relevant callback function (pStartCallback or pEndCallback) depending on the status (PENDING => Needs to run, RUNNING => Needs to stop) of the schedule.
*/
//Timer3A (fuel schedule 1) Compare Vector
#ifdef CORE_AVR //AVR chips use the ISR for this
//fuelSchedules 1 and 5
ISR(TIMER3_COMPA_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule1);
  }


#ifdef CORE_AVR
ISR(TIMER3_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule2);
  }


#ifdef CORE_AVR
ISR(TIMER3_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule3);
  }


#ifdef CORE_AVR
ISR(TIMER4_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule4);
  }

#if INJ_CHANNELS >= 5
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule5);
  }
#endif

#if INJ_CHANNELS >= 6
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule6);
  }
#endif

#if INJ_CHANNELS >= 7
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule7);
  }
#endif

#if INJ_CHANNELS >= 8
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void fuelSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(fuelSchedule8);
  }
#endif

#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPA_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule1Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule1);
  }

#if IGN_CHANNELS >= 2
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule2Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule2);
  }
#endif

#if IGN_CHANNELS >= 3
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule3Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule3);
  }
#endif

#if IGN_CHANNELS >= 4
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule4Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule4);
  }
#endif

#if IGN_CHANNELS >= 5
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule5Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule5);
  }
#endif

#if IGN_CHANNELS >= 6
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule6Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule6);
  }
#endif

#if IGN_CHANNELS >= 7
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule7Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule7);
  }
#endif

#if IGN_CHANNELS >= 8
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //cppcheck-suppress misra-c2012-8.2
#else
void ignitionSchedule8Interrupt(void) //Most ARM chips can simply call a function
#endif
  {
    scheduleISR(ignitionSchedule8);
  }
#endif

void disableFuelSchedule(byte channel)
{
  noInterrupts();
  switch(channel)
  {
    case 0:
      if(fuelSchedule1.Status == PENDING) { fuelSchedule1.Status = OFF; }
      else if(fuelSchedule1.Status == RUNNING) { fuelSchedule1.hasNextSchedule = false; }
      break;
    case 1:
      if(fuelSchedule2.Status == PENDING) { fuelSchedule2.Status = OFF; }
      else if(fuelSchedule2.Status == RUNNING) { fuelSchedule2.hasNextSchedule = false; }
      break;
    case 2: 
      if(fuelSchedule3.Status == PENDING) { fuelSchedule3.Status = OFF; }
      else if(fuelSchedule3.Status == RUNNING) { fuelSchedule3.hasNextSchedule = false; }
      break;
    case 3:
      if(fuelSchedule4.Status == PENDING) { fuelSchedule4.Status = OFF; }
      else if(fuelSchedule4.Status == RUNNING) { fuelSchedule4.hasNextSchedule = false; }
      break;
    case 4:
#if (INJ_CHANNELS >= 5)
      if(fuelSchedule5.Status == PENDING) { fuelSchedule5.Status = OFF; }
      else if(fuelSchedule5.Status == RUNNING) { fuelSchedule5.hasNextSchedule = false; }
#endif
      break;
    case 5:
#if (INJ_CHANNELS >= 6)
      if(fuelSchedule6.Status == PENDING) { fuelSchedule6.Status = OFF; }
      else if(fuelSchedule6.Status == RUNNING) { fuelSchedule6.hasNextSchedule = false; }
#endif
      break;
    case 6:
#if (INJ_CHANNELS >= 7)
      if(fuelSchedule7.Status == PENDING) { fuelSchedule7.Status = OFF; }
      else if(fuelSchedule7.Status == RUNNING) { fuelSchedule7.hasNextSchedule = false; }
#endif
      break;
    case 7:
#if (INJ_CHANNELS >= 8)
      if(fuelSchedule8.Status == PENDING) { fuelSchedule8.Status = OFF; }
      else if(fuelSchedule8.Status == RUNNING) { fuelSchedule8.hasNextSchedule = false; }
#endif
      break;
  }
  interrupts();
}
void disableIgnSchedule(byte channel)
{
  noInterrupts();
  switch(channel)
  {
    case 0:
      if(ignitionSchedule1.Status == PENDING) { ignitionSchedule1.Status = OFF; }
      else if(ignitionSchedule1.Status == RUNNING) { ignitionSchedule1.hasNextSchedule = false; }
      break;
    case 1:
      if(ignitionSchedule2.Status == PENDING) { ignitionSchedule2.Status = OFF; }
      else if(ignitionSchedule2.Status == RUNNING) { ignitionSchedule2.hasNextSchedule = false; }
      break;
    case 2: 
      if(ignitionSchedule3.Status == PENDING) { ignitionSchedule3.Status = OFF; }
      else if(ignitionSchedule3.Status == RUNNING) { ignitionSchedule3.hasNextSchedule = false; }
      break;
    case 3:
      if(ignitionSchedule4.Status == PENDING) { ignitionSchedule4.Status = OFF; }
      else if(ignitionSchedule4.Status == RUNNING) { ignitionSchedule4.hasNextSchedule = false; }
      break;
    case 4:
      if(ignitionSchedule5.Status == PENDING) { ignitionSchedule5.Status = OFF; }
      else if(ignitionSchedule5.Status == RUNNING) { ignitionSchedule5.hasNextSchedule = false; }
      break;
#if IGN_CHANNELS >= 6      
    case 5:
      if(ignitionSchedule6.Status == PENDING) { ignitionSchedule6.Status = OFF; }
      else if(ignitionSchedule6.Status == RUNNING) { ignitionSchedule6.hasNextSchedule = false; }
      break;
#endif
#if IGN_CHANNELS >= 7      
    case 6:
      if(ignitionSchedule7.Status == PENDING) { ignitionSchedule7.Status = OFF; }
      else if(ignitionSchedule7.Status == RUNNING) { ignitionSchedule7.hasNextSchedule = false; }
      break;
#endif
#if IGN_CHANNELS >= 8      
    case 7:
      if(ignitionSchedule8.Status == PENDING) { ignitionSchedule8.Status = OFF; }
      else if(ignitionSchedule8.Status == RUNNING) { ignitionSchedule8.hasNextSchedule = false; }
      break;
#endif
  }
  interrupts();
}

void disableAllFuelSchedules()
{
  disableFuelSchedule(0);
  disableFuelSchedule(1);
  disableFuelSchedule(2);
  disableFuelSchedule(3);
  disableFuelSchedule(4);
  disableFuelSchedule(5);
  disableFuelSchedule(6);
  disableFuelSchedule(7);
}
void disableAllIgnSchedules()
{
  disableIgnSchedule(0);
  disableIgnSchedule(1);
  disableIgnSchedule(2);
  disableIgnSchedule(3);
  disableIgnSchedule(4);
  disableIgnSchedule(5);
  disableIgnSchedule(6);
  disableIgnSchedule(7);
}
  void Schedule::setCallbacks(voidVoidCallback pStartCallback, voidVoidCallback pEndCallback)
{
  ATOMIC()
  {
  if(Status==RUNNING)
  {
    pEndCallback();
    Status=OFF;
  }
  pStartCallback = pStartCallback;
  pEndCallback = pEndCallback;
  }
}

//Start all schedulers. Timers are always running, even if schedule is OFF. Timers are defined in board_*.h. 
void initialiseSchedulers(void){
  IGN1_TIMER_ENABLE();
  IGN2_TIMER_ENABLE();
  IGN3_TIMER_ENABLE();
  IGN4_TIMER_ENABLE();  
  IGN5_TIMER_ENABLE();
#if IGN_CHANNELS >= 6
  IGN6_TIMER_ENABLE();
#endif
#if IGN_CHANNELS >= 7
  IGN7_TIMER_ENABLE();
#endif
#if IGN_CHANNELS >= 8
  IGN8_TIMER_ENABLE();
#endif

  FUEL1_TIMER_ENABLE();
  FUEL2_TIMER_ENABLE();
  FUEL3_TIMER_ENABLE();
  FUEL4_TIMER_ENABLE();
#if INJ_CHANNELS >= 5
  FUEL5_TIMER_ENABLE();
#endif
#if INJ_CHANNELS >= 6
  FUEL6_TIMER_ENABLE();
#endif
#if INJ_CHANNELS >= 7
  FUEL7_TIMER_ENABLE();
#endif
#if INJ_CHANNELS >= 8
  FUEL8_TIMER_ENABLE();
#endif
}