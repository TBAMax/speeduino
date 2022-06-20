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

Action::Action()
 : Action(nullCallback, nullCallback)
 {
 }

void Schedule::reset()
{
  timer.stop();
  action.stop();

  Status = OFF;
  endCounter = 0;
  nextStartCounter = 0;
  nextEndCounter = 0;

  action = Action();
}

// Immediately run the schedule if not already running.
void Schedule::runAction (unsigned long duration)
{
  if(!isRunning()) //Check that we're not already part way through a schedule
  {      
    forceRunAction(duration);
  }
}

// Immediately run the schedule - regardless of current state.
void Schedule::forceRunAction(unsigned long duration)
{
  action.start();
  noInterrupts(); // make sure start and end values are updated simultaneously
  SET_COMPARE(compare, counter + (COMPARE_TYPE)uS_TO_TIMER_COMPARE(duration));
  Status = RUNNING; //RUN this schedule immediately
  interrupts(); 
  timer.start(); 
}

// Setup the schedule to run on the next cycle
inline void Schedule::beginScheduleInternal(unsigned long totalDuration, unsigned long runDuration)
{
  noInterrupts(); // make sure start and end values are updated simultaneously
  SET_COMPARE(compare, counter + uS_TO_TIMER_COMPARE(totalDuration-runDuration));
  endCounter =         counter + uS_TO_TIMER_COMPARE(totalDuration);
  Status = PENDING; //Turn this schedule on
  interrupts(); 
  timer.start();
}

// Setup the schedule to run after the current cycle
inline void Schedule::queueScheduleInternal(unsigned long totalDuration, unsigned long runDuration)
{
  noInterrupts();
  nextStartCounter = counter + uS_TO_TIMER_COMPARE(totalDuration-runDuration);
  nextEndCounter =   counter + uS_TO_TIMER_COMPARE(totalDuration);
  Status = RUNNINGHASNEXT;
  interrupts();
}

// Set the schedule
Schedule::scheduleResult Schedule::beginSchedule(unsigned long totalDuration, unsigned long actionDuration)
{
// Need to check that the timeout doesn't exceed the overflow, also allow for fixed 230us safety between setting the schedule and running it
  if (totalDuration < MAX_TIMER_PERIOD)
  {
    if (!isRunning()) //Check that we're not already part way through a schedule
    {
      beginScheduleInternal(totalDuration, actionDuration);
      return STARTED;
    }
    else
    {
      queueScheduleInternal(totalDuration, actionDuration);
      return QUEUED;
    }
  }
  return BADDURATION;
}

inline bool Schedule::queuedScheduleOverlaps(COMPARE_TYPE overlapThreshold) const
{
  return (nextEndCounter-nextStartCounter)+uS_TO_TIMER_COMPARE(overlapThreshold)>=(nextEndCounter-endCounter);
}

void Schedule::moveToNextState(bool continueAction, COMPARE_TYPE overlapThreshold)
{
  switch (Status)
  {
    // Wait period has ended, so start the action & wait for it to finish
    case PENDING:
      SET_COMPARE(compare, endCounter);
      action.start();
      Status = RUNNING;
      break;

    // Action duration has expired & there is a next schedule
    case RUNNINGHASNEXT:
      if (continueAction && queuedScheduleOverlaps(overlapThreshold))
      {
        // Overlap and action can be continued, so start the next schedule
        SET_COMPARE(compare, nextEndCounter);
        endCounter = nextEndCounter;
        Status = RUNNING;
      }
      else
      {
        // No overlap, so stop the current action and begin the next schedule
        action.stop();
        SET_COMPARE(compare, nextStartCounter);
        endCounter = nextEndCounter;
        Status = PENDING;
      }
      break;

    case RUNNING:
    case OFF:
    default:
      action.stop();
      timer.stop();
      Status = OFF;
      break;
  }
}

void Schedule::adjustEndTime(COMPARE_TYPE ticksFromNow)
{
  if( isRunning() ) 
  { 
    SET_COMPARE(compare, counter + ticksFromNow); 
  }
  else
  { 
    endCounter = (COMPARE_TYPE)(counter + ticksFromNow); 
  }
}
 
