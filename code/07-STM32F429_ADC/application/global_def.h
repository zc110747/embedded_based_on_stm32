//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      global_def.h
//
//  Purpose:
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _GLOBAL_DEF_H
#define _GLOBAL_DEF_H

typedef enum
{
    RT_OK = 0,
    RT_FAIL,
}GlobalType_t;

#ifndef UINT_MAX
#define UINT_MAX    0xFFFFFFFF
#endif

#define NORMAL_MODE             0
#define DMA_MODE                1
#define INTERRUPT               2

#define DRIVER_GPIO_ENABLE      1
#define DRIVER_EXTI_ENABLE      0
#define DRIVER_RTC_ENABLE       0
#define DRIVER_DAC_ENABLE       0
#define DRIVER_ADC_ENABLE       1

#if DRIVER_DAC_ENABLE == 1
#define DAC_RUN_MODE            DMA_MODE
#endif

#if DRIVER_ADC_ENABLE == 1
#define ADC_RUN_MODE            DMA_MODE
#endif

#endif
