//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
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

/// logger supported define
#define LOG_MODULE_ON               1
#define LOG_DEFAULT_DEVICE          LOG_DEVICE_USART

// global drvier defined
#define DRIVER_LOG_UART_ENABLE      1
#define DRIVER_GPIO_ENABLE          0
#define DRIVER_EXTI_ENABLE          0
#define DRIVER_RTC_ENABLE           0

typedef enum
{
    RT_OK = 0,
    RT_FAIL,
}GlobalType_t;

#ifndef UINT_MAX
#define UINT_MAX    0xFFFFFFFF
#endif

#endif