FuelSchedule fuelSchedule1(FUEL1_COUNTER, FUEL1_COMPARE, FUEL1_TIMER_DISABLE, FUEL1_TIMER_ENABLE);
FuelSchedule fuelSchedule2(FUEL2_COUNTER, FUEL2_COMPARE, FUEL2_TIMER_DISABLE, FUEL2_TIMER_ENABLE);
FuelSchedule fuelSchedule3(FUEL3_COUNTER, FUEL3_COMPARE, FUEL3_TIMER_DISABLE, FUEL3_TIMER_ENABLE);
FuelSchedule fuelSchedule4(FUEL4_COUNTER, FUEL4_COMPARE, FUEL4_TIMER_DISABLE, FUEL4_TIMER_ENABLE);
#if (INJ_CHANNELS >= 5)
FuelSchedule fuelSchedule5(FUEL5_COUNTER, FUEL5_COMPARE, FUEL5_TIMER_DISABLE, FUEL5_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 6)
FuelSchedule fuelSchedule6(FUEL6_COUNTER, FUEL6_COMPARE, FUEL6_TIMER_DISABLE, FUEL6_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 7)
FuelSchedule fuelSchedule7(FUEL7_COUNTER, FUEL7_COMPARE, FUEL7_TIMER_DISABLE, FUEL7_TIMER_ENABLE);
#endif
#if (INJ_CHANNELS >= 8)
FuelSchedule fuelSchedule8(FUEL8_COUNTER, FUEL8_COMPARE, FUEL8_TIMER_DISABLE, FUEL8_TIMER_ENABLE);
#endif

IgnSchedule ignitionSchedule1(IGN1_COUNTER, IGN1_COMPARE, IGN1_TIMER_DISABLE, IGN1_TIMER_ENABLE);
IgnSchedule ignitionSchedule2(IGN2_COUNTER, IGN2_COMPARE, IGN2_TIMER_DISABLE, IGN2_TIMER_ENABLE);
IgnSchedule ignitionSchedule3(IGN3_COUNTER, IGN3_COMPARE, IGN3_TIMER_DISABLE, IGN3_TIMER_ENABLE);
IgnSchedule ignitionSchedule4(IGN4_COUNTER, IGN4_COMPARE, IGN4_TIMER_DISABLE, IGN4_TIMER_ENABLE);
IgnSchedule ignitionSchedule5(IGN5_COUNTER, IGN5_COMPARE, IGN5_TIMER_DISABLE, IGN5_TIMER_ENABLE);
#if IGN_CHANNELS >= 6
IgnSchedule ignitionSchedule6(IGN6_COUNTER, IGN6_COMPARE, IGN6_TIMER_DISABLE, IGN6_TIMER_ENABLE);
#endif
#if IGN_CHANNELS >= 7
IgnSchedule ignitionSchedule7(IGN7_COUNTER, IGN7_COMPARE, IGN7_TIMER_DISABLE, IGN7_TIMER_ENABLE);
#endif
#if IGN_CHANNELS >= 8
IgnSchedule ignitionSchedule8(IGN8_COUNTER, IGN8_COMPARE, IGN8_TIMER_DISABLE, IGN8_TIMER_ENABLE);
#endif


Schedule::scheduleResult FuelSchedule::setFuelSchedule (int16_t crankAngle, int16_t injectorEndAngle, unsigned long openDuration)
{
  // Time in uS that the refresh functions will check to ensure there is enough time before changing the start or end compare
  constexpr uint8_t INJECTION_REFRESH_THRESHOLD = 230U; 

  // The current injection pulse must have at least 400 ticks left.
  if (isRunning() && (compare-counter)<400U)
  {
    return BADDURATION;
  }
  
  // Calculate into the next cycle
  while (injectorEndAngle <= crankAngle)   { injectorEndAngle += CRANK_ANGLE_MAX_INJ; } 

  //If the schedule is already running, we can set the next schedule so it is ready to go
  //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
  if (isRunning())
  {
    injectorEndAngle += CRANK_ANGLE_MAX_INJ;
  }
  unsigned long totalDuration = (injectorEndAngle - crankAngle) * (unsigned long)timePerDegree; 

  // If the total duration is less than the minimum duration, then we can't run the schedule
  if (totalDuration < openDuration + INJECTION_REFRESH_THRESHOLD)
  {
    return BADDURATION;
  }

  return beginSchedule(totalDuration, openDuration);
}


