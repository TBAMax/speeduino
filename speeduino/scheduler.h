/** @file
Injector and Ignition (on/off) scheduling (structs).

This scheduler is designed to maintain 2 schedules for use by the fuel and ignition systems.
It functions by waiting for the overflow vectors from each of the timers in use to overflow, which triggers an interrupt.

## Technical

Currently I am prescaling the 16-bit timers to 256 for injection and 64 for ignition.
This means that the counter increments every 16us (injection) / 4uS (ignition) and will overflow every 1048576uS.

    Max Period = (Prescale)*(1/Frequency)*(2^17)

For more details see https://playground.arduino.cc/Code/Timer1/ (OLD: http://playground.arduino.cc/code/timer1 ).
This means that the precision of the scheduler is:

- 16uS (+/- 8uS of target) for fuel
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

256 prescale gives tick every 16uS.
256 prescale gives overflow every 1048576uS (This means maximum wait time is 1.0485 seconds).

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "globals.h"
#include "crankMaths.h"
#include "scheduledIO.h"

#define SCHEDULE_REFRESH_THRESHOLD  30U //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#define SCHEDULE_OVERLAP_THRESHOLD  96U //Time in us, minimum injector off time that is allowed before switching to 100% duty cycle, to ensure there is enough time before changing end compare

#define DWELL_AVERAGE_ALPHA 30
#define DWELL_AVERAGE(input) LOW_PASS_FILTER((input), DWELL_AVERAGE_ALPHA, currentStatus.actualDwell)
//#define DWELL_AVERAGE(input) (currentStatus.dwell) //Can be use to disable the above for testing

typedef void (*voidVoidCallback)(void);

void initialiseSchedulers(void);
void beginInjectorPriming(void);

void disableFuelSchedule(byte channel);
void disableIgnSchedule(byte channel);
void disableAllFuelSchedules(void);
void disableAllIgnSchedules(void);

void refreshIgnitionSchedule1(unsigned long timeToEnd);

//The ARM cores use separate functions for their ISRs
#if defined(ARDUINO_ARCH_STM32) || defined(CORE_TEENSY)
  void fuelSchedule1Interrupt(void);
  void fuelSchedule2Interrupt(void);
  void fuelSchedule3Interrupt(void);
  void fuelSchedule4Interrupt(void);
#if (INJ_CHANNELS >= 5)
  void fuelSchedule5Interrupt(void);
#endif
#if (INJ_CHANNELS >= 6)
  void fuelSchedule6Interrupt(void);
#endif
#if (INJ_CHANNELS >= 7)
  void fuelSchedule7Interrupt(void);
#endif
#if (INJ_CHANNELS >= 8)
  void fuelSchedule8Interrupt(void);
#endif
#if (IGN_CHANNELS >= 1)
  void ignitionSchedule1Interrupt(void);
#endif
#if (IGN_CHANNELS >= 2)
  void ignitionSchedule2Interrupt(void);
#endif
#if (IGN_CHANNELS >= 3)
  void ignitionSchedule3Interrupt(void);
#endif
#if (IGN_CHANNELS >= 4)
  void ignitionSchedule4Interrupt(void);
#endif
#if (IGN_CHANNELS >= 5)
  void ignitionSchedule5Interrupt(void);
#endif
#if (IGN_CHANNELS >= 6)
  void ignitionSchedule6Interrupt(void);
#endif
#if (IGN_CHANNELS >= 7)
  void ignitionSchedule7Interrupt(void);
#endif
#if (IGN_CHANNELS >= 8)
  void ignitionSchedule8Interrupt(void);
#endif
#endif
/** Schedule statuses.
 * - OFF - Schedule turned off and there is no scheduled plan
 * - PENDING - There's a scheduled plan, but is has not started to run yet
 * - RUNNING - Schedule is currently running
 */
enum ScheduleStatus {OFF, PENDING, RUNNING}; //The statuses that a schedule can have
extern void nullCallback(void);

/** Universal schedule for Ignition or Fuel .
 */
struct Schedule {
  // Deduce the real types of the counter and compare registers.
  // COMPARE_TYPE is NOT the same - it's just an integer type wide enough to
  // store 16-bit counter/compare calculation results.
  /** @brief The type of a timer counter register (this varies between platforms) */
  using counter_t = decltype(FUEL1_COUNTER /* <-- Arbitrary choice of macro, assumes all have the same type */);
  /** @brief The type of a timer compare register (this varies between platforms) */
  using compare_t = decltype(FUEL1_COMPARE /* <-- Arbitrary choice of macro, assumes all have the same type */);

