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
 * @brief A schedule for a single channel. A schedule consists of 2 parts:
 * - total duration
 * - event duration (E.g. ignition or injection)
 * 
 * These overlap and end at the same time:
 *                         Total Duration
 *   |------------------------------------------------------------|
 *                                          |---------------------|
 *                                               Event Duration
 * We use this overlapping format because it's simpler for the rest
 * of the code base to compute start and end crank angles (which 
 * are synonymous with time).
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
    reset();
  }

  /** @brief Reset the schedule to its default state. */
  void reset();

  inline bool isRunning() const {
    return Status==RUNNING || Status==RUNNINGHASNEXT;
  }
  
  inline bool isPending() const {
    return Status==PENDING;
  }  

  /**
   * @brief Set the timer start and end callbacks.
   * Each timer can have only 1 callback associated with it at any given time. 
   * **If you call the setCallback function a 2nd time, the original schedule will be overwritten and not occur.**
   */
  inline void setCallbacks(void (*pStartFunction)(), void (*pEndFunction)()) {
    this->pStartFunction = pStartFunction;
    this->pEndFunction = pEndFunction;
  }

  /** \enum scheduleResult
   * @brief return values for methods that initiate a schedule
   * */
  enum scheduleResult {
    /** Schedule was started */
    STARTED,
    /** Schedule was already running, new schedule is queued */
    QUEUED,
    /** Schedule is too long, wait period isn't long enough */
    BADDURATION
  };
  
  /** @brief Immediately run the schedule if not already running. */
  void runSchedule(unsigned long duration);

  /** @brief Immediately run the schedule - regardless of current state. */
  void forceRunSchedule(unsigned long duration);

  /** @brief Begin a new schedule. 
   * If no schedule is currently running, the new schedule will be started immediately.
   * If a schedule is currently running, the new schedule will be started after the current one finishes.
   * 
   * @param totalDuration the duration of the entire schedule in uS (microseconds): pEndFunction will be invoked after this duration.
   * @param eventDuration event length of time. pStartFunction will be invoked at time (totalDuration-eventDuration).
   * @param minWaitDuration minimum duration of the waiting period before pEndFunction is invoked.
   */
  scheduleResult beginSchedule(unsigned long totalDuration, unsigned long eventDuration);  

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


/** @brief A schedule specialized for injection pulses. */
struct FuelSchedule: public Schedule {
  FuelSchedule(counter_t &counter, compare_t &compare,
              void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : Schedule(counter, compare, _pTimerDisable, _pTimerEnable)
  {    
  }

  /** @brief Set the number of crank degrees until corresponding cylinder is at TDC */
  void setInjDegrees(int injDegrees) {
    this->injDegrees = injDegrees;
    injEnabled = true;
  }

  /** @brief Set the next schedule for the injection channel.
   * 
   * The injector open time is automatically calculated.
   * @param crankAngle The current crank angle
   * @param injectorEndAngle The crank angle at which to end teh injection pulse
   * @param openDuration length of time the injector is open
   */
  scheduleResult setFuelSchedule(int16_t crankAngle, int16_t injectorEndAngle, unsigned long openDuration);

  /** @brief The number of crank degrees until corresponding cylinder is at TDC 
   * (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones) */
  int injDegrees = 0;

  /** @brief Is this injection channel enabled. */
  bool injEnabled = true;
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

  /** @brief Tell this igntion schedule what the current calculated ignition advance is
   * 
   * Side effect is that it computes the ignition end angle for this schedule.
   */
  inline void setAdvance(int8_t advance)
  {
      ignitionEndAngle = channelIgnDegrees - advance;
      if(ignitionEndAngle > CRANK_ANGLE_MAX_IGN) {ignitionEndAngle -= CRANK_ANGLE_MAX_IGN;}
  }

  /** @brief Set the next schedule for the ignition channel.
   * 
   * The spark timing is automatically calculated
   * @param crankAngle The current crank angle
   * @param coilChargeDuration is the time to charge the ignition coil
   */
  scheduleResult setIgnitionSchedule(int16_t crankAngle, unsigned long coilChargeDuration);

  /** @brief The number of crank degrees until corresponding cylinder is at TDC 
   * (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)
  */
  int16_t channelIgnDegrees=0;

  /** @brief The crank angle to fire the spark at */
  int16_t ignitionEndAngle=0;
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

/** @brief Start priming all injectors. */
void beginInjectorPriming();


#endif // SCHEDULER_H
