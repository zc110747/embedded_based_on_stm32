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
#include "drv_target.h"

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
#include "drv_spi_wq.h"
#endif

#if DRIVER_I2C_ENABLE == 1
#include "drv_i2c_ap3216.h"
#endif

#if DRIVER_DS3213_ENABLE == 1
#include "drv_i2c_ds3213.h"
#endif

#if DRIVER_PCF8563_ENABLE == 1
#include "drv_i2c_pcf8563.h"
#endif

#if DRIVER_PCF8574_ENABLE == 1
#include "drv_i2c_pcf8574.h"
#endif

#if DRIVER_TIME_ENABLE == 1
#include "drv_timer.h"
#endif

#if DRIVER_WDG_ENABLE == 1
#include "drv_wdg.h"
#endif

#if DRIVER_DMA_ENABLE == 1
#include "drv_dma.h"
#endif

#if DRIVER_SDIO_ENABLE == 1
#include "drv_sdio.h"
#endif

#if DRIVER_SDRAM_ENABLE == 1
#include "drv_sdram.h"
#endif

#if DRIVER_LCD_ENABLE == 1
#include "drv_lcd.h"
#endif

#if DRIVER_CAN_ENABLE == 1
#include "drv_can.h"
#endif

#if DRIVER_ALG_ENABLE == 1
#include "drv_alg.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
