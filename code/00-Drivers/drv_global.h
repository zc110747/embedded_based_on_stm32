//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_global.h
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
#ifndef _DRV_GLOBAL_H
#define _DRV_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"
#include "SEGGER_RTT.h"

#if DRIVER_GPIO_ENABLE == 1
#include "drv_gpio.h"
#endif

#if DRIVER_EXTI_ENABLE == 1
#include "drv_exti.h"
#endif

#if DRIVER_USART_ENABLE == 1
#include "drv_usart.h"
#endif

#if DRIVER_RTC_ENABLE == 1
#include "drv_rtc.h"
#endif

#if DRIVER_DAC_ENABLE == 1
#include "drv_dac.h"
#endif

#if DRIVER_ADC_ENABLE == 1
#include "drv_adc.h"
#endif

#if DRIVER_SPI_ENABLE == 1
#include "drv_spi.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
