#pragma once

#include <stdint.h>
#include "scheduler.h"

extern int ignition1EndAngle;

extern int ignition2EndAngle;

extern int ignition3EndAngle;

extern int ignition4EndAngle;

#if (IGN_CHANNELS >= 5)
extern int ignition5EndAngle;
#endif
#if (IGN_CHANNELS >= 6)
extern int ignition6EndAngle;
#endif
#if (IGN_CHANNELS >= 7)
extern int ignition7EndAngle;
#endif
#if (IGN_CHANNELS >= 8)
extern int ignition8EndAngle;
#endif

static inline uint16_t __attribute__((always_inline))  calculateInjectorEndAngle(int16_t injChannelDegrees);

static inline int __attribute__((always_inline)) calculateIgnitionAngle(int channelDegrees);

// Ignition for rotary.
static inline int __attribute__((always_inline))  calculateIgnitionTrailingRotary(int rotarySplitDegrees, int leadIgnitionAngle);

#include "schedule_calcs.hpp"
