// Note that all functions with an underscore prefix are NOT part 
// of the public API. They are only here so we can inline them.

#include "scheduler.h"
#include "crankMaths.h"
#include "maths.h"
#include "timers.h"

static inline uint16_t calculateInjectorEndAngle(int16_t injChannelDegrees)
{
  uint16_t endAngle;  
  endAngle=currentStatus.injAngle + injChannelDegrees;//Injector end angle is calculated in degrees ATDC!
  while(endAngle > (uint16_t)CRANK_ANGLE_MAX_INJ) { endAngle -= CRANK_ANGLE_MAX_INJ; }
  return endAngle;
}

static inline int calculateIgnitionAngle(int channelDegrees)
{
  int ignitionEndAngleTemp = channelDegrees - currentStatus.advance;//Ignition advance angle is in degrees BTDC
  if(ignitionEndAngleTemp < 0) {ignitionEndAngleTemp += CRANK_ANGLE_MAX_IGN;}
  if(ignitionEndAngleTemp > CRANK_ANGLE_MAX_IGN) {ignitionEndAngleTemp -= CRANK_ANGLE_MAX_IGN;}
  return ignitionEndAngleTemp;
}

static inline int calculateIgnitionTrailingRotary(int rotarySplitDegrees, int leadIgnitionAngle)
{
  return leadIgnitionAngle + rotarySplitDegrees;
}

#define MIN_CYCLES_FOR_ENDCOMPARE 6

inline void adjustCrankAngle(IgnitionSchedule &schedule, int endAngle, int crankAngle) 
{
  if( (schedule.Status == RUNNING) ) { 
    schedule._compare = schedule._counter + uS_TO_TIMER_COMPARE( angleToTimeMicroSecPerDegree( ignitionLimits( (endAngle - crankAngle) ) ) ) ; 
  }
  else if(currentStatus.startRevolutions > MIN_CYCLES_FOR_ENDCOMPARE) { 
    schedule.endCompare = schedule._counter + uS_TO_TIMER_COMPARE( angleToTimeMicroSecPerDegree( ignitionLimits( (endAngle - crankAngle) ) ) ); 
    //schedule.endScheduleSetByDecoder = true; 
  }
}