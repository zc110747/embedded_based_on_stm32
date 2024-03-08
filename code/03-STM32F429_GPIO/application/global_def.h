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

//global drvier defined
#define DRIVER_GPIO_ENABLE      1
#define DRIVER_EXTI_ENABLE      0
#define DRIVER_RTC_ENABLE       0
#define DRIVER_DAC_ENABLE       0
#define DRIVER_ADC_ENABLE       0

typedef enum
{
    RT_OK = 0,
    RT_FAIL,
}GlobalType_t;

#ifndef UINT_MAX
#define UINT_MAX    0xFFFFFFFF
#endif

#define RUN_MODE_NORMAL         0
#define RUN_MODE_DMA            1
#define RUN_MODE_INTERRUPT      2

#define RUN_WITH_RTOS_NULL  0
#define RUN_WITH_FREERTOS   1
#define RUN_WITH_RTTHREAD   2

#define RUN_OS_MODE         RUN_WITH_RTOS_NULL
#endif