  /**
   * @brief Construct a new Schedule object
   * 
   * @param counter A <b>reference</b> to the timer counter
   * @param compare A <b>reference</b> to the timer comparator
   */
  constexpr Schedule(counter_t &counter, compare_t &compare)
    : _counter(counter)
    , _compare(compare) 
  {}  

  ScheduleStatus Status = OFF;  ///< Schedule status: OFF, PENDING, RUNNING
  voidVoidCallback pStartCallback = &nullCallback; ///< Start Callback function for schedule
  voidVoidCallback pEndCallback = &nullCallback;   ///< End Callback function for schedule
  int16_t channelDegrees = 0;          ///< The number of crank degrees until this cylinder is at TDC

//  volatile unsigned long startTime = 0; /**< The system time (in uS) that the schedule started, used by the overdwell protection or duty cycle limiter */
  COMPARE_TYPE endCompare = 0;   ///< The counter value of the timer when this will end

  COMPARE_TYPE nextStartCompare = 0;      ///< Planned start of next schedule (when current schedule is RUNNING)
  COMPARE_TYPE nextEndCompare = 0;        ///< Planned end of next schedule (when current schedule is RUNNING)
  bool hasNextSchedule = false; ///< Enable flag for planned next schedule (when current schedule is RUNNING)
  bool overlap = false;   ///< Flag to indicate that the next schedule overlaps the current one (100% duty cycle case) 

  //uint16_t dwellTimeLimit = 0;    /**< [uS]Dwell time limit, used by dwell limiting and duty cycle limiting, 0 for unlimited */
  //uint8_t minPauseBetweenSchedules = 0; /**< [us]Minimum pause between schedules, used dwell time and duty cycle limiting */

  //uint8_t startTime = 0; /**[ms]this is used in owerdwell protection, not really needed for internal working of the schedulers 
  //                   *only use uint8_t here assuming dwell limit is always way smaller than 255ms, this speeds up things on atmega, also saves some ram*/  

  counter_t &_counter;  // Reference to the counter register. E.g. TCNT3
  compare_t &_compare;  // Reference to the compare register. E.g. OCR3A
  void setCallbacks(voidVoidCallback pStartCallback, voidVoidCallback pEndCallback);
  void setSchedule(int16_t EndAngle, unsigned long duration, int crankAngleMax);
  void setSchedule(unsigned long duration);//Sets schedule to run immediately for duration specified
  virtual void setSchedule(int16_t EndAngle, unsigned long duration)=0;  
};

//Ignition schedules
struct IgnitionSchedule : public Schedule {
  using Schedule::Schedule;
  using Schedule::setSchedule;
  void setSchedule(int16_t EndAngle, unsigned long duration) override{
    Schedule::setSchedule(EndAngle, duration, CRANK_ANGLE_MAX_IGN);
  };
};

//Fuel schedules (must have special ability to allow for continous running at 100% duty cycle)
struct FuelSchedule : public Schedule {
  using Schedule::Schedule;
  using Schedule::setSchedule;
  void setSchedule(int16_t EndAngle, unsigned long duration) override{
    Schedule::setSchedule(EndAngle, duration, CRANK_ANGLE_MAX_INJ);
  };
};

extern FuelSchedule fuelSchedule1;
extern FuelSchedule fuelSchedule2;
extern FuelSchedule fuelSchedule3;
extern FuelSchedule fuelSchedule4;
#if INJ_CHANNELS >= 5
extern FuelSchedule fuelSchedule5;
#endif
#if INJ_CHANNELS >= 6
extern FuelSchedule fuelSchedule6;
#endif
#if INJ_CHANNELS >= 7
extern FuelSchedule fuelSchedule7;
#endif
#if INJ_CHANNELS >= 8
extern FuelSchedule fuelSchedule8;
#endif

extern IgnitionSchedule ignitionSchedule1;
extern IgnitionSchedule ignitionSchedule2;
extern IgnitionSchedule ignitionSchedule3;
extern IgnitionSchedule ignitionSchedule4;
extern IgnitionSchedule ignitionSchedule5;
#if IGN_CHANNELS >= 6
extern IgnitionSchedule ignitionSchedule6;
#endif
#if IGN_CHANNELS >= 7
extern IgnitionSchedule ignitionSchedule7;
#endif
#if IGN_CHANNELS >= 8
extern IgnitionSchedule ignitionSchedule8;
#endif

#endif // SCHEDULER_H
