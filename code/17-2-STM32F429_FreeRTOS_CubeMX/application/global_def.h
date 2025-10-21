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

//global module defined
#define LOGGER_MODULE_ON        1

//global drvier defined
#define DRIVER_GPIO_ENABLE          1
#define DRIVER_EXTI_ENABLE          0
#define DRIVER_USART_ENABLE         0
#define DRIVER_RTC_ENABLE           0
#define DRIVER_DAC_ENABLE           0
#define DRIVER_ADC_ENABLE           0
#define DRIVER_SPI_ENABLE           0
#define DRIVER_I2C_ENABLE           0
#define DRIVER_I2C_PCF8574_ENABLE   1
#define DRIVER_TIME_ENABLE          0
#define DRIVER_WDG_ENABLE           0
#define DRIVER_DMA_ENABLE           0
#define DRIVER_SDIO_ENABLE          0
#define DRIVER_SDRAM_ENABLE         0
#define DRIVER_LCD_ENABLE           0
#define DRIVER_CAN_ENABLE           0

#if DRIVER_LCD_ENABLE == 1
#define SUPPORT_ASCII_1206      1
#define SUPPORT_ASCII_1608      1
#define SUPPORT_ASCII_2412      1
#define SUPPORT_ASCII_3216      1
#endif

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
#define RUN_MODE_SOFT           3

//run os mode
#define RUN_WITH_RTOS_NULL      0
#define RUN_WITH_FREERTOS       1
#define RUN_WITH_RTTHREAD       2
#define RUN_OS_MODE             RUN_WITH_FREERTOS

#endif
