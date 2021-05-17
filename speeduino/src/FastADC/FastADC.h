/**
  ******************************************************************************
  * @file    FastADC.h
  * @author  Teemo Vaas
  * @brief   Faster ADC reading 
  ******************************************************************************
  */
#ifndef FASTADC_H
#define FASTADC_H

#include "stm32f4xx_hal.h"

/* Variable containing ADC conversions results */
#define ADCCONVERTEDVALUES_BUFFER_SIZE 6
__IO uint16_t   aADC2ConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];
__IO uint16_t   aADC3ConvertedValues; //This is currently single, dedicated only for MAP sensor reading


void FastADCsetup(void);

#endif