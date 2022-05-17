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
Each timer can have only 1 callback associated with it at any given time. If you call the setCallback function a 2nd time,
the original schedule will be overwritten and not occur.

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

#define USE_IGN_REFRESH
#define IGNITION_REFRESH_THRESHOLD  230U //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#define INJECTION_REFRESH_TRESHOLD  230U //Time in us that the refresh functions will check to ensure there is enough time before changing the start or end compare
#define INJECTION_OVERLAP_TRESHOLD  96U //Time in us, basically minimum injector off time that is allowed.

//The ARM cores use seprate functions for their ISRs
#if defined(ARDUINO_ARCH_STM32) || defined(CORE_TEENSY)
  static inline void fuelSchedule1Interrupt();
  static inline void fuelSchedule2Interrupt();
  static inline void fuelSchedule3Interrupt();
  static inline void fuelSchedule4Interrupt();
#if (INJ_CHANNELS >= 5)
  static inline void fuelSchedule5Interrupt();
#endif
#if (INJ_CHANNELS >= 6)
  static inline void fuelSchedule6Interrupt();
#endif
#if (INJ_CHANNELS >= 7)
  static inline void fuelSchedule7Interrupt();
#endif
#if (INJ_CHANNELS >= 8)
  static inline void fuelSchedule8Interrupt();
#endif
#if (IGN_CHANNELS >= 1)
  static inline void ignitionSchedule1Interrupt();
#endif
#if (IGN_CHANNELS >= 2)
  static inline void ignitionSchedule2Interrupt();
#endif
#if (IGN_CHANNELS >= 3)
  static inline void ignitionSchedule3Interrupt();
#endif
#if (IGN_CHANNELS >= 4)
  static inline void ignitionSchedule4Interrupt();
#endif
#if (IGN_CHANNELS >= 5)
  static inline void ignitionSchedule5Interrupt();
#endif
#if (IGN_CHANNELS >= 6)
  static inline void ignitionSchedule6Interrupt();
#endif
#if (IGN_CHANNELS >= 7)
  static inline void ignitionSchedule7Interrupt();
#endif
#if (IGN_CHANNELS >= 8)
  static inline void ignitionSchedule8Interrupt();
#endif
#endif
/** Schedule statuses.
 * - OFF - Schedule turned off and there is no scheduled plan
 * - PENDING - There's a scheduled plan, but is has not started to run yet
 * - STAGED - (???, Not used)
 * - RUNNING - Schedule is currently running,
 * - RUNNINGHASNEXT - Schedule is currently running, but has a next schedule to run
 */
enum ScheduleStatus {
  OFF, 
  PENDING,
  STAGED,
  RUNNING,
  RUNNINGHASNEXT,  
}; //The statuses that a schedule can have

/** Ignition schedule.
 */
struct Schedule {  

  // Deduce the real types of the counter and compare registers.
  // COMPARE_TYPE is NOT the same - it's just an integer type wide enough to
  // store counter/compare calculation results.
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
  volatile COMPARE_TYPE endCompare;   ///< The counter value of the timer when this will end

  volatile COMPARE_TYPE nextStartCompare;      ///< Planned start of next schedule (when current schedule is RUNNINGHASNEXT)
  volatile COMPARE_TYPE nextEndCompare;        ///< Planned end of next schedule (when current schedule is RUNNINGHASNEXT)

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

struct FuelSchedule: public Schedule {
  FuelSchedule(counter_t &counter, compare_t &compare,
              void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : Schedule(counter, compare, _pTimerDisable, _pTimerEnable)
  {    
  }
};

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

struct IgnSchedule: public Schedule {
  IgnSchedule(counter_t &counter, compare_t &compare,
              void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : Schedule(counter, compare, _pTimerDisable, _pTimerEnable)
  {    
  }
  int channelIgnDegrees=0; // The number of crank degrees until corresponding cylinder is at TDC (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)
};

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

void initialiseSchedulers();
void beginInjectorPriming();

void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule , int16_t crankAngle,int ignitionEndAngle, unsigned long duration);
void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule , unsigned long timeout, unsigned long duration);
void setIgnitionSchedule(struct IgnSchedule *ignitionSchedule); //overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition

void setFuelSchedule (struct FuelSchedule *targetSchedule, unsigned long duration);
void setFuelSchedule (struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration);

#endif // SCHEDULER_H
