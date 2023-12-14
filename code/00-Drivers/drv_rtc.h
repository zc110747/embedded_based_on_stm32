//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_rtc.h
//
//  Purpose:
//      driver for rtc
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_RTC_H
#define _DRV_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}RTC_TIME_DEF;

typedef struct
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
}RTC_INFO;

void rtc_update(void);
void rtc_get_info(RTC_INFO *pGetInfo);
void rtc_alarm_register(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t get_rtc_alarm(void);
void clear_rtc_alarm(void);
GlobalType_t rtc_driver_init(void);

#ifdef __cplusplus
}
#endif

#endif
