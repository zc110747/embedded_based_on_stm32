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

#if DRIVER_RTC_ENABLE == 1
#include "drv_rtc.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
