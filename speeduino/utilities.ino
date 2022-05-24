/*
  Speeduino - Simple engine management for the Arduino Mega 2560 platform
  Copyright (C) Josh Stewart
  A full copy of the license may be found in the projects root directory
*/
/** @file
 * Custom Programmable I/O.
 * The config related to Programmable I/O is found on configPage13 (of type @ref config13).
 */
#include <avr/pgmspace.h>
#include "globals.h"
#include "utilities.h"
#include "decoders.h"
#include "comms.h"
#include "logger.h"
#include "scheduler.h"
#include "scheduledIO.h"
#include "speeduino.h"

uint8_t ioDelay[sizeof(configPage13.outputPin)];
uint8_t ioOutDelay[sizeof(configPage13.outputPin)];
uint8_t pinIsValid = 0;
uint8_t currentRuleStatus = 0;


/** Translate between the pin list that appears in TS and the actual pin numbers.
For the **digital IO**, this will simply return the same number as the rawPin value as those are mapped directly.
For **analog pins**, it will translate them into the correct internal pin number.
* @param rawPin - High level pin number
* @return Translated / usable pin number
*/
byte pinTranslate(byte rawPin)
{
  byte outputPin = rawPin;
  if(rawPin > BOARD_MAX_DIGITAL_PINS) { outputPin = A8 + (outputPin - BOARD_MAX_DIGITAL_PINS - 1); }

  return outputPin;
}
/** Translate a pin number (0 - 22) to the relevant Ax (analog) pin reference.
* This is required as some ARM chips do not have all analog pins in order (EG pin A15 != A14 + 1).
* */
byte pinTranslateAnalog(byte rawPin)
{
  byte outputPin = rawPin;
  switch(rawPin)
  {
    case 0: outputPin = A0; break;
    case 1: outputPin = A1; break;
    case 2: outputPin = A2; break;
    case 3: outputPin = A3; break;
    case 4: outputPin = A4; break;
    case 5: outputPin = A5; break;
    case 6: outputPin = A6; break;
    case 7: outputPin = A7; break;
    case 8: outputPin = A8; break;
    case 9: outputPin = A9; break;
    case 10: outputPin = A10; break;
    case 11: outputPin = A11; break;
    case 12: outputPin = A12; break;
    case 13: outputPin = A13; break;
  #if BOARD_MAX_ADC_PINS >= 14
      case 14: outputPin = A14; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 15
      case 15: outputPin = A15; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 16
      case 16: outputPin = A16; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 17
      case 17: outputPin = A17; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 18
      case 18: outputPin = A18; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 19
      case 19: outputPin = A19; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 20
      case 20: outputPin = A20; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 21
      case 21: outputPin = A21; break;
    #endif
    #if BOARD_MAX_ADC_PINS >= 22
      case 22: outputPin = A22; break;
    #endif
  }

  return outputPin;
}


void setResetControlPinState()
{
  BIT_CLEAR(currentStatus.status3, BIT_STATUS3_RESET_PREVENT);

  /* Setup reset control initial state */
  switch (resetControl)
  {
    case RESET_CONTROL_PREVENT_WHEN_RUNNING:
      /* Set the reset control pin LOW and change it to HIGH later when we get sync. */
      digitalWrite(pinResetControl, LOW);
      BIT_CLEAR(currentStatus.status3, BIT_STATUS3_RESET_PREVENT);
      break;
    case RESET_CONTROL_PREVENT_ALWAYS:
      /* Set the reset control pin HIGH and never touch it again. */
      digitalWrite(pinResetControl, HIGH);
      BIT_SET(currentStatus.status3, BIT_STATUS3_RESET_PREVENT);
      break;
    case RESET_CONTROL_SERIAL_COMMAND:
      /* Set the reset control pin HIGH. There currently isn't any practical difference
         between this and PREVENT_ALWAYS but it doesn't hurt anything to have them separate. */
      digitalWrite(pinResetControl, HIGH);
      BIT_CLEAR(currentStatus.status3, BIT_STATUS3_RESET_PREVENT);
      break;
  }
}


