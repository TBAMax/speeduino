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
it will not reoccur unless you explicitely ask/re-register for it).
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

#define USE_IGN_REFRESH
#define IGNITION_REFRESH_THRESHOLD  230U //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#define INJECTION_REFRESH_TRESHOLD  230U //Time in us that the refresh functions will check to ensure there is enough time before changing the start or end compare

//(convert macros to inline functions for now)
inline void setFuel1Compare(COMPARE_TYPE compareValue);
inline void setFuel2Compare(COMPARE_TYPE compareValue);
inline void setFuel3Compare(COMPARE_TYPE compareValue);
inline void setFuel4Compare(COMPARE_TYPE compareValue);
inline void setFuel5Compare(COMPARE_TYPE compareValue);
inline void setFuel6Compare(COMPARE_TYPE compareValue);
inline void setFuel7Compare(COMPARE_TYPE compareValue);
inline void setFuel8Compare(COMPARE_TYPE compareValue);

//fuel counter getting functions(convert macros to inline functions)
inline COMPARE_TYPE getFuel1Counter();
inline COMPARE_TYPE getFuel2Counter();
inline COMPARE_TYPE getFuel3Counter();
inline COMPARE_TYPE getFuel4Counter();
inline COMPARE_TYPE getFuel5Counter();
inline COMPARE_TYPE getFuel6Counter();
inline COMPARE_TYPE getFuel7Counter();
inline COMPARE_TYPE getFuel8Counter();

//(convert macros to inline functions for now)
inline void fuel1TimerDisable();//fuel timer disable functions 
inline void fuel2TimerDisable();//fuel timer disable functions 
inline void fuel3TimerDisable();//fuel timer disable functions 
inline void fuel4TimerDisable();//fuel timer disable functions 
inline void fuel5TimerDisable();//fuel timer disable functions 
inline void fuel6TimerDisable();//fuel timer disable functions 
inline void fuel7TimerDisable();//fuel timer disable functions 
inline void fuel8TimerDisable();//fuel timer disable functions 

//(convert macros to inline functions for now)
inline void fuel1TimerEnable();//fuel timer enable functions 
inline void fuel2TimerEnable();//fuel timer enable functions
inline void fuel3TimerEnable();//fuel timer enable functions 
inline void fuel4TimerEnable();//fuel timer enable functions 
inline void fuel5TimerEnable();//fuel timer enable functions
inline void fuel6TimerEnable();//fuel timer enable functions
inline void fuel7TimerEnable();//fuel timer enable functions
inline void fuel8TimerEnable();//fuel timer enable functions


void initialiseSchedulers();
void beginInjectorPriming();

void setIgnitionSchedule(struct Schedule *ignitionSchedule , int16_t crankAngle,int ignitionEndAngle, unsigned long duration);
void setIgnitionSchedule(struct Schedule *ignitionSchedule); //overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition

void ignitionScheduleInterrupt(struct Schedule *ignitionSchedule);

inline void ign1TimerEnable();
inline void ign2TimerEnable();
inline void ign3TimerEnable();
inline void ign4TimerEnable();
inline void ign5TimerEnable();
inline void ign6TimerEnable();
inline void ign7TimerEnable();
inline void ign8TimerEnable();

//those small functions are needed to use ignition counter definitions on different platvorms
inline COMPARE_TYPE getIgn1Counter();
inline COMPARE_TYPE getIgn2Counter();
inline COMPARE_TYPE getIgn3Counter();
inline COMPARE_TYPE getIgn4Counter();
inline COMPARE_TYPE getIgn5Counter();
inline COMPARE_TYPE getIgn6Counter();
inline COMPARE_TYPE getIgn7Counter();
inline COMPARE_TYPE getIgn8Counter();

//those small functions are needed to use ignition counter compare definitions on different platvorms
inline void setIgnition1Compare(COMPARE_TYPE value);
inline void setIgnition2Compare(COMPARE_TYPE value);
inline void setIgnition3Compare(COMPARE_TYPE value);
inline void setIgnition4Compare(COMPARE_TYPE value);
inline void setIgnition5Compare(COMPARE_TYPE value);
inline void setIgnition6Compare(COMPARE_TYPE value);
inline void setIgnition7Compare(COMPARE_TYPE value);
inline void setIgnition8Compare(COMPARE_TYPE value);

void setFuelSchedule (struct FuelSchedule *targetSchedule, unsigned long duration);
void setFuelSchedule (struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration);

inline void refreshIgnitionSchedule1(unsigned long timeToEnd) __attribute__((always_inline));

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
 * - RUNNING - Schedule is currently running
 */
enum ScheduleStatus {OFF, PENDING, STAGED, RUNNING}; //The statuses that a schedule can have

/** Ignition schedule.
 */
struct Schedule {

  volatile ScheduleStatus Status; ///< Schedule status: OFF, PENDING, STAGED, RUNNING
  void (*StartFunction)();        ///< Start Callback function for schedule
  void (*EndFunction)();          ///< End Callback function for schedule
  volatile COMPARE_TYPE endCompare;   ///< The counter value of the timer when this will end

  COMPARE_TYPE nextStartCompare;      ///< Planned start of next schedule (when current schedule is RUNNING)
  COMPARE_TYPE nextEndCompare;        ///< Planned end of next schedule (when current schedule is RUNNING)
  volatile bool hasNextSchedule = false; ///< Enable flag for planned next schedule (when current schedule is RUNNING)
  volatile bool endScheduleSetByDecoder = false;

  COMPARE_TYPE (*getIgnCounter)(); //Function for getting counter value
  void (*setIgnitionCompare)(COMPARE_TYPE); //Function for setting counter compare value
  void (*ignTimerDisable)(); //Function to disable timer for specific channel
  void (*ignTimerEnable)(); //Function to enable timer for specific channel
  int channelIgnDegrees=0; // The number of crank degrees until corresponding cylinder is at TDC (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)
};
/** Fuel injection schedule.
* Fuel schedules don't use startTime/endScheduleSetByDecoder/schedulesSet/startCompare/duration variables.
* They are removed in this struct to save RAM.
*/
struct FuelSchedule {
  volatile ScheduleStatus Status; ///< Schedule status: OFF, PENDING, STAGED, RUNNING
  volatile COMPARE_TYPE endCompare;   ///< The counter value of the timer when this will end

  COMPARE_TYPE nextStartCompare;
  COMPARE_TYPE nextEndCompare;
  volatile bool hasNextSchedule = false;

  void (*injStartFunction)();        ///< Start function for injection
  void (*injEndFunction)();        ///< End function for injection
  COMPARE_TYPE (*getFuelCounter)(); //Function for getting counter value
  void (*setFuelCompare)(COMPARE_TYPE); //Function for setting counter compare value
  void (*fuelTimerDisable)(); //Function to disable timer for specific channel
  void (*fuelTimerEnable)(); //Function to enable timer for specific channel
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
