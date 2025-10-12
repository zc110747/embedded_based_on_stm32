//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.h
//
//  Purpose:
//      i2c driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_I2C_PCF8563_H
#define _DRV_I2C_PCF8563_H

#include "includes.h"

#define PCF8563_ADDR 	            0x51
#define PCF8563_I2C_TIMEOUT         10

#define PCF8563_REG_CONTROL1        0x00
#define PCF8563_REG_CONTROL2        0x01
#define PCF8563_REG_SECONDS         0x02
#define PCF8563_REG_MINUTES         0x03
#define PCF8563_REG_HOURS           0x04
#define PCF8563_REG_DAYS            0x05
#define PCF8563_REG_WEEKDAYS        0x06
#define PCF8563_REG_MONTHS          0x07
#define PCF8563_REG_YEARS           0x08
#define PCF8563_REG_ALARM_MINUTE    0x09
#define PCF8563_REG_ALARM_HOUR      0x0A
#define PCF8563_REG_ALARM_DAY       0x0B
#define PCF8563_REG_ALARM_WEEKDAY   0x0C
#define PCF8563_REG_TIMER_CLK       0x0D
#define PCF8563_REG_TIMER_CTL       0x0E
#define PCF8563_REG_TIMER_COUNTDOWN 0x0F

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
}RTC_TimerInfo;

typedef struct
{
    uint8_t minutes;
    uint8_t hour;
    uint8_t weekday;
}RTC_AlarmInfo;

#ifdef __cplusplus
extern "C" {
#endif

//rtc read information
GlobalType_t pcf8563_driver_init(void);
GlobalType_t pcf8563_update_timer(void);
GlobalType_t pcf8563_read_timer(RTC_TimerInfo *info);
GlobalType_t pcf8563_set_timer(RTC_TimerInfo *info);
void get_rtc_pcf8563_info(RTC_TimerInfo *rtc_info);

//rtc alarm information
GlobalType_t pcf8563_control_alarm(FunctionalState mode);
GlobalType_t pcf8563_set_alarm(RTC_AlarmInfo *Info);
GlobalType_t pcf8563_ctrl_alarm(FunctionalState state);
RTC_AlarmInfo pcf8563_formward_minute(RTC_TimerInfo *info, uint8_t minute);
#ifdef __cplusplus
}
#endif

#endif