/** Change injectors or/and ignition angles to 720deg.
 * Roll back req_fuel size and set number of outputs equal to cylinder count.
* */
void changeHalfToFullSync(void)
{
  //Need to do another check for injLayout as this function can be called from ignition
  if( (configPage2.injLayout == INJ_SEQUENTIAL) && (CRANK_ANGLE_MAX_INJ != 720) )
  {
    CRANK_ANGLE_MAX_INJ = 720;
    maxIgnOutputs = configPage2.nCylinders;
    req_fuel_uS *= 2;
    switch (configPage2.nCylinders)
    {
      case 4:
        setCallbacks(fuelSchedule1, openInjector1, closeInjector1);
        setCallbacks(fuelSchedule2, openInjector2, closeInjector2);
        fuelSchedule3.injEnabled = true;
        fuelSchedule4.injEnabled = true;
        break;
            
      case 6:
        setCallbacks(fuelSchedule1, openInjector1, closeInjector1);
        setCallbacks(fuelSchedule2, openInjector2, closeInjector2);
        setCallbacks(fuelSchedule3, openInjector3, closeInjector3);
        fuelSchedule4.injEnabled = true;
#if (INJ_CHANNELS >= 5)
        fuelSchedule5.injEnabled = true;
#endif
#if (INJ_CHANNELS >= 6)
        fuelSchedule6.injEnabled = true;
#endif
        break;

      case 8:
        setCallbacks(fuelSchedule1, openInjector1, closeInjector1);
        setCallbacks(fuelSchedule2, openInjector2, closeInjector2);
        setCallbacks(fuelSchedule3, openInjector3, closeInjector3);
        setCallbacks(fuelSchedule4, openInjector4, closeInjector4);
#if (INJ_CHANNELS >= 5)
        fuelSchedule5.injEnabled = true;
#endif
#if (INJ_CHANNELS >= 6)
        fuelSchedule6.injEnabled = true;
#endif
#if (INJ_CHANNELS >= 7)
        fuelSchedule7.injEnabled = true;
#endif
#if (INJ_CHANNELS >= 8)
        fuelSchedule8.injEnabled = true;
#endif
        break;

    }
  }

  //Need to do another check for sparkMode as this function can be called from injection
  if( (configPage4.sparkMode == IGN_MODE_SEQUENTIAL) && (CRANK_ANGLE_MAX_IGN != 720) )
  {
    CRANK_ANGLE_MAX_IGN = 720;
    maxIgnOutputs = configPage2.nCylinders;
    switch (configPage2.nCylinders)
    {
    case 4:
      setCallbacks(ignitionSchedule1, beginCoil1Charge, endCoil1Charge);
      setCallbacks(ignitionSchedule2, beginCoil2Charge, endCoil2Charge);
      break;

    case 6:
      setCallbacks(ignitionSchedule1, beginCoil1Charge, endCoil1Charge);
      setCallbacks(ignitionSchedule2, beginCoil2Charge, endCoil2Charge);
      setCallbacks(ignitionSchedule3, beginCoil3Charge, endCoil3Charge);
      break;

    case 8:
      setCallbacks(ignitionSchedule1, beginCoil1Charge, endCoil1Charge);
      setCallbacks(ignitionSchedule2, beginCoil2Charge, endCoil2Charge);
      setCallbacks(ignitionSchedule3, beginCoil3Charge, endCoil3Charge);
      setCallbacks(ignitionSchedule4, beginCoil4Charge, endCoil4Charge);
      break;
    }
  }
}

