/** @file
Injector and Ignition (on/off) scheduling (structs).

This scheduler is designed to maintain 2 schedules for use by the fuel and ignition systems.
It functions by waiting for the overflow vectors from each of the timers in use to overflow, which triggers an interrupt.

## Technical

Currently prescaling the 16-bit timers to 64 for injection and 64 for ignition.
This means that the counter increments every 4us (injection) / 4uS (ignition) and will overflow every 262140uS.

    Max Period = (Prescale)*(1/Frequency)*(2^17)

For more details see https://playground.arduino.cc/Code/Timer1/ (OLD: http://playground.arduino.cc/code/timer1 ).
This means that the precision of the scheduler is:

- 4uS for fuel
- 4uS (+/- 2uS) for ignition

## Features

This differs from most other schedulers in that its calls are non-recurring (ie when you schedule an event at a certain time and once it has occurred,
it will not reoccur unless you explicitly ask/re-register for it).

## Timer identification

Arduino timers usage for injection and ignition schedules:
- timer3 is used for schedule 1(?) (fuel 1,2,3,4 ign 7,8)
- timer4 is used for schedule 2(?) (fuel 5,6 ign 4,5,6)
- timer5 is used ... (fuel 7,8, ign 1,2,3)

Timers 3,4 and 5 are 16-bit timers (ie count to 65536).
See page 136 of the processors datasheet: http://www.atmel.com/Images/doc2549.pdf .

64 prescale gives tick every 4uS.
64 prescale gives overflow every 262140uS (This means maximum wait time is 0.26214 seconds).

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "globals.h"

#if defined(ARDUINO_ARCH_STM32) || defined(CORE_TEENSY)
/*! \name The ARM cores need these declarations so they can hook up the interrupt vectors
 * in their boardInit() function.
 */
/**@{*/ 
  void fuelSchedule1Interrupt();
  void fuelSchedule2Interrupt();
  void fuelSchedule3Interrupt();
  void fuelSchedule4Interrupt();
#if (INJ_CHANNELS >= 5)
  void fuelSchedule5Interrupt();
#endif
#if (INJ_CHANNELS >= 6)
  void fuelSchedule6Interrupt();
#endif
#if (INJ_CHANNELS >= 7)
  void fuelSchedule7Interrupt();
#endif
#if (INJ_CHANNELS >= 8)
  void fuelSchedule8Interrupt();
#endif
#if (IGN_CHANNELS >= 1)
  void ignitionSchedule1Interrupt();
#endif
#if (IGN_CHANNELS >= 2)
  void ignitionSchedule2Interrupt();
#endif
#if (IGN_CHANNELS >= 3)
  void ignitionSchedule3Interrupt();
#endif
#if (IGN_CHANNELS >= 4)
  void ignitionSchedule4Interrupt();
#endif
#if (IGN_CHANNELS >= 5)
  void ignitionSchedule5Interrupt();
#endif
#if (IGN_CHANNELS >= 6)
  void ignitionSchedule6Interrupt();
#endif
#if (IGN_CHANNELS >= 7)
  void ignitionSchedule7Interrupt();
#endif
#if (IGN_CHANNELS >= 8)
  void ignitionSchedule8Interrupt();
#endif
/**@}*/
#endif

/** \enum ScheduleStatus
 * @brief The current state of a schedule
 * */
enum ScheduleStatus {
  /** Schedule turned off and there is no scheduled plan */
  OFF, 
  /** There's a scheduled plan, but is has not started to run yet */
  PENDING,
  /** Schedule is currently running */
  RUNNING,
  /** Schedule is currently running, and has a next schedule to run */
  RUNNINGHASNEXT,  
}; 


/**
 * @brief A schedule for a single channel.
 */
struct Schedule {  

  // Deduce the real types of the counter and compare registers.
  // COMPARE_TYPE is NOT the same - it's just an integer type wide enough to
  // store 16-bit counter/compare calculation results.
  typedef decltype(FUEL1_COUNTER /* <-- Arbitrary choice of macro, assumes all have the same type */) counter_t;
  typedef decltype(FUEL1_COMPARE /* <-- Arbitrary choice of macro, assumes all have the same type */) compare_t;