Schedule::scheduleResult IgnSchedule::setIgnitionSchedule(int16_t crankAngle, unsigned long coilChargeDuration)
{
  constexpr COMPARE_TYPE IGNITION_REFRESH_THRESHOLD = 230U; //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare

  //calculate into the next cycle
  int endAngle = ignitionEndAngle; // We don't want to change the ignitionEndAngle member variable
  while (endAngle <= crankAngle)   { endAngle += CRANK_ANGLE_MAX_IGN; }

  //If the schedule is already running, we can set the next schedule so it is ready to go
  //This is required in cases of high rpm and high DC where there otherwise would not be enough time to set the schedule
  if (isRunning())
  {
    endAngle += CRANK_ANGLE_MAX_IGN;
  }
  unsigned long totalDuration = angleToTime((endAngle - crankAngle), CRANKMATH_METHOD_INTERVAL_REV);
  
  // If the total duration is less than the minimum duration, then we can't run the schedule
  if (totalDuration < coilChargeDuration + IGNITION_REFRESH_THRESHOLD)
  {
    return BADDURATION;
  }

  return beginSchedule(totalDuration, coilChargeDuration);
}


void beginInjectorPriming()
{
  unsigned long primingValue = table2D_getValue(&PrimingPulseTable, currentStatus.coolant + CALIBRATION_TEMPERATURE_OFFSET);
  if( (primingValue > 0) && (currentStatus.TPS < configPage4.floodClear) )
  {
    primingValue = primingValue * 100 * 5; //to acheive long enough priming pulses, the values in tuner studio are divided by 0.5 instead of 0.1, so multiplier of 5 is required.
    if ( fuelSchedule1.injEnabled == true ) { fuelSchedule1.runAction(primingValue); }
#if (INJ_CHANNELS >= 2)
    if ( fuelSchedule2.injEnabled == true ) { fuelSchedule2.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 3)
    if ( fuelSchedule3.injEnabled == true ) { fuelSchedule3.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 4)
    if ( fuelSchedule4.injEnabled == true ) { fuelSchedule4.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 5)
    if ( fuelSchedule5.injEnabled == true ) { fuelSchedule5.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 6)
    if ( fuelSchedule6.injEnabled == true ) { fuelSchedule6.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 7)
    if ( fuelSchedule7.injEnabled == true ) { fuelSchedule7.runAction(primingValue); }
#endif
#if (INJ_CHANNELS >= 8)
    if ( fuelSchedule8.injEnabled == true ) { fuelSchedule8.runAction(primingValue); }
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
void fuelSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule1.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 2)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //fuelSchedule2
#else
void fuelSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule2.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 3)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //fuelSchedule3
#else
void fuelSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule3.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 4)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //fuelSchedule4
#else
void fuelSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule4.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 5)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //fuelSchedule5
#else
void fuelSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule5.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 6)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //fuelSchedule6
#else
void fuelSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule6.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 7)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //fuelSchedule7
#else
void fuelSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule7.timerCallback();
}
#endif

#if (INJ_CHANNELS >= 8)
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //fuelSchedule8
#else
void fuelSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
  fuelSchedule8.timerCallback();
}
#endif

#if IGN_CHANNELS >= 1
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPA_vect) //ignitionSchedule1
#else
void ignitionSchedule1Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule1.timerCallback();
}
#endif

#if IGN_CHANNELS >= 2
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPB_vect) //ignitionSchedule2
#else
void ignitionSchedule2Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule2.timerCallback();
}
#endif

#if IGN_CHANNELS >= 3
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER5_COMPC_vect) //ignitionSchedule3
#else
void ignitionSchedule3Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule3.timerCallback();
}
#endif

#if IGN_CHANNELS >= 4
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPA_vect) //ignitionSchedule4
#else
void ignitionSchedule4Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule4.timerCallback();
}
#endif

#if IGN_CHANNELS >= 5
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPC_vect) //ignitionSchedule5
#else
void ignitionSchedule5Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule5.timerCallback();
}
#endif

#if IGN_CHANNELS >= 6
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER4_COMPB_vect) //ignitionSchedule6
#else
void ignitionSchedule6Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule6.timerCallback();
}
#endif

#if IGN_CHANNELS >= 7
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPC_vect) //ignitionSchedule6
#else
void ignitionSchedule7Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule7.timerCallback();
}
#endif

#if IGN_CHANNELS >= 8
#if defined(CORE_AVR) //AVR chips use the ISR for this
ISR(TIMER3_COMPB_vect) //ignitionSchedule8
#else
void ignitionSchedule8Interrupt() //Most ARM chips can simply call a function
#endif
{
  ignitionSchedule8.timerCallback();
}
#endif