/** Change injectors or/and ignition angles to 360deg.
 * In semi sequentiol mode req_fuel size is half.
 * Set number of outputs equal to half cylinder count.
* */
void changeFullToHalfSync(void)
{
  if(configPage2.injLayout == INJ_SEQUENTIAL)
  {
    CRANK_ANGLE_MAX_INJ = 360;
    maxIgnOutputs = configPage2.nCylinders / 2;
    req_fuel_uS /= 2;
    switch (configPage2.nCylinders)
    {
      case 4:
        if(configPage4.inj4cylPairing == INJ_PAIR_13_24)
        {
          setCallbacks(fuelSchedule1, openInjector1and3, closeInjector1and3);
          setCallbacks(fuelSchedule2, openInjector2and4, closeInjector2and4);
        }
        else
        {
          setCallbacks(fuelSchedule1, openInjector1and4, closeInjector1and4);
          setCallbacks(fuelSchedule2, openInjector2and3, closeInjector2and3);
        }
        fuelSchedule3.injEnabled = false;
        fuelSchedule4.injEnabled = false;
        break;
            
      case 6:
        setCallbacks(fuelSchedule1, openInjector1and4, closeInjector1and4);
        setCallbacks(fuelSchedule2, openInjector2and5, closeInjector2and5);
        setCallbacks(fuelSchedule3, openInjector3and6, closeInjector3and6);
        fuelSchedule4.injEnabled = false;
#if (INJ_CHANNELS >= 5)
        fuelSchedule5.injEnabled = false;
#endif
#if (INJ_CHANNELS >= 6)
        fuelSchedule6.injEnabled = false;
#endif
        break;

      case 8:
        setCallbacks(fuelSchedule1, openInjector1and5, closeInjector1and5);
        setCallbacks(fuelSchedule2, openInjector2and6, closeInjector2and6);
        setCallbacks(fuelSchedule3, openInjector3and7, closeInjector3and7);
        setCallbacks(fuelSchedule4, openInjector4and8, closeInjector4and8);
#if (INJ_CHANNELS >= 5)
        fuelSchedule5.injEnabled = false;
#endif
#if (INJ_CHANNELS >= 6)
        fuelSchedule6.injEnabled = false;
#endif
#if (INJ_CHANNELS >= 7)
        fuelSchedule7.injEnabled = false;
#endif
#if (INJ_CHANNELS >= 8)
        fuelSchedule8.injEnabled = false;
#endif
        break;
    }
  }

  if(configPage4.sparkMode == IGN_MODE_SEQUENTIAL)
  {
    CRANK_ANGLE_MAX_IGN = 360;
    maxIgnOutputs = configPage2.nCylinders / 2;
    switch (configPage2.nCylinders)
    {
      case 4:
        setCallbacks(ignitionSchedule1, beginCoil1and3Charge, endCoil1and3Charge);
        setCallbacks(ignitionSchedule2, beginCoil2and4Charge, endCoil2and4Charge);
        break;
            
      case 6:
        setCallbacks(ignitionSchedule1, beginCoil1and4Charge, endCoil1and4Charge);
        setCallbacks(ignitionSchedule2, beginCoil2and5Charge, endCoil2and5Charge);
        setCallbacks(ignitionSchedule3, beginCoil3and6Charge, endCoil3and6Charge);
        break;

      case 8:
        setCallbacks(ignitionSchedule1, beginCoil1and5Charge, endCoil1and5Charge);
        setCallbacks(ignitionSchedule2, beginCoil2and6Charge, endCoil2and6Charge);
        setCallbacks(ignitionSchedule3, beginCoil3and7Charge, endCoil3and7Charge);
        setCallbacks(ignitionSchedule4, beginCoil4and8Charge, endCoil4and8Charge);
        break;
    }
  }
}
//*********************************************************************************************************************************************************************************
void initialiseProgrammableIO()
{
  uint8_t outputPin;
  for (uint8_t y = 0; y < sizeof(configPage13.outputPin); y++)
  {
    ioDelay[y] = 0;
    ioOutDelay[y] = 0;
    outputPin = configPage13.outputPin[y];
    if (outputPin > 0)
    {
      if ( outputPin >= 128 ) //Cascate rule usage
      {
        BIT_WRITE(currentStatus.outputsStatus, y, BIT_CHECK(configPage13.outputInverted, y));
        BIT_SET(pinIsValid, y);
      }
      else if ( !pinIsUsed(outputPin) )
      {
        pinMode(outputPin, OUTPUT);
        digitalWrite(outputPin, BIT_CHECK(configPage13.outputInverted, y));
        BIT_WRITE(currentStatus.outputsStatus, y, BIT_CHECK(configPage13.outputInverted, y));
        BIT_SET(pinIsValid, y);
      }
      else { BIT_CLEAR(pinIsValid, y); }
    }
  }
}
/** Check all (8) programmable I/O:s and carry out action on output pin as needed.
 * Compare 2 (16 bit) vars in a way configured by @ref cmpOperation (see also @ref config13.operation).
 * Use ProgrammableIOGetData() to get 2 vars to compare.
 * Skip all programmable I/O:s where output pin is set 0 (meaning: not programmed).
 */