  Schedule( counter_t &counter, compare_t &compare,
            void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : counter(counter)
  , compare(compare)
  , pTimerDisable(_pTimerDisable)
  , pTimerEnable(_pTimerEnable)
  {
  }

  volatile ScheduleStatus Status; ///< Schedule status: OFF, PENDING, STAGED, RUNNING, RUNNINGHASNEXT
  void (*pStartFunction)();        ///< Start Callback function for schedule
  void (*pEndFunction)();          ///< End Callback function for schedule
  volatile COMPARE_TYPE endCounter;   ///< The counter value of the timer when this will end

  volatile COMPARE_TYPE nextStartCounter;      ///< Planned start of next schedule (when current schedule is RUNNINGHASNEXT)
  volatile COMPARE_TYPE nextEndCounter;        ///< Planned end of next schedule (when current schedule is RUNNINGHASNEXT)

  counter_t &counter;  // Reference to the counter register. E.g. TCNT3
  compare_t &compare;  // Reference to the compare register. E.g. OCR3A
  void (&pTimerDisable)();    // Reference to the timer disable function
  void (&pTimerEnable)();     // Reference to the timer enable function
};

inline bool isRunning(const Schedule &schedule) {
  return schedule.Status==RUNNING || schedule.Status==RUNNINGHASNEXT;
}
inline bool isPending(const Schedule &schedule) {
  return schedule.Status==PENDING;
}


/**
 * @brief Set the timer start and end callbacks.
 * Each timer can have only 1 callback associated with it at any given time. 
 * **If you call the setCallback function a 2nd time, the original schedule will be overwritten and not occur.**
 */
inline void setCallbacks(Schedule &schedule, void (*pStartFunction)(), void (*pEndFunction)()) {
  schedule.pStartFunction = pStartFunction;
  schedule.pEndFunction = pEndFunction;
}

/** @brief Immediately run the schedule if not already running. */
void runSchedule(struct Schedule *schedule, unsigned long duration);

/** @brief Immediately run the schedule - regardless of current state. */
void forceRunSchedule(struct Schedule *schedule, unsigned long duration);

/** @brief A schedule specialized for injection pulses. */
struct FuelSchedule: public Schedule {
  FuelSchedule(counter_t &counter, compare_t &compare,
              void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : Schedule(counter, compare, _pTimerDisable, _pTimerEnable)
  {    
  }
};

/*! \name The fuel schedulers */
/**@{*/
extern FuelSchedule fuelSchedule1;
extern FuelSchedule fuelSchedule2;
extern FuelSchedule fuelSchedule3;
extern FuelSchedule fuelSchedule4;
#if (INJ_CHANNELS >= 5)
extern FuelSchedule fuelSchedule5;
#endif
#if (INJ_CHANNELS >= 6)
extern FuelSchedule fuelSchedule6;
#endif
#if (INJ_CHANNELS >= 7)
extern FuelSchedule fuelSchedule7;
#endif
#if (INJ_CHANNELS >= 8)
extern FuelSchedule fuelSchedule8;
#endif
/**@}*/


/** @brief A schedule specialized for ignition events. */
struct IgnSchedule: public Schedule {
  IgnSchedule(counter_t &counter, compare_t &compare,
              void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : Schedule(counter, compare, _pTimerDisable, _pTimerEnable)
  {    
  }

  /** @brief The number of crank degrees until corresponding cylinder is at TDC 
   * (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)
  */
  int channelIgnDegrees=0;
};

/*! \name The ignition schedulers */
/**@{*/ 
extern IgnSchedule ignitionSchedule1;
extern IgnSchedule ignitionSchedule2;
extern IgnSchedule ignitionSchedule3;
extern IgnSchedule ignitionSchedule4;
extern IgnSchedule ignitionSchedule5;
#if IGN_CHANNELS >= 6
extern IgnSchedule ignitionSchedule6;
#endif
#if IGN_CHANNELS >= 7
extern IgnSchedule ignitionSchedule7;
#endif
#if IGN_CHANNELS >= 8
extern IgnSchedule ignitionSchedule8;
#endif
/**@}*/

void initialiseSchedulers();

/** @brief Start priming all injectors. */
void beginInjectorPriming();

/** @brief Set the next schedule for the ignition channel.
 * 
 * The spark timing is automatically calculated
 * @param crankAngle The current crank angle
 * @param ignitionEndAngle The crank angle at which to fire the spark
 * @param coilChargeDuration is the time to charge the ignition coil
 */
void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule, int16_t crankAngle, int ignitionEndAngle, unsigned long coilChargeDuration);

/** @brief Manually set the next schedule for the ignition channel.
 * 
 * @param totalDuration is the duration of the entire schedule in uS (microseconds): spark will fire at the end of the schedule
 * @param coilChargeDuration is the time to charge the ignition coil
 */
void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule, unsigned long totalDuration, unsigned long coilChargeDuration);

/** @brief Set the next schedule for the injection channel.
 * 
 * The injector open time is automatically calculated.
 * @param crankAngle The current crank angle
 * @param injectorEndAngle The crank angle at which to end teh injection pulse
 * @param openDuration length of time the injector is open
 */
void setFuelSchedule(struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long openDuration);

/** @brief Manually set the next schedule for the ignition channel.
 * 
 * @param totalDuration the duration of the entire schedule in uS (microseconds): injector will close at the end of the schedule
 * @param openDuration length of time the injector is open
 */
void setFuelSchedule(struct FuelSchedule *targetSchedule , unsigned long totalDuration, unsigned long openDuration);

#endif // SCHEDULER_H
