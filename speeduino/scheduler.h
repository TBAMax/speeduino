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
#include "timers.h"

#define USE_IGN_REFRESH
#define IGNITION_REFRESH_THRESHOLD  230U //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#define INJECTION_REFRESH_TRESHOLD  230U //Time in us that the refresh functions will check to ensure there is enough time before changing the start or end compare
#define INJECTION_OVERLAP_TRESHOLD  96U //Time in us, basically minimum injector off time that is allowed.

void initialiseSchedulers(void);
void beginInjectorPriming(void);

void setIgnitionSchedule(struct Schedule *ignitionSchedule , int16_t crankAngle,int ignitionEndAngle, unsigned long duration);
void setIgnitionSchedule(struct Schedule *ignitionSchedule); //overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition
void setIgnitionSchedule(struct Schedule *ignitionSchedule,uint16_t dwell);

void ignitionScheduleInterrupt(struct Schedule *ignitionSchedule);
void fuelScheduleInterrupt(struct Schedule *fuelSchedule);

void setFuelSchedule (struct Schedule *targetSchedule, unsigned long duration);
void setFuelSchedule (struct Schedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration);


//The ARM cores use seprate functions for their ISRs
#if defined(ARDUINO_ARCH_STM32) || defined(CORE_TEENSY)
  static inline void fuelSchedule1Interrupt(void);
  static inline void fuelSchedule2Interrupt(void);
  static inline void fuelSchedule3Interrupt(void);
  static inline void fuelSchedule4Interrupt(void);
#if (INJ_CHANNELS >= 5)
  static inline void fuelSchedule5Interrupt(void);
#endif
#if (INJ_CHANNELS >= 6)
  static inline void fuelSchedule6Interrupt(void);
#endif
#if (INJ_CHANNELS >= 7)
  static inline void fuelSchedule7Interrupt(void);
#endif
#if (INJ_CHANNELS >= 8)
  static inline void fuelSchedule8Interrupt(void);
#endif
#if (IGN_CHANNELS >= 1)
  static inline void ignitionSchedule1Interrupt(void);
#endif
#if (IGN_CHANNELS >= 2)
  static inline void ignitionSchedule2Interrupt(void);
#endif
#if (IGN_CHANNELS >= 3)
  static inline void ignitionSchedule3Interrupt(void);
#endif
#if (IGN_CHANNELS >= 4)
  static inline void ignitionSchedule4Interrupt(void);
#endif
#if (IGN_CHANNELS >= 5)
  static inline void ignitionSchedule5Interrupt(void);
#endif
#if (IGN_CHANNELS >= 6)
  static inline void ignitionSchedule6Interrupt(void);
#endif
#if (IGN_CHANNELS >= 7)
  static inline void ignitionSchedule7Interrupt(void);
#endif
#if (IGN_CHANNELS >= 8)
  static inline void ignitionSchedule8Interrupt(void);
#endif
#endif
/** Schedule statuses.
 * - OFF - Schedule turned off and there is no scheduled plan
 * - PENDING - There's a scheduled plan, but is has not started to run yet
 * - RUNNING - Schedule is currently running
 */
enum ScheduleStatus : uint8_t {OFF, PENDING, RUNNING}; //The statuses that a schedule can have

/** Ignition schedule and Fuel Schedule, both use the same struct now.
 */
struct Schedule {  
  // Deduce the real types of the counter and compare registers.
  // COMPARE_TYPE is NOT the same - it's just an integer type wide enough to
  // store 16-bit counter/compare calculation results.
  /** @brief The type of a timer counter register */
  using counter_t = decltype(FUEL1_COUNTER /* <-- Arbitrary choice of macro, assumes all have the same type */);
  /** @brief The type of a timer compare register */
  using compare_t = decltype(FUEL1_COMPARE /* <-- Arbitrary choice of macro, assumes all have the same type */);

  Schedule( counter_t &counter, compare_t &compare,
            void (&_pTimerDisable)(), void (&_pTimerEnable)())
  : counter(counter)
  , compare(compare)
  , pTimerStartFunction(_pTimerEnable)
  , pTimerEndFunction(_pTimerDisable)
  {
  }

  volatile ScheduleStatus Status; ///< Schedule status: OFF, PENDING, RUNNING
 
  volatile COMPARE_TYPE endCompare;   ///< The counter value of the timer when this will end

  volatile COMPARE_TYPE nextStartCompare;      ///< Planned start of next schedule (when current schedule is RUNNING)
  volatile COMPARE_TYPE nextEndCompare;        ///< Planned end of next schedule (when current schedule is RUNNING)
  volatile bool hasNextSchedule = false; ///< Enable flag for planned next schedule (when current schedule is RUNNING)  
  int channelDegrees=0; // The number of crank degrees until corresponding cylinder is at TDC (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)

  void (*StartFunction)();        ///< Start Callback function for schedule
  void (*EndFunction)();          ///< End Callback function for schedule

  COMPARE_TYPE getCounter(void) { return counter; }; //Function for getting counter value
  void setCompare(COMPARE_TYPE compareValue) { compare=(COMPARE_TYPE)compareValue; }; //Function for setting counter compare value
  void timerDisable(void) { pTimerEndFunction(); }; //Function to disable timer for specific channel
  void timerEnable(void) { pTimerStartFunction(); }; //Function to enable timer for specific channel

  uint8_t startTime; /**[ms]this is used in owerdwell protection, not really needed for internal working of the schedulers 
                     *only use uint8_t here assuming dwell limit is always way smaller than 255ms, this speeds up things on atmega, also saves some ram*/

  counter_t &counter;  // Reference to the counter register. E.g. TCNT3
  compare_t &compare;  // Reference to the compare register. E.g. OCR3A
  void (*pTimerStartFunction)();
  void (*pTimerEndFunction)();              
};


extern Schedule fuelSchedule1;
extern Schedule fuelSchedule2;
extern Schedule fuelSchedule3;
extern Schedule fuelSchedule4;
#if (INJ_CHANNELS >= 5)
extern Schedule fuelSchedule5;
#endif
#if (INJ_CHANNELS >= 6)
extern Schedule fuelSchedule6;
#endif
#if (INJ_CHANNELS >= 7)
extern Schedule fuelSchedule7;
#endif
#if (INJ_CHANNELS >= 8)
extern Schedule fuelSchedule8;
#endif

extern Schedule ignitionSchedule1;
extern Schedule ignitionSchedule2;
extern Schedule ignitionSchedule3;
extern Schedule ignitionSchedule4;
extern Schedule ignitionSchedule5;
#if IGN_CHANNELS >= 6
extern Schedule ignitionSchedule6;
#endif
#if IGN_CHANNELS >= 7
extern Schedule ignitionSchedule7;
#endif
#if IGN_CHANNELS >= 8
extern Schedule ignitionSchedule8;
#endif


#endif // SCHEDULER_H
