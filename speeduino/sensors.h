#ifndef SENSORS_H
#define SENSORS_H

#include "Arduino.h"

// The following are alpha values for the ADC filters.
// Their values are from 0 to 255 with 0 being no filtering and 255 being maximum
/*
#define ADCFILTER_TPS  128
#define ADCFILTER_CLT  180
#define ADCFILTER_IAT  180
#define ADCFILTER_O2   128
#define ADCFILTER_BAT  128
#define ADCFILTER_MAP   20 //This is only used on Instantaneous MAP readings and is intentionally very weak to allow for faster response
#define ADCFILTER_BARO  64
*/

#define BARO_MIN      87
#define BARO_MAX      108

#define KNOCK_MODE_DIGITAL  1
#define KNOCK_MODE_ANALOG   2

#define VSS_GEAR_HYSTERESIS 10

volatile byte flexCounter = 0;
volatile byte knockCounter = 0;
volatile uint16_t knockAngle;

unsigned long MAPrunningValue; //Used for tracking either the total of all MAP readings in this cycle (Event average) or the lowest value detected in this cycle (event minimum)
unsigned long EMAPrunningValue; //As above but for EMAP
unsigned int MAPcount; //Number of samples taken in the current MAP cycle
uint32_t MAPcurRev; //Tracks which revolution we're sampling on
bool auxIsEnabled;
byte TPSlast; /**< The previous TPS reading */
unsigned long TPS_time; //The time the TPS sample was taken
unsigned long TPSlast_time; //The time the previous TPS sample was taken
byte MAPlast; /**< The previous MAP reading */
unsigned long MAP_time; //The time the MAP sample was taken
unsigned long MAPlast_time; //The time the previous MAP sample was taken
volatile unsigned long vssLastPulseTime; /**< The times of the last VSS_NUM_SAMPLES pulses of the VSS are stored in this array */
volatile unsigned long vssLastMinusOnePulseTime; /**< The times of the last VSS_NUM_SAMPLES pulses of the VSS are stored in this array */

//These variables are used for tracking the number of running sensors values that appear to be errors. Once a threshold is reached, the sensor reading will go to default value and assume the sensor is faulty
byte mapErrorCount = 0;
byte iatErrorCount = 0;
byte cltErrorCount = 0;

/**
 * @brief Simple low pass IIR filter macro for the analog inputs
 * This is effectively implementing the smooth filter from playground.arduino.cc/Main/Smooth
 * But removes the use of floats and uses 8 bits of fixed precision.
 */
#define ADC_FILTER(input, alpha, prior) (((long)input * (256 - alpha) + ((long)prior * alpha))) >> 8

static inline void instanteneousMAPReading() __attribute__((always_inline));
static inline void readMAP() __attribute__((always_inline));
static inline void validateMAP();
void initialiseADC();
void readTPS(bool=true); //Allows the option to override the use of the filter
void readO2_2();
void flexPulse();
void vssPulse();
uint16_t getSpeed();
byte getGear();
byte getFuelPressure();
byte getOilPressure();
uint16_t readAuxanalog(uint8_t analogPin);
uint16_t readAuxdigital(uint8_t digitalPin);
void readCLT(bool=true); //Allows the option to override the use of the filter
void readIAT();
void readO2();
void readBat();
void readBaro();


#endif // SENSORS_H