void checkProgrammableIO()
{
  int16_t data, data2;
  uint8_t dataRequested;
  bool firstCheck, secondCheck;

  for (uint8_t y = 0; y < sizeof(configPage13.outputPin); y++)
  {
    firstCheck = false;
    secondCheck = false;
    if ( BIT_CHECK(pinIsValid, y) ) //if outputPin == 0 it is disabled
    {
      dataRequested = configPage13.firstDataIn[y];
      if ( dataRequested > 239U ) //Somehow using 239 uses 9 bytes of RAM, why??
      {
        dataRequested -= REUSE_RULES;
        if ( dataRequested <= sizeof(configPage13.outputPin) ) { data = BIT_CHECK(currentRuleStatus, dataRequested); }
        else { data = 0; }
      }
      else { data = ProgrammableIOGetData(dataRequested); }
      data2 = configPage13.firstTarget[y];

      if ( (configPage13.operation[y].firstCompType == COMPARATOR_EQUAL) && (data == data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_NOT_EQUAL) && (data != data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_GREATER) && (data > data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_GREATER_EQUAL) && (data >= data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_LESS) && (data < data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_LESS_EQUAL) && (data <= data2) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_AND) && ((data & data2) != 0) ) { firstCheck = true; }
      else if ( (configPage13.operation[y].firstCompType == COMPARATOR_XOR) && ((data ^ data2) != 0) ) { firstCheck = true; }

      if (configPage13.operation[y].bitwise != BITWISE_DISABLED)
      {
        dataRequested = configPage13.secondDataIn[y];
        if ( dataRequested <= (REUSE_RULES + sizeof(configPage13.outputPin)) ) //Failsafe check
        {
          if ( dataRequested > 239U ) //Somehow using 239 uses 9 bytes of RAM, why??
          {
            dataRequested -= REUSE_RULES;
            data = BIT_CHECK(currentRuleStatus, dataRequested);
          }
          else { data = ProgrammableIOGetData(dataRequested); }
          data2 = configPage13.secondTarget[y];
          
          if ( (configPage13.operation[y].secondCompType == COMPARATOR_EQUAL) && (data == data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_NOT_EQUAL) && (data != data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_GREATER) && (data > data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_GREATER_EQUAL) && (data >= data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_LESS) && (data < data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_LESS_EQUAL) && (data <= data2) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_AND) && ((data & data2) != 0) ) { secondCheck = true; }
          else if ( (configPage13.operation[y].secondCompType == COMPARATOR_XOR) && ((data ^ data2) != 0) ) { secondCheck = true; }

          if (configPage13.operation[y].bitwise == BITWISE_AND) { firstCheck &= secondCheck; }
          if (configPage13.operation[y].bitwise == BITWISE_OR) { firstCheck |= secondCheck; }
          if (configPage13.operation[y].bitwise == BITWISE_XOR) { firstCheck ^= secondCheck; }
        }
      }

      //If the limiting time is active(>0) and using maximum time
      if (BIT_CHECK(configPage13.kindOfLimiting, y))
      {
        if(firstCheck)
        {
          if ((configPage13.outputTimeLimit[y] != 0) && (ioOutDelay[y] >= configPage13.outputTimeLimit[y])) { firstCheck = false; } //Time has counted, disable the output
        }
        else
        {
          //Released before Maximum time, set delay to maximum to flip the output next
          if(BIT_CHECK(currentStatus.outputsStatus, y)) { ioOutDelay[y] = configPage13.outputTimeLimit[y]; }
          else { ioOutDelay[y] = 0; } //Reset the counter for next time
        }
      }

      if ( (firstCheck == true) && (configPage13.outputDelay[y] < 255) )
      {
        if (ioDelay[y] >= configPage13.outputDelay[y])
        {
          bool bitStatus = BIT_CHECK(configPage13.outputInverted, y) ^ firstCheck;
          if (BIT_CHECK(currentStatus.outputsStatus, y) && (ioOutDelay[y] < configPage13.outputTimeLimit[y])) { ioOutDelay[y]++; }
          if (configPage13.outputPin[y] < 128) { digitalWrite(configPage13.outputPin[y], bitStatus); }
          else { BIT_WRITE(currentRuleStatus, y, bitStatus); }
          BIT_WRITE(currentStatus.outputsStatus, y, bitStatus);
        }
        else { ioDelay[y]++; }
      }
      else
      {
        if (ioOutDelay[y] >= configPage13.outputTimeLimit[y])
        {
          bool bitStatus = BIT_CHECK(configPage13.outputInverted, y) ^ firstCheck;
          if (configPage13.outputPin[y] < 128) { digitalWrite(configPage13.outputPin[y], bitStatus); }
          else { BIT_WRITE(currentRuleStatus, y, bitStatus); }
          BIT_WRITE(currentStatus.outputsStatus, y, bitStatus);
          if(!BIT_CHECK(configPage13.kindOfLimiting, y)) { ioOutDelay[y] = 0; }
        }
        else { ioOutDelay[y]++; }

        ioDelay[y] = 0;
      }
    }
  }
}
/** Get single I/O data var (from currentStatus) for comparison.
 * Uses member offset index @ref fsIntIndex to lookup realtime 'live' data from @ref currentStatus.
 * @param index - Field index/number (?)
 * @return 16 bit (int) result
 */
