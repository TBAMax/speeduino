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

64 prescale gives tick every 4uS.
64 prescale gives overflow every 262140uS (This means maximum wait time is 0.26214 seconds).

*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "globals.h"

#define USE_IGN_REFRESH
#define IGNITION_REFRESH_THRESHOLD  230U //Time in uS that the refresh functions will check to ensure there is enough time before changing the end compare
#define INJECTION_REFRESH_TRESHOLD  230U //Time in us that the refresh functions will check to ensure there is enough time before changing the start or end compare

void initialiseSchedulers();
void beginInjectorPriming();

void setIgnitionSchedule(struct Schedule *ignitionSchedule , int16_t crankAngle,int ignitionEndAngle, unsigned long duration);
void setIgnitionSchedule(struct Schedule *ignitionSchedule); //overload function for starting schedule(dwell) immediately, this is used in the fixed cranking ignition

void ignitionScheduleInterrupt(struct Schedule *ignitionSchedule);

void setFuelSchedule (struct FuelSchedule *targetSchedule, unsigned long duration);
void setFuelSchedule (struct FuelSchedule *targetSchedule, int16_t crankAngle, int16_t injectorEndAngle, unsigned long duration);


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

  volatile COMPARE_TYPE nextStartCompare;      ///< Planned start of next schedule (when current schedule is RUNNING)
  volatile COMPARE_TYPE nextEndCompare;        ///< Planned end of next schedule (when current schedule is RUNNING)
  volatile bool hasNextSchedule = false; ///< Enable flag for planned next schedule (when current schedule is RUNNING)
  volatile bool endScheduleSetByDecoder = false;
  int channelIgnDegrees=0; // The number of crank degrees until corresponding cylinder is at TDC (cylinder1 is obviously 0 for virtually ALL engines, but there's some weird ones)

  virtual COMPARE_TYPE getIgnCounter(){return 0U;}; //Function for getting counter value
  virtual void setIgnitionCompare(COMPARE_TYPE compareValue){}; //Function for setting counter compare value
  virtual void ignTimerDisable(){}; //Function to disable timer for specific channel
  virtual void ignTimerEnable(){}; //Function to enable timer for specific channel  
};

struct Ign1: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN1_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN1_COMPARE =compareValue;}
  void ignTimerDisable(){IGN1_TIMER_DISABLE();}
  void ignTimerEnable(){IGN1_TIMER_ENABLE();}
};

struct Ign2: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN2_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN2_COMPARE =compareValue;}
  void ignTimerDisable(){IGN2_TIMER_DISABLE();}
  void ignTimerEnable(){IGN2_TIMER_ENABLE();}
};

struct Ign3: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN3_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN3_COMPARE =compareValue;}
  void ignTimerDisable(){IGN3_TIMER_DISABLE();}
  void ignTimerEnable(){IGN3_TIMER_ENABLE();}
};

struct Ign4: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN4_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN4_COMPARE =compareValue;}
  void ignTimerDisable(){IGN4_TIMER_DISABLE();}
  void ignTimerEnable(){IGN4_TIMER_ENABLE();}
};

struct Ign5: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN5_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN5_COMPARE =compareValue;}
  void ignTimerDisable(){IGN5_TIMER_DISABLE();}
  void ignTimerEnable(){IGN5_TIMER_ENABLE();}
};

struct Ign6: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN6_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN6_COMPARE =compareValue;}
  void ignTimerDisable(){IGN6_TIMER_DISABLE();}
  void ignTimerEnable(){IGN6_TIMER_ENABLE();}
};

struct Ign7: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN7_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN7_COMPARE =compareValue;}
  void ignTimerDisable(){IGN7_TIMER_DISABLE();}
  void ignTimerEnable(){IGN7_TIMER_ENABLE();}
};

struct Ign8: Schedule //Derived ignitionSchedule structs with  channel specific override functions
{
  COMPARE_TYPE getIgnCounter(){return IGN8_COUNTER;}
  void setIgnitionCompare(COMPARE_TYPE compareValue){IGN8_COMPARE =compareValue;}
  void ignTimerDisable(){IGN8_TIMER_DISABLE();}
  void ignTimerEnable(){IGN8_TIMER_ENABLE();}
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
  virtual COMPARE_TYPE getFuelCounter(){}; //Function for getting counter value
  virtual void setFuelCompare(COMPARE_TYPE compareValue){}; //Function for setting counter compare value
  virtual void fuelTimerDisable(){}; //Function to disable timer for specific channel
  virtual void fuelTimerEnable(){}; //Function to enable timer for specific channel
};
struct Fue1: FuelSchedule //derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL1_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL1_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL1_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL1_TIMER_ENABLE();};
};
struct Fue2: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL2_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL2_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL2_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL2_TIMER_ENABLE();};
};
struct Fue3: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL3_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL3_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL3_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL3_TIMER_ENABLE();};
};
struct Fue4: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL4_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL4_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL4_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL4_TIMER_ENABLE();};
};
struct Fue5: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL5_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL5_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL5_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL5_TIMER_ENABLE();};
};
struct Fue6: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL6_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL6_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL6_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL6_TIMER_ENABLE();};
};
struct Fue7: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL7_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL7_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL7_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL7_TIMER_ENABLE();};
};
struct Fue8: FuelSchedule//derived FuelShedule with channel specific functions
{
  COMPARE_TYPE getFuelCounter() {return FUEL8_COUNTER;};
  void setFuelCompare(COMPARE_TYPE compareValue){FUEL8_COMPARE =compareValue;};
  void fuelTimerDisable(){FUEL8_TIMER_DISABLE();};
  void fuelTimerEnable(){FUEL8_TIMER_ENABLE();};
};


extern Fue1 fuelSchedule1;
extern Fue2 fuelSchedule2;
extern Fue3 fuelSchedule3;
extern Fue4 fuelSchedule4;
#if (INJ_CHANNELS >= 5)
extern Fue5 fuelSchedule5;
#endif
#if (INJ_CHANNELS >= 6)
extern Fue6 fuelSchedule6;
#endif
#if (INJ_CHANNELS >= 7)
extern Fue7 fuelSchedule7;
#endif
#if (INJ_CHANNELS >= 8)
extern Fue8 fuelSchedule8;
#endif

extern Ign1 ignitionSchedule1;
extern Ign2 ignitionSchedule2;
extern Ign3 ignitionSchedule3;
extern Ign4 ignitionSchedule4;
extern Ign5 ignitionSchedule5;
#if IGN_CHANNELS >= 6
extern Ign6 ignitionSchedule6;
#endif
#if IGN_CHANNELS >= 7
extern Ign7 ignitionSchedule7;
#endif
#if IGN_CHANNELS >= 8
extern Ign8 ignitionSchedule8;
#endif


#endif // SCHEDULER_H