int16_t ProgrammableIOGetData(uint16_t index)
{
  int16_t result;
  if ( index < LOG_ENTRY_SIZE )
  {
    /*
    for(uint8_t x = 0; x<sizeof(fsIntIndex); x++)
    {
      // Stop at desired field
      if (pgm_read_byte(&(fsIntIndex[x])) == index) { break; }
    }
    if (x >= sizeof(fsIntIndex)) { result = getTSLogEntry(index); } // 8-bit, coerce to 16 bit result
    else { result = word(getTSLogEntry(index+1), getTSLogEntry(index)); } // Assemble 2 bytes to word of 16 bit result
    */
    if(is2ByteEntry(index)) { result = word(getTSLogEntry(index+1), getTSLogEntry(index)); }
    else { result = getTSLogEntry(index); }
    

    //Special cases for temperatures
    if( (index == 6) || (index == 7) ) { result -= CALIBRATION_TEMPERATURE_OFFSET; }
  }
  else if ( index == 239U ) { result = (int16_t)max((uint32_t)runSecsX10, (uint32_t)32768); } //STM32 used std lib
  else { result = -1; } //Index is bigger than fullStatus array
  return result;
}
    //Handle any of the hardware testing outputs
void testOutputs()
{
    if( BIT_CHECK(currentStatus.testOutputs, 1) )
    {
      //Check whether any of the fuel outputs is on

      //Check for injector outputs on 50%
      if(BIT_CHECK(HWTest_INJ_50pc, INJ1_CMD_BIT)) { injector1Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ2_CMD_BIT)) { injector2Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ3_CMD_BIT)) { injector3Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ4_CMD_BIT)) { injector4Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ5_CMD_BIT)) { injector5Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ6_CMD_BIT)) { injector6Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ7_CMD_BIT)) { injector7Toggle(); }
      if(BIT_CHECK(HWTest_INJ_50pc, INJ8_CMD_BIT)) { injector8Toggle(); }

      //Check for ignition outputs on 50%
      if(BIT_CHECK(HWTest_IGN_50pc, IGN1_CMD_BIT)) { coil1Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN2_CMD_BIT)) { coil2Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN3_CMD_BIT)) { coil3Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN4_CMD_BIT)) { coil4Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN5_CMD_BIT)) { coil5Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN6_CMD_BIT)) { coil6Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN7_CMD_BIT)) { coil7Toggle(); }
      if(BIT_CHECK(HWTest_IGN_50pc, IGN8_CMD_BIT)) { coil8Toggle(); }
    }
}
